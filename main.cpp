#include<bits/stdc++.h>
#include <thread>
#include<chrono>
#include "grab.hpp"
using namespace std;

#define GB (1024ULL*1024*1024)

grab g(GB);

void work(){
    for(int i=0;i<100000;i++){
        g.allocate(16);
    }
}

int main(){
    int n;
    cout<<"Enter Number of Threads: ";
    cin>>n;

    vector<thread> threads;

    auto start = chrono::high_resolution_clock::now();
    //creation
    for(int i=0;i<n;i++){
        threads.push_back(thread(work));
    }
    //waiting
    for(int i=0;i<n;i++){
        threads[i].join();
    }

    auto end = chrono::high_resolution_clock::now();

    auto time = chrono::duration_cast<chrono::milliseconds>(end-start);

    cout<<"Time Taken = " << time.count() << "ms\n";
    return 0;
}