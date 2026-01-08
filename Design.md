# Memory Management Simulator – Design Document

## 1. Overview

This document describes the design and internal architecture of the **Memory Management Simulator** implemented in C++.

The simulator models key operating system memory subsystems:

- Contiguous physical memory allocation
- Buddy allocation
- Multilevel CPU cache (L1, L2) with FIFO replacement
- Virtual memory with paging and FIFO page replacement
- Correct integration between Virtual Memory → Cache → Main Memory

The project is designed to be modular, extensible, and educational, closely reflecting real OS behavior while remaining simple to reason about.
## 2. High-Level Architecture

### 2.1 Component Interaction
```

    ┌──────────────────────────────────────────────┐
    │ CLI (main.cpp)                               │
    │ - Command parsing                            │
    │ - Mode switching                             │
    │ - Orchestration                              │
    └───────────────┬───────────────┬─────────────┘
                    │               │
    ┌──────────▼─────────┐   ┌───▼────────────┐
    │ Standard Allocator │   │ Buddy Allocator │
    │ (normal_allocator)│   │ (buddy_allocator)
    └───────────────────┘   └─────────────────┘
                    │
    ┌──────────▼─────────┐
    │ Cache System        │
    │ (L1 + L2 FIFO)      │
    └──────────┬─────────┘
               │
    ┌──────────▼─────────┐
    │ Virtual Memory      │
    │ (Paging + FIFO)     │
    └───────────────────┘
```

## 3. Module-wise Design

### 3.1 CLI & Control Flow (`main.cpp`)

#### Responsibilities
- Parse user commands
- Switch between modes: `standard`, `buddy`, `cache`, `vm`
- Route commands to correct subsystem
- Enforce execution order

#### Design Choices
- Single interactive loop using `getline`
- `stringstream` for tokenization
- `enum class Mode` for isolation
- Cross-mode command prevention

---

### 3.2 Standard Memory Allocator (`normal_allocator.cpp`)

#### Data Structure

    struct Block {
        size_t start;
        size_t size;
        bool free;
        int id;
    };

Memory representation:

    list<Block> memory_blocks

Always ordered by address.

#### Allocation Algorithm

    malloc(size):
    1. Traverse memory_blocks
    2. Select block based on strategy
    3. Split if larger
    4. Mark allocated
    5. Assign ID
    6. Update statistics

Allocated block is inserted **before** remaining free block.

#### Deallocation

    free(id):
    1. Locate block
    2. Mark free
    3. Merge neighbors

Fragmentation:
- Internal: 0
- External: `1 - (largest_free / total_free)`

---

### 3.3 Buddy Allocator (`buddy_allocator.cpp`)

#### Core Structures

    map<size_t, set<size_t>> free_lists;
    map<int, pair<size_t, size_t>> allocated_blocks;

#### Buddy Address

    buddy_address = address XOR block_size

#### Allocation

    buddy_malloc(size):
    1. Round to power of two
    2. Find smallest block
    3. Split recursively
    4. Allocate

#### Deallocation

    buddy_free(id):
    1. Retrieve address and size
    2. Find buddy
    3. Merge if free
    4. Repeat

### 3.4 Cache Simulator (`cache.cpp`)

Set-associative cache with FIFO replacement.

Address breakdown:

    num_sets = cache_size / (block_size × associativity)
    set_index = (address / block_size) % num_sets
    tag       = (address / block_size) / num_sets

Cache line:

    struct Line {
        bool valid;
        size_t tag;
        size_t fifo_time;
    };

FIFO victim = smallest `fifo_time`.

Latency:
- L1: 1 cycle
- L2: 10 cycles
- Memory: 100 cycles

---

### 3.5 Virtual Memory (`vm.cpp`)

Page table entry:

    struct PageTableEntry {
        bool valid;
        size_t frame;
    };

Translation:

    page   = VA / PAGE_SIZE
    offset = VA % PAGE_SIZE
    PA     = frame × PAGE_SIZE + offset

FIFO page replacement:
- Evict oldest page
- Invalidate page table
- Invalidate cache lines of reused frame

---

### 3.6 Integrated Access Flow

    User access(VA)
        ↓
    VM Translation (VA → PA)
        ↓
    Cache Access (PA)
        ↓
    L1 → L2 → Memory

Guarantees:
- Cache never sees virtual address
- Page fault and cache hit never coincide
- Cache consistency preserved

---

## 4. Testing Strategy

Tests in `tests/` include:
- Allocation behavior
- Fragmentation
- Buddy merge/split
- Cache eviction
- Page faults
- Integrated correctness

---

## 5. Assumptions & Limitations

Assumptions:
- Single process
- FIFO everywhere
- No disk I/O

Limitations:
- No TLB
- No write-back cache
- No segmentation
- No multi-process VM

---


## 7. Conclusion

This simulator accurately models OS memory subsystems with correct integration and clean modular design, making it suitable for both learning and extension.
