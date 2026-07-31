#include<bits/stdc++.h>
#include <sys/mman.h>
using namespace std;

class grab {
    private:
        char* mem_pool = nullptr;
        size_t offset{};
        size_t total_size{};
        size_t mem_size{};
        struct Node{
            Node* next;
        };
    
    public:
        grab(size_t size) : total_size(size),offset(0) 
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
            Node* head = nullptr;
        }
         
        //allocation of the chunked memory from mmap 

        void* allocate(size_t size){
            if(offset+size>total_size){
                return nullptr;
            }

            if(!head){
                void* bump = mem_pool+offset; //pointing to starting address
                offset += size;
                return bump;
            }

            Node* temp = head;
            head = head->next;
            return (void*)temp;
        }

        void deallocate(void* ptr){
            Node* temp = (Node*)ptr;
            temp->next = head;
            head = temp;
        }
}