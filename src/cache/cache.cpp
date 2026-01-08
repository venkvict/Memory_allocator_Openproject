#include "cache.h"

#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

// ==========================
// Latencies
// ==========================
static const size_t L1_LATENCY = 1;
static const size_t L2_LATENCY = 10;
static const size_t MEMORY_LATENCY = 100;

// ==========================
// Cache implementation
// ==========================
Cache::Cache()
    : cache_size(0),
      block_size(0),
      associativity(0),
      num_sets(0),
      global_time(0),
      hits(0),
      misses(0) {}

void Cache::init(size_t csize,
                 size_t bsize,
                 size_t assoc,
                 const string &name)
{
    cache_size = csize;
    block_size = bsize;
    associativity = assoc;
    level_name = name;

    num_sets = cache_size / (block_size * associativity);
    sets.assign(num_sets, vector<Line>(associativity, {false, 0, 0}));

    hits = misses = 0;
    global_time = 0;

    cout << level_name << " initialized: "
         << cache_size << " bytes, "
         << block_size << "B block, "
         << associativity << "-way, "
         << num_sets << " sets\n";
}

bool Cache::is_initialized() const
{
    return cache_size != 0;
}

bool Cache::access(size_t address)
{
    size_t block_addr = address / block_size;
    size_t set_index = block_addr % num_sets;
    size_t tag = block_addr / num_sets;

    for (auto &line : sets[set_index])
    {
        if (line.valid && line.tag == tag)
        {
            hits++;
            return true;
        }
    }

    misses++;
    return false;
}

void Cache::insert(size_t address)
{
    size_t block_addr = address / block_size;
    size_t set_index = block_addr % num_sets;
    size_t tag = block_addr / num_sets;

    // empty slot
    for (auto &line : sets[set_index])
    {
        if (!line.valid)
        {
            line.valid = true;
            line.tag = tag;
            line.fifo_time = global_time++;
            return;
        }
    }

    // FIFO eviction
    size_t victim = 0;
    for (size_t i = 1; i < associativity; i++)
    {
        if (sets[set_index][i].fifo_time <
            sets[set_index][victim].fifo_time)
            victim = i;
    }

    sets[set_index][victim].tag = tag;
    sets[set_index][victim].fifo_time = global_time++;
}

void Cache::invalidate_range(size_t start, size_t end)
{
    for (auto &set : sets)
    {
        for (auto &line : set)
        {
            if (!line.valid) continue;

            size_t block_addr = line.tag * block_size;

            if (block_addr >= start && block_addr < end)
            {
                line.valid = false;
            }
        }
    }
}

void Cache::print_stats() const
{
    size_t total = hits + misses;
    double hit_rate = (total == 0) ? 0.0 : (double)hits / total;

    cout << "\n=== " << level_name << " Cache Statistics ===\n";
    cout << "Hits: " << hits << "\n";
    cout << "Misses: " << misses << "\n";
    cout << "Total Accesses: " << total << "\n";
    cout << "Hit Rate: " << fixed << setprecision(2)
         << hit_rate * 100 << "%\n";
}

// ==========================
// CacheSystem implementation
// ==========================
CacheSystem::CacheSystem() {}

void CacheSystem::init_L1(size_t size,
                          size_t block,
                          size_t assoc)
{
    L1.init(size, block, assoc, "L1");
}

void CacheSystem::init_L2(size_t size,
                          size_t block,
                          size_t assoc)
{
    L2.init(size, block, assoc, "L2");
}

void CacheSystem::invalidate_frame(size_t frame,
                                   size_t page_size)
{
    size_t start = frame * page_size;
    size_t end   = start + page_size;

    if (L1.is_initialized())
        L1.invalidate_range(start, end);

    if (L2.is_initialized())
        L2.invalidate_range(start, end);
}

void CacheSystem::access(size_t address)
{
    size_t total_cycles = 0;

    total_cycles += L1_LATENCY;
    if (L1.is_initialized() && L1.access(address))
    {
        cout << "L1 HIT\n";
        cout << "Total access time: " << total_cycles << " cycles\n";
        return;
    }
    cout << "L1 MISS\n";

    total_cycles += L2_LATENCY;
    if (L2.is_initialized() && L2.access(address))
    {
        cout << "L2 HIT\n";
        L1.insert(address);
        cout << "Total access time: " << total_cycles << " cycles\n";
        return;
    }

    cout << "L2 MISS — fetching from memory\n";
    total_cycles += MEMORY_LATENCY;

    L2.insert(address);
    L1.insert(address);

    cout << "Total access time: " << total_cycles << " cycles\n";
}

void CacheSystem::print_stats() const
{
    if (L1.is_initialized())
        L1.print_stats();
    if (L2.is_initialized())
        L2.print_stats();
}
