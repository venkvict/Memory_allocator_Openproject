#ifndef CACHE_H
#define CACHE_H

#include <cstddef>
#include <string>
#include <vector>

class Cache {
public:
    Cache();   // ✅ constructor declared

    void init(std::size_t cache_size,
              std::size_t block_size,
              std::size_t associativity,
              const std::string& name);

    bool access(std::size_t address);
    void insert(std::size_t address);

    void print_stats() const;
    bool is_initialized() const;

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

class CacheSystem {
public:
    CacheSystem();  // ✅ constructor declared

    void init_L1(std::size_t size,
                 std::size_t block,
                 std::size_t assoc);

    void init_L2(std::size_t size,
                 std::size_t block,
                 std::size_t assoc);

    void access(std::size_t address);
    void print_stats() const;

private:
    Cache L1;
    Cache L2;
};

#endif
