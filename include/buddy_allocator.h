#ifndef BUDDY_ALLOCATOR_H
#define BUDDY_ALLOCATOR_H

#include <cstddef>

void buddy_init(std::size_t total_size);
int  buddy_malloc(std::size_t req_size);
bool buddy_free(int id);
void buddy_dump();

#endif
