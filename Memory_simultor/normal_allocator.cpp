#include "normal_allocator.h"

#include <iostream>
#include <list>
#include <iomanip>
#include <algorithm>

using namespace std;

// ==========================
// Block representation
// ==========================
struct Block {
    size_t start;
    size_t size;
    bool free;
    int id;
};

// Memory as list of blocks
static list<Block> memory_blocks;

// ID generator
static int next_block_id = 1;

// Allocation statistics
static size_t total_alloc_requests = 0;
static size_t successful_allocs = 0;
static size_t failed_allocs = 0;

// Total memory size
static size_t TOTAL_MEMORY = 0;

// Current allocator
static AllocatorType current_allocator = FIRST_FIT;

// ==========================
// Initialize memory
// ==========================
void init_memory(size_t total_size) {
    memory_blocks.clear();
    memory_blocks.push_back({0, total_size, true, -1});
    next_block_id = 1;

    TOTAL_MEMORY = total_size;
    total_alloc_requests = 0;
    successful_allocs = 0;
    failed_allocs = 0;
}

// ==========================
// Set allocator
// ==========================
void set_allocator(AllocatorType type) {
    current_allocator = type;
}

// ==========================
// Dump memory
// ==========================
void dump_memory() {
    cout << "Memory Layout:\n";
    for (auto &blk : memory_blocks) {
        cout << "[" << blk.start
             << " - " << blk.start + blk.size - 1 << "] ";
        if (blk.free)
            cout << "FREE\n";
        else
            cout << "USED (id=" << blk.id << ")\n";
    }
    cout << "-----------------------------\n";
}

// ==========================
// Unified malloc
// ==========================
int malloc_block(size_t req_size) {
    total_alloc_requests++;

    auto chosen = memory_blocks.end();

    for (auto it = memory_blocks.begin(); it != memory_blocks.end(); ++it) {
        if (!it->free || it->size < req_size)
            continue;

        if (current_allocator == FIRST_FIT) {
            chosen = it;
            break;
        }
        else if (current_allocator == BEST_FIT) {
            if (chosen == memory_blocks.end() || it->size < chosen->size)
                chosen = it;
        }
        else if (current_allocator == WORST_FIT) {
            if (chosen == memory_blocks.end() || it->size > chosen->size)
                chosen = it;
        }
    }

    if (chosen == memory_blocks.end()) {
        failed_allocs++;
        return -1;
    }

    successful_allocs++;
    int id = next_block_id++;

    if (chosen->size == req_size) {
        chosen->free = false;
        chosen->id = id;
    } else {
        Block allocated = {chosen->start, req_size, false, id};
        Block remaining = {
            chosen->start + req_size,
            chosen->size - req_size,
            true,
            -1
        };

        auto pos = memory_blocks.erase(chosen);
        memory_blocks.insert(pos, allocated);
        memory_blocks.insert(pos, remaining);
    }

    return id;
}

// ==========================
// Free + coalescing
// ==========================
bool free_block(int id) {
    for (auto it = memory_blocks.begin(); it != memory_blocks.end(); ++it) {
        if (!it->free && it->id == id) {
            it->free = true;
            it->id = -1;

            // Merge with previous
            if (it != memory_blocks.begin()) {
                auto prev = it; --prev;
                if (prev->free) {
                    prev->size += it->size;
                    it = memory_blocks.erase(it);
                    it = prev;
                }
            }

            // Merge with next
            auto next = it; ++next;
            if (next != memory_blocks.end() && next->free) {
                it->size += next->size;
                memory_blocks.erase(next);
            }
            return true;
        }
    }
    return false;
}

// ==========================
// Statistics helpers
// ==========================
static size_t total_used() {
    size_t sum = 0;
    for (auto &b : memory_blocks)
        if (!b.free)
            sum += b.size;
    return sum;
}

static size_t total_free() {
    size_t sum = 0;
    for (auto &b : memory_blocks)
        if (b.free)
            sum += b.size;
    return sum;
}

static size_t largest_free_block() {
    size_t mx = 0;
    for (auto &b : memory_blocks)
        if (b.free)
            mx = max(mx, b.size);
    return mx;
}

// ==========================
// Print stats
// ==========================
void print_stats() {
    size_t used = total_used();
    size_t free = total_free();
    size_t largest = largest_free_block();

    cout << "\n===== MEMORY STATISTICS =====\n";

    double utilization = (TOTAL_MEMORY == 0)
        ? 0.0
        : (double)used / TOTAL_MEMORY;

    cout << "Total Memory: " << TOTAL_MEMORY << " bytes\n";
    cout << "Used Memory: " << used << " bytes\n";
    cout << "Free Memory: " << free << " bytes\n";
    cout << "Memory Utilization: "
         << fixed << setprecision(2)
         << utilization * 100 << "%\n";

    cout << "Internal Fragmentation: 0 bytes\n";

    if (free > 0) {
        double ext_frag = 1.0 - (double)largest / free;
        cout << "External Fragmentation: "
             << fixed << setprecision(2)
             << ext_frag * 100 << "%\n";
    } else {
        cout << "External Fragmentation: 0%\n";
    }

    cout << "Allocation Requests: " << total_alloc_requests << "\n";
    cout << "Successful Allocations: " << successful_allocs << "\n";
    cout << "Failed Allocations: " << failed_allocs << "\n";

    if (total_alloc_requests > 0) {
        double success_rate =
            (double)successful_allocs / total_alloc_requests;
        cout << "Allocation Success Rate: "
             << fixed << setprecision(2)
             << success_rate * 100 << "%\n";
    }

    cout << "==============================\n\n";
}
