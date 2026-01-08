#include "buddy_allocator.h"

#include <iostream>
#include <map>
#include <set>
#include <unordered_map>
#include <cmath>
#include <iomanip>

using namespace std;

// ==========================
// Internal structures
// ==========================

// free_lists[size] = set of starting addresses
static map<size_t, set<size_t>> free_lists;

// allocated_blocks[id] = (address, size)
static unordered_map<int, pair<size_t, size_t>> allocated_blocks;

// allocator state
static size_t TOTAL_MEMORY = 0;
static int next_id = 1;

// ==========================
// Utility helpers
// ==========================

static bool is_power_of_two(size_t x) {
    return x && ((x & (x - 1)) == 0);
}

static size_t next_power_of_two(size_t x) {
    if (is_power_of_two(x)) return x;
    size_t p = 1;
    while (p < x) p <<= 1;
    return p;
}

// ==========================
// Initialize buddy allocator
// ==========================
void buddy_init(size_t total_size) {
    if (!is_power_of_two(total_size)) {
        cout << "Buddy allocator requires power-of-two memory size\n";
        return;
    }

    free_lists.clear();
    allocated_blocks.clear();

    TOTAL_MEMORY = total_size;
    next_id = 1;

    // one big free block
    free_lists[total_size].insert(0);

    cout << "Buddy allocator initialized with "
         << total_size << " bytes\n";
}

// ==========================
// Buddy malloc
// ==========================
int buddy_malloc(size_t req_size) {
    if (TOTAL_MEMORY == 0) {
        cout << "Buddy allocator not initialized\n";
        return -1;
    }

    size_t need = next_power_of_two(req_size);

    // find smallest available block >= need
    auto it = free_lists.lower_bound(need);
    if (it == free_lists.end()) {
        return -1;
    }

    size_t block_size = it->first;
    size_t addr = *it->second.begin();

    // remove chosen block
    it->second.erase(it->second.begin());
    if (it->second.empty())
        free_lists.erase(it);

    // split until exact size reached
    while (block_size > need) {
        block_size >>= 1;

        size_t buddy_addr = addr + block_size;

        // IMPORTANT: lower address stays allocated path,
        // higher address goes to free list
        free_lists[block_size].insert(buddy_addr);
    }

    int id = next_id++;
    allocated_blocks[id] = {addr, need};
    return id;
}

// ==========================
// Buddy free
// ==========================
bool buddy_free(int id) {
    auto it = allocated_blocks.find(id);
    if (it == allocated_blocks.end())
        return false;

    size_t addr = it->second.first;
    size_t size = it->second.second;

    allocated_blocks.erase(it);

    // try recursive coalescing
    while (size < TOTAL_MEMORY) {
        size_t buddy = addr ^ size;

        auto &set_ref = free_lists[size];
        auto buddy_it = set_ref.find(buddy);

        if (buddy_it == set_ref.end())
            break;

        // merge with buddy
        set_ref.erase(buddy_it);
        if (set_ref.empty())
            free_lists.erase(size);

        addr = min(addr, buddy);
        size <<= 1;
    }

    free_lists[size].insert(addr);
    return true;
}

// ==========================
// Dump buddy allocator state
// ==========================
void buddy_dump() {
    cout << "\n=== Buddy Allocator Dump ===\n";

    cout << "Allocated blocks:\n";
    for (auto &p : allocated_blocks) {
        cout << "ID " << p.first
             << ": [0x" << hex << p.second.first
             << " - 0x" << (p.second.first + p.second.second - 1)
             << "] (" << dec << p.second.second << " bytes)\n";
    }

    cout << "\nFree blocks:\n";
    for (auto &fl : free_lists) {
        cout << "Size " << fl.first << ": ";
        for (auto addr : fl.second)
            cout << "0x" << hex << addr << " ";
        cout << dec << "\n";
    }

    cout << "============================\n\n";
}
