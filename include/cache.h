#ifndef CACHE_H
#define CACHE_H

#include <cstddef>
#include <string>
#include <vector>

// ==========================
// Cache (single level)
// ==========================
class Cache {
public:
    Cache();

    void init(std::size_t cache_size,
              std::size_t block_size,
              std::size_t associativity,
              const std::string& name);

    bool access(std::size_t address);
    void insert(std::size_t address);

    void print_stats() const;
    bool is_initialized() const;

    // 🔥 invalidate all cache lines mapping to [start, end)
    void invalidate_range(std::size_t start, std::size_t end);

private:
    struct Line {
        bool valid;
        std::size_t tag;
        std::size_t fifo_time;
    };

    std::string level_name;

    std::size_t cache_size;
    std::size_t block_size;
    std::size_t associativity;
    std::size_t num_sets;

    std::size_t global_time;
    std::size_t hits;
    std::size_t misses;

    std::vector<std::vector<Line>> sets;
};

// ==========================
// CacheSystem (L1 + L2)
// ==========================
class CacheSystem {
public:
    CacheSystem();

    void init_L1(std::size_t size,
                 std::size_t block,
                 std::size_t assoc);

    void init_L2(std::size_t size,
                 std::size_t block,
                 std::size_t assoc);

    void access(std::size_t address);
    void print_stats() const;

    // 🔥 invalidate cache lines for a physical frame
    void invalidate_frame(std::size_t frame,
                           std::size_t page_size);

private:
    Cache L1;
    Cache L2;
};

#endif
