#include<bits/stdc++.h>
#include <sys/mman.h>
using namespace std;

/**
 * @class GrabAllocator
 * @brief A high-performance, custom memory allocator using Segregated Free Lists.
 *
 * DESIGN TRADE-OFFS:
 * This allocator utilizes an array of segregated free lists (size classes) rather 
 * than a single contiguous free list with block headers. 
 * 
 * 1. Time Complexity (The O(1) Advantage): In a single-list design, allocation 
 *    requires O(N) traversal to find a block of suitable size (First-Fit/Best-Fit). 
 *    By segregating blocks into power-of-2 buckets, allocation and deallocation 
 *    become strictly O(1) operations.
 * 
 * 2. Space Trade-off (Internal vs External Fragmentation): 
 *    - Single List: Suffers from external fragmentation as free blocks scatter.
 *    - Segregated Lists: Trades external fragmentation for minor "internal" 
 *      fragmentation (e.g., allocating 12 bytes returns a 16-byte block, wasting 
 *      4 bytes), but guarantees predictable, ultra-low latency.
 */

class grab {
    private:
        char* mem_pool = nullptr;
        size_t offset{};
        size_t total_size{};
        struct Node{
            Node* next;
        };
        
        Node* list[32] = nullptr; //init of the free list

    public:
        grab(size_t size) : total_size(size), offset(0) 
        { 
            //using mmap to take some chunk of memory 
            void* raw_mem = mmap( 
                nullptr,                    
                total_size,         
                PROT_READ | PROT_WRITE,     
                MAP_PRIVATE | MAP_ANONYMOUS,
                -1,                         
                0                          
            );
            mem_pool = (char*) raw_mem;
        }
         
        //allocation of the chunked memory from mmap 

        void* allocate(size_t size){
            if(offset+size>total_size){
                return nullptr;
            }
            
            unsigned idx = bit_width(size-1) - 3;

            if(!list[idx]){
                char* bump = mem_pool+offset; //pointing to starting address
                *(size_t*) bump = size;
                offset += size+sizeof(size_t); 
                bump += sizeof(size_t);
                return (void*)bump;
            }
            
            Node* temp = list[idx];
            list[idx] = list[idx]->next;
            return (void*)temp;
        }

        void deallocate(void* ptr){
            size_t* size_start = (size_t*)ptr - 1;
            Node* temp = (Node*)ptr;
            unsigned idx = bit_width(*(size_start)-1) - 3;
            temp->next = list[idx];
            list[idx] = temp; 
        }
}