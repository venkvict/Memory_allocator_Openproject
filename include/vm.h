#ifndef VM_H
#define VM_H

#include <cstddef>

// Initialize virtual memory system
void vm_init(std::size_t num_pages,
             std::size_t page_size,
             std::size_t num_frames);

// Translate virtual address to physical address
// Returns true if page fault occurred
bool vm_translate(std::size_t virtual_address,
                  std::size_t &physical_address);

// Print VM statistics
void vm_stats();

// Check if VM is initialized
bool vm_is_initialized();

#endif
