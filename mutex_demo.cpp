/*

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
    
    std::cout << "thread id: " << id << ", i = " << i++ << std::endl;
    
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
