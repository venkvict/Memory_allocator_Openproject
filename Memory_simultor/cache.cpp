#include "cache.h"

#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

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
                 const string& name) {
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

bool Cache::is_initialized() const {
    return cache_size != 0;
}

bool Cache::access(size_t address) {
    size_t block_addr = address / block_size;
    size_t set_index = block_addr % num_sets;
    size_t tag = block_addr / num_sets;

    for (auto &line : sets[set_index]) {
        if (line.valid && line.tag == tag) {
            hits++;
            return true;
        }
    }

    misses++;
    return false;
}

void Cache::insert(size_t address) {
    size_t block_addr = address / block_size;
    size_t set_index = block_addr % num_sets;
    size_t tag = block_addr / num_sets;

    for (auto &line : sets[set_index]) {
        if (!line.valid) {
            line.valid = true;
            line.tag = tag;
            line.fifo_time = global_time++;
            return;
        }
    }

    size_t victim = 0;
    for (size_t i = 1; i < associativity; i++) {
        if (sets[set_index][i].fifo_time <
            sets[set_index][victim].fifo_time)
            victim = i;
    }

    sets[set_index][victim].tag = tag;
    sets[set_index][victim].fifo_time = global_time++;
}

void Cache::print_stats() const {
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
                          size_t assoc) {
    L1.init(size, block, assoc, "L1");
}

void CacheSystem::init_L2(size_t size,
                          size_t block,
                          size_t assoc) {
    L2.init(size, block, assoc, "L2");
}

void CacheSystem::access(size_t address) {
    if (L1.is_initialized() && L1.access(address)) {
        cout << "L1 HIT\n";
        return;
    }

    cout << "L1 MISS\n";

    if (L2.is_initialized() && L2.access(address)) {
        cout << "L2 HIT\n";
        L1.insert(address);
        return;
    }

    cout << "L2 MISS — fetching from memory\n";

    if (L2.is_initialized())
        L2.insert(address);
    if (L1.is_initialized())
        L1.insert(address);
}

void CacheSystem::print_stats() const {
    if (L1.is_initialized()) L1.print_stats();
    if (L2.is_initialized()) L2.print_stats();
}
