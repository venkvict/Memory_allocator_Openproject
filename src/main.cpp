#include <iostream>
#include <string>
#include <sstream>

#include "normal_allocator.h"
#include "buddy_allocator.h"
#include "cache.h"
#include "vm.h"

using namespace std;
CacheSystem cache_system;

// ==========================
// Mode system
// ==========================
enum class Mode
{
    STANDARD,
    BUDDY,
    CACHE,
    VM
};

Mode current_mode = Mode::STANDARD;

// ==========================
// Print help banner
// ==========================
void print_banner()
{
    cout << "=============================================\n";
    cout << " Memory Management Simulator\n";
    cout << "=============================================\n";
    cout << "Modes:\n";
    cout << "  mode standard\n";
    cout << "  mode buddy\n";
    cout << "  mode cache\n\n";

    cout << "STANDARD mode commands:\n";
    cout << "  init memory <size>\n";
    cout << "  set allocator <first|best|worst>\n";
    cout << "  malloc <size>\n";
    cout << "  free <block_id>\n";
    cout << "  dump\n";
    cout << "  stats\n\n";

    cout << "BUDDY mode commands:\n";
    cout << "  init memory <size>   (size must be power of 2)\n";
    cout << "  malloc <size>\n";
    cout << "  free <block_id>\n";
    cout << "  dump\n\n";

    cout << "CACHE mode commands:\n";
    cout << "  cache init L1 <size> <block> <assoc>\n";
    cout << "  cache init L2 <size> <block> <assoc>\n";
    cout << "  cache access <address>\n";
    cout << "  stats\n\n";

    cout << "  mode vm\n";
    cout << "VM mode commands:\n";
    cout << "  vm init <pages> <page_size> <frames>\n";
    cout << "  access <virtual_address>\n";
    cout << "  stats\n\n";

    cout << "General:\n";
    cout << "  exit\n";
    cout << "=============================================\n\n";
}

// ==========================
// Main
// ==========================
int main()
{
    print_banner();

    string line;
    while (true)
    {
        cout << "> ";
        if (!getline(cin, line))
            break;

        stringstream ss(line);
        string cmd;
        ss >> cmd;

        // ==========================
        // MODE SWITCHING
        // ==========================
        if (cmd == "mode")
        {
            string m;
            ss >> m;

            if (m == "standard")
            {
                current_mode = Mode::STANDARD;
                cout << "Switched to STANDARD allocator mode\n";
            }
            else if (m == "buddy")
            {
                current_mode = Mode::BUDDY;
                cout << "Switched to BUDDY allocator mode\n";
            }
            else if (m == "cache")
            {
                current_mode = Mode::CACHE;
                cout << "Switched to CACHE mode\n";
            }
            else if (m == "vm")
            {
                current_mode = Mode::VM;
                cout << "Switched to VM mode\n";
            }
            else
            {
                cout << "Unknown mode\n";
            }
            continue;
        }

        // ==========================
        // EXIT
        // ==========================
        if (cmd == "exit")
            break;

        // ==========================
        // VM MODE
        // ==========================
        if (current_mode == Mode::VM)
        {
            if (cmd == "vm")
            {
                string sub;
                ss >> sub;

                if (sub == "init")
                {
                    size_t pages, page_size, frames;
                    ss >> pages >> page_size >> frames;
                    vm_init(pages, page_size, frames);
                }
                else
                {
                    cout << "Unknown VM command\n";
                }
            }
            else if (cmd == "access")
            {
                size_t vaddr;
                ss >> vaddr;

                if (!vm_is_initialized())
                {
                    cout << "VM not initialized\n";
                    continue;
                }

                size_t paddr;
                vm_translate(vaddr, paddr);
                cache_system.access(paddr);
            }
            else if (cmd == "stats")
            {
                vm_stats();
            }
            else
            {
                cout << "Unknown command in VM mode\n";
            }
            continue;
        }

        // ==========================
        // CACHE MODE
        // ==========================
        if (current_mode == Mode::CACHE)
        {
            if (cmd == "cache")
            {
                string sub;
                ss >> sub;

                if (sub == "init")
                {
                    string level;
                    size_t size, block, assoc;
                    ss >> level >> size >> block >> assoc;

                    if (level == "L1")
                        cache_system.init_L1(size, block, assoc);
                    else if (level == "L2")
                        cache_system.init_L2(size, block, assoc);
                    else
                        cout << "Unknown cache level\n";
                }
                else if (sub == "access")
                {
                    size_t addr;
                    ss >> addr;
                    cache_system.access(addr);
                }
                else
                {
                    cout << "Unknown cache command\n";
                }
            }
            else if (cmd == "stats")
            {
                cache_system.print_stats();
            }
            else
            {
                cout << "Unknown command in CACHE mode\n";
            }
            continue;
        }

        // ==========================
        // STANDARD MODE
        // ==========================
        if (current_mode == Mode::STANDARD)
        {
            if (cmd == "init")
            {
                string mem;
                size_t size;
                ss >> mem >> size;
                init_memory(size);
                cout << "Initialized memory with " << size << " bytes\n";
            }
            else if (cmd == "set")
            {
                string what, algo;
                ss >> what >> algo;

                if (algo == "first")
                    set_allocator(FIRST_FIT);
                else if (algo == "best")
                    set_allocator(BEST_FIT);
                else if (algo == "worst")
                    set_allocator(WORST_FIT);
                else
                {
                    cout << "Unknown allocator\n";
                    continue;
                }

                cout << "Allocator set to " << algo << "\n";
            }
            else if (cmd == "malloc")
            {
                size_t sz;
                ss >> sz;
                int id = malloc_block(sz);

                if (id == -1)
                    cout << "Allocation failed\n";
                else
                    cout << "Allocated block id=" << id << "\n";
            }
            else if (cmd == "free")
            {
                int id;
                ss >> id;

                if (free_block(id))
                    cout << "Block " << id << " freed\n";
                else
                    cout << "Invalid block id\n";
            }
            else if (cmd == "dump")
            {
                dump_memory();
            }
            else if (cmd == "stats")
            {
                print_stats();
            }
            else
            {
                cout << "Unknown command in STANDARD mode\n";
            }
            continue;
        }

        // ==========================
        // BUDDY MODE
        // ==========================
        if (current_mode == Mode::BUDDY)
        {
            if (cmd == "init")
            {
                string mem;
                size_t size;
                ss >> mem >> size;
                buddy_init(size);
            }
            else if (cmd == "malloc")
            {
                size_t sz;
                ss >> sz;
                int id = buddy_malloc(sz);

                if (id == -1)
                    cout << "Allocation failed\n";
                else
                    cout << "Allocated block id=" << id << "\n";
            }
            else if (cmd == "free")
            {
                int id;
                ss >> id;

                if (buddy_free(id))
                    cout << "Block " << id << " freed\n";
                else
                    cout << "Invalid block id\n";
            }
            else if (cmd == "dump")
            {
                buddy_dump();
            }
            else
            {
                cout << "Unknown command in BUDDY mode\n";
            }
            continue;
        }
    }

    return 0;
}
