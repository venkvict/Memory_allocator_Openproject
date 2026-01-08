#include "vm.h"

#include <iostream>
#include <vector>
#include <queue>
#include "cache.h"
extern CacheSystem cache_system;

using namespace std;

// ==========================
// Page Table Entry
// ==========================
struct PageTableEntry
{
    bool valid;
    size_t frame;
};

// ==========================
// VM State
// ==========================
static vector<PageTableEntry> page_table;
static queue<size_t> fifo_queue;

static size_t PAGE_SIZE = 0;
static size_t NUM_FRAMES = 0;

static size_t page_faults = 0;
static size_t page_hits = 0;

// ==========================
// Helpers
// ==========================
bool vm_is_initialized()
{
    return PAGE_SIZE != 0;
}

// ==========================
// Init VM
// ==========================
void vm_init(size_t num_pages,
             size_t page_size,
             size_t num_frames)
{
    PAGE_SIZE = page_size;
    NUM_FRAMES = num_frames;

    page_table.assign(num_pages, {false, 0});
    while (!fifo_queue.empty())
        fifo_queue.pop();

    page_faults = 0;
    page_hits = 0;

    cout << "Virtual memory initialized:\n";
    cout << "Pages: " << num_pages
         << ", Page size: " << PAGE_SIZE
         << ", Frames: " << NUM_FRAMES << "\n";
}

// ==========================
// Address translation
// ==========================
bool vm_translate(size_t virtual_address, size_t &physical_address)
{
    if (!vm_is_initialized())
    {
        cout << "VM not initialized\n";
        physical_address = 0;
        return false;
    }

    size_t page = virtual_address / PAGE_SIZE;
    size_t offset = virtual_address % PAGE_SIZE;

    if (page >= page_table.size())
    {
        cout << "Invalid virtual address\n";
        physical_address = 0;
        return false;
    }

    bool page_fault = false;

    if (!page_table[page].valid)
    {
        page_fault = true;
        page_faults++;
        cout << "Page fault on page " << page << "\n";

        size_t frame;
        if (fifo_queue.size() < NUM_FRAMES)
        {
            frame = fifo_queue.size();
        }
        else
        {
            size_t victim = fifo_queue.front();
            fifo_queue.pop();

            frame = page_table[victim].frame;
            page_table[victim].valid = false;

            // 🔥 CRITICAL FIX: invalidate cache lines for this physical frame
            cache_system.invalidate_frame(frame, PAGE_SIZE);
        }

        page_table[page] = {true, frame};
        fifo_queue.push(page);
    }
    else
    {
        page_hits++;
    }

    physical_address = page_table[page].frame * PAGE_SIZE + offset;
    return page_fault;
}

// ==========================
// VM stats
// ==========================
void vm_stats()
{
    size_t total = page_hits + page_faults;
    double fault_rate = (total == 0) ? 0.0 : (double)page_faults / total;

    cout << "\n=== Virtual Memory Statistics ===\n";
    cout << "Page hits: " << page_hits << "\n";
    cout << "Page faults: " << page_faults << "\n";
    cout << "Total accesses: " << total << "\n";
    cout << "Page fault rate: "
         << fault_rate * 100 << "%\n";
}
