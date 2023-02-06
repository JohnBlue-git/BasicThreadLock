/*

*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

// mutex lock
mutex m;
// conditional condition variable
condition_variable cond;

void count_in_order(unsigned int id) {
    static unsigned int i = 0;
    //
    // critical section
    //
    // conditional wait
    unique_lock<mutex> key(m);// !!! very unique mutex
    while (id != i) cond.wait(key);
    
    cout << "thread id: " << id << ", i = " << i++ << endl;
    
    // conditional signal
    cond.notify_all();
    //cond.notify_one();
    //
    // critical section end
    //
}

int main() {
    // create threads and race
    thread t[9];
    for (unsigned int j = 0; j < 9; j++) {
        t[j] = thread(count_in_order, j);
    }
    
    // (join) wait until all threads are finished
    for (unsigned int j = 0; j < 9; j++) {
        t[j].join();
    }
    return 0;
}
