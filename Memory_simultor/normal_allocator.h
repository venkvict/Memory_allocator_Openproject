#ifndef NORMAL_ALLOCATOR_H
#define NORMAL_ALLOCATOR_H

#include <cstddef>

// Allocation strategies
enum AllocatorType {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
};

// API exposed to main.cpp
void init_memory(std::size_t total_size);
void set_allocator(AllocatorType type);

int malloc_block(std::size_t req_size);
bool free_block(int id);

void dump_memory();
void print_stats();

#endif
