# Memory Management Simulator

A comprehensive operating system memory management simulator implementing
contiguous memory allocation, buddy allocation, multilevel CPU cache
simulation, and virtual memory with paging. The simulator demonstrates how
different memory management subsystems interact in an OS-like environment.

---

## Features

### 1. Physical Memory Simulation
- Contiguous block of configurable physical memory
- Dynamic memory allocation and deallocation
- Explicit tracking of allocated and free blocks
- Automatic coalescing of adjacent free blocks

### 2. Memory Allocation Strategies
- **First Fit**: Allocates the first free block large enough
- **Best Fit**: Allocates the smallest free block that fits
- **Worst Fit**: Allocates the largest available free block
- Tracks fragmentation and allocation statistics

### 3. Buddy Allocation System
- Power-of-two memory allocation
- Recursive block splitting
- Buddy merging using XOR-based address computation
- Efficient handling of frequent allocations and deallocations

### 4. Multilevel Cache Simulation
- L1 and L2 cache support
- Configurable cache size, block size, and associativity
- FIFO (First-In-First-Out) replacement policy
- Hit/miss tracking and cache statistics
- Miss penalty propagation across cache levels

### 5. Virtual Memory System
- Paging-based virtual memory simulation
- Page table with valid bit and frame mapping
- FIFO page replacement policy
- Page fault handling
- Virtual to physical address translation

### 6. Integrated Memory Access Flow

Virtual Address → Page Table → Physical Address → Cache (L1 → L2) → Main Memory

- Cache accesses occur only after address translation
- Cache lines are invalidated when pages are evicted
- Prevents stale cache hits after page faults

## Project Structure
```
Project/
├── Makefile
├── bin/
│   └── memsim
├── build/
├── include/
│   ├── normal_allocator.h
│   ├── buddy_allocator.h
│   ├── cache.h
│   └── vm.h
├── src/
│   ├── main.cpp
│   ├── normal_allocator/
│   │   └── normal_allocator.cpp
│   ├── buddy_allocator/
│   │   └── buddy_allocator.cpp
│   ├── cache/
│   │   └── cache.cpp
│   └── vm/
│       └── vm.cpp
└── tests/

```

### Prerequisites
```

- C++ compiler supporting **C++17** (g++ or clang++)
- make
```

### Compilation
```
make
```

Executable location:
bin/memsim

### Clean Build
```

make clean
```

---

## Running the Simulator

### Interactive Mode
```
./bin/memsim
```

### Batch Mode
```

./bin/memsim < tests/vm_test.txt
```

---

## Command Reference

### Mode Selection
```

mode standard   → Contiguous memory allocator  
mode buddy      → Buddy allocator  
mode cache      → Cache simulator  
mode vm         → Virtual memory simulator  
```

---

### Standard Allocator Commands
```

init memory <size>  
set allocator <first|best|worst>  
malloc <size>  
free <block_id>  
dump  
stats  
```

---

### Buddy Allocator Commands
```

init memory <size>  
malloc <size>  
free <block_id>  
dump  
```

---

### Cache Commands
```

cache init L1 <size> <block_size> <associativity>  
cache init L2 <size> <block_size> <associativity>  
cache access <physical_address>  
stats  
```

---

### Virtual Memory Commands
```

vm init <num_pages> <page_size> <num_frames>  
access <virtual_address>  
stats  
```

## Integrated Example
```

mode cache  
cache init L1 512 64 2  
cache init L2 2048 64 4  

mode vm  
vm init 4 256 2  

access 0  
access 256  
access 512  
access 0  
stats  
```

---

## Metrics and Statistics

### Memory Allocator
```

- Total, used, free memory
- Internal and external fragmentation
- Allocation success and failure rate
```

### Cache
```

- Cache hits and misses
- Total accesses
- Hit rate
```

### Virtual Memory
```

- Page hits and page faults
- Page fault rate
```

---

## Testing

Run:
```

./bin/memsim < tests/integrated_test.txt
```

Compare with:
```

tests/integrated_output.txt
```

---

## Design Decisions

### Buddy Allocation
buddy_address = block_address XOR block_size

- Recursive splitting and merging
- Free lists indexed by block size

### Cache
- Set-associative cache
- FIFO replacement
- Physical-address-only indexing

### Virtual Memory
- FIFO page replacement
- Cache invalidation on page eviction

---

## Assumptions and Limitations

1. Single process simulation
2. FIFO replacement policies
3. No disk I/O
4. No write-back or write-through cache
5. No TLB simulation
6. Contiguous physical memory

---

## References
- Silberschatz, Galvin, Gagne — Operating System Concepts
- Tanenbaum — Modern Operating Systems
- Standard OS course material

