#ifndef GRAB_HPP
#define GRAB_HPP

#include <bits/stdc++.h>
#include <sys/mman.h>

using namespace std;

/*
@class grab
    DESIGN TRADE-OFFS:
    This allocator utilizes an array of segregated free lists (size classes) rather 
    than a single contiguous free list with block headers. 
    
    1. Time Complexity (The O(1) Advantage): In a single-list design, allocation 
        requires O(N) traversal to find a block of suitable size (First-Fit/Best-Fit). 
        By segregating blocks into power-of-2 buckets, allocation and deallocation 
        become strictly O(1) operations.
    
    2. Space Trade-off (Internal vs External Fragmentation): 
        - Single List: Suffers from external fragmentation as free blocks scatter.
        - Segregated Lists: Trades external fragmentation for minor "internal" 
        fragmentation (e.g., allocating 12 bytes returns a 16-byte block, wasting 
        4 bytes), but guarantees predictable, ultra-low latency.    
     
*/

const size_t MAX_SMALL_ALLOC = 1024;

const size_t NUM_CLASSES = MAX_SMALL_ALLOC / 8;

class grab {
    private:
        char* mem_pool = nullptr; // base addrs
        size_t offset{}; //bump offset
        size_t total_size{}; // how much is reserved
        // a free block as Node
        struct Node{
            Node* next; //free list next pointer
        };
        
        Node* list[NUM_CLASSES] = {}; //init of the free list

    public:
        //constructor
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
            
            if(raw_mem == MAP_FAILED){
                throw bad_alloc();
            }

            mem_pool = (char*) raw_mem;
        }
        
        //allocation of the chunked memory from mmap 

        void* allocate(size_t size){
            /*
            Using multiples of 8 as free list indices instead of powers of 2 to redue the internal fragmentation. 
            Also the list index is mapped to the payload size not the entire chunk taken from the memory pool (header+payload)
            since every payload is attached with a header of of size "sizeof(size_t)" (8 bytes).
            */
            
            if (size > MAX_SMALL_ALLOC) {
                return nullptr; 
            }

            size = std::max((size_t)8, size); // Cap to 8 minimum

            unsigned idx = (size-1)/8; //ensures 0-based mapping
            //calculate payload
            size_t payload = (idx+1)*8;

            size_t total_chunk = payload+sizeof(size_t);

            if(list[idx]){
                //making it the head and pushing the used node  
                Node* temp = list[idx];
                list[idx] = list[idx]->next;
                return (void*)temp;
            }

            if(offset+total_chunk>total_size){
                return nullptr;
            }
        
            char* bwlock = mem_pool+offset; //pointing to starting address    
            *(size_t*) block = payload; //writing of payload size
            void* user_ptr = block+sizeof(size_t); //pointing to starting addrs of free mem
            offset += total_chunk;  
            return (void*)user_ptr;
            
        }

        void deallocate(void* ptr){

            if (!ptr) {
                return; 
            }
            
            //getting the size of the block from the payload
            size_t* size_start = (size_t*)ptr - 1;
            Node* temp = (Node*)ptr;
            unsigned idx = (*size_start-1)/8;
            //making it the head and pushing the used node 
            temp->next = list[idx];
            list[idx] = temp; 
        }
};

#endif