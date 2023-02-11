/*
Auther: John Blue
Time: 2023/2
Object: demonstrate usage of mutex in c++ thread (for preventing race condition on variavble i)
*/

#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

// mutex lock
mutex key;

void count(unsigned int id) {
    static int i = 0;
    //
    // critical section
    //
    key.lock();
    
    cout << "thread id: " << id << ", i = " << i++ << endl;
    
    key.unlock();
    //
    // critical section end
    //
}

int main() {
    // create threads and race
    thread t[9];
    for (unsigned int j = 0; j < 9; j++) {
        t[j] = thread(count, j);
    }
    
    // (join) wait until all threads are finished
    for (unsigned int j = 0; j < 9; j++) {
        t[j].join();
    }
    return 0;
}
