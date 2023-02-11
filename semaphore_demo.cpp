/*
Auther: John Blue
Time: 2023/2
Object: demonstrate usage of semaphore in c++ thread (but C++ thread don;t have semphore, we have to bulid it with mutex & conditional variable)
*/

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

// semaphore (by mutex lock & condition variable)
class my_semaphore {
private:
    // mutex lock
    mutex m;
    // condition variable
    condition_variable cond;
  
    int event;

public:
    explicit my_semaphore(int ev = 0): event(ev) {}

    void Signal() {
        // event reload
        event++;
        
        // unlock
        cond.notify_all();
        //cond.notify_one();
    }

    void Wait() {
        // lock
        unique_lock<mutex> key(m);
        while (event < 0) cond.wait(key);
        
        // event count down
        event--;
    }
};

// global const
unsigned int M = 9; 
// global sum
int sum = 0;

// semaphore lock
my_semaphore smp(- M);

void caculate(unsigned int id, int num) {
    // normal
    cout << "thread id: " << id << ", num = " << num << endl;
    // sum up
    sum += num;
    
    // semaphore signal
    smp.Signal();
}

int main() {
    // create threads and race
    thread t[M];
    for (unsigned int j = 0; j < M; j++) {
        t[j] = thread(caculate, j, j);
        t[j].detach();
    }
    
    // semaphore wait ... until all num are added up
    smp.Wait();
    cout << "sum = " << sum << endl;// 0 + ... + 8 = 36

    return 0;
}
