/*
Auther: John Blue
Time: 2023/2
Platform: CodeChef IDE
Object: Utilizing thread to simulate Container Port Problem.
        Container ship port problem is a logistic problem that ships would have to be loaded and unloaded in the port units
        , and the number of ships and port units will greatly effect whether the shipping will reach on time.
*/

//
// import libarary
//

// basic libarary
#include <iostream>
#include <time.h>
//#include <string.h>

// random
#include <stdlib.h>
#include <time.h>

// thread libarary
#include <thread>
#include <mutex>
#include <condition_variable>
// sleep
#include <chrono>

using namespace std;

//
// shipping, port, route ... class
//

struct shipping {
    string way;// name of the shipping (from A port to B port)
    unsigned int span;// period that would cost
};
typedef struct shipping shipping;
shipping* create_shipping(string wy, unsigned int tim) {
    shipping* shp = new shipping();
    shp->way = wy;
    shp->span = tim;
    return shp;
}

struct porting {
    string port;// name of the port
    unsigned int unit;// number of (un)loading units in the port
    unsigned int wait_time;// each period would cost when (un)loading
    unsigned int queue_head, queue_tail;

    // pthread locks
    mutex m;
    condition_variable cond;
};
typedef struct porting porting;
porting* create_porting(string pt, unsigned int num, unsigned int tim) {
    porting* ld = new porting();
    ld->port = pt;
    ld->unit = num;
    ld->wait_time = tim;
    ld->queue_head = 0;
    ld->queue_tail = 0;
    return ld;
}

// class for storing ship, loading port, unloading port, ...
struct Route {
    string ship;// name of ship
    shipping* way;// shipping route object
    porting* loading_port;// port object
    porting* unloading_port;// port object
    unsigned int deadline;// dealine
    unsigned int sum;
};
typedef struct Route Route;
Route* create_Route(string shp, shipping* oc, porting* ld, porting* uld, unsigned int ddl) {
    Route* rt = new Route();
    rt->ship = shp; 
    rt->way = oc;
    rt->loading_port = ld;
    rt->unloading_port = uld;
    rt->deadline = ddl;
    rt->sum = 0;
    return rt;
}

//
// thread class
//

void port_thread(porting* prt, unsigned int* ret) {
    //
    // critical section
    //
    unsigned int queue_me = prt->queue_tail++;// drawing ticket
    unique_lock<mutex>* key = new unique_lock<mutex>(prt->m);
    while (prt->unit < 0 || queue_me != prt->queue_head)
    {
        // it will release mutex lock and wait for signal or broadcast
        // FIFO policy
        prt->cond.wait(*key);
    }
    prt->unit--;// deduct number of available units
    
    delete key;
    //
    // critical section end
    //
    
    //
    // parallel working period
    //
    srand(time(NULL));
    unsigned int rt = prt->wait_time + rand() % 3;
    this_thread::sleep_for(chrono::milliseconds(rt));
    
    
    //
    // critical section
    //
    prt->m.lock();
    
    *ret += rt;// time sum up
    
    prt->unit++;// reload number of available units
    prt->queue_head++;// de-queue

    // to signal or broadcast?
    //http://www.qnx.com/developers/docs/qnxcar2/index.jsp?topic=%2Fcom.qnx.doc.neutrino.getting_started%2Ftopic%2Fs1_procs_Condvar_signal_vs_broadcast.html
    prt->cond.notify_all();
    //prt->cond.notify_one();

    prt->m.unlock();
    //
    // critical section end
    //
}

void shipping_thread(unsigned int* span, unsigned int* ret) {
    
    srand(time(NULL));
    unsigned int rt = *span + rand() % 3;
    this_thread::sleep_for(chrono::milliseconds(rt));//this_thread::sleep_for(std::chrono::milliseconds(3));
    *ret += rt;
    
}

unsigned int succ = 0;
unsigned int delay = 0;
void func() {}
void ship_thread(Route** mission) {
    // thread
    thread th;
    
    //
    // loading port period
    //
    //printf("Ship %s at Port %s : loading\n", mission[0]->ship, mission[0]->loading_port->port);
    th = thread(port_thread, mission[0]->loading_port, &(mission[0]->sum));
    th.join();
    cout << "Ship " << mission[0]->ship << " at Port " << mission[0]->loading_port->port << " : departing\n";

    //
    // shipping period
    //
    //printf("Ship %s on Way %s : sailing\n", mission[0]->ship, mission[0]->way->way);
    th = thread(shipping_thread, &(mission[0]->way->span), &(mission[0]->sum));
    th.join();
    cout << "Ship " << mission[0]->ship << " on Way " << mission[0]->way->way << " : reaching\n";
    
    //
    // unloading port period
    //
    //printf("Ship %s at Port %s : unloading\n", mission[0]->ship, mission[0]->unloading_port->port);
    th = thread(port_thread, mission[0]->unloading_port, &(mission[0]->sum));
    th.join();
    cout << "Ship " << mission[0]->ship << " at Port " << mission[0]->unloading_port->port << " : shipping finished\n";

    //
    // count sucess or delay
    //
    bool check = mission[0]->sum > mission[0]->deadline;// 1 delay 0 sucess
    if (check) {
        //printf("Ship %s delay\n", mission[0]->ship);
        delay++;
    }
    else {
        //printf("Ship %s success\n", mission[0]->ship);
        succ++;
    }
}

//
// main function
//

void sub_main(unsigned int ship_span, unsigned int port_span, unsigned int deadline, unsigned int port_A_unit, unsigned int port_B_unit) {
    // define ships number
    unsigned int ship_N = 12;
    char ship_name[12][3];

    // define route, ports
    shipping* A_to_B = create_shipping("A_to_B", ship_span);// shipping time
    porting* A = create_porting("A", port_A_unit, port_span);// port A
    porting* B = create_porting("B", port_B_unit, port_span);// port B
    // mission_list
    // [0]{[0]Route*, [1]Route*, ...}, [1]{[0]...}, ...
    Route*** mission_list = new Route**[ship_N];
    for (int i = 0; i < ship_N; i++) {
        // give ship name (00, 01, to ...)
        if (i < 10) {
            ship_name[i][0] = '0';
        }
        else if (i < 20) {
            ship_name[i][0] = '1';
        }
        else if (i < 30) {
            ship_name[i][0] = '2';
        }
        else {
            ship_name[i][0] = '3';
        }
        ship_name[i][1] = i % 10 + '0';
        ship_name[i][2] = '\0';
        
        // mission_list
        mission_list[i] = new Route*;
        mission_list[i][0] = create_Route(ship_name[i], A_to_B, A, B, deadline);
    }
    
    // threads start
    thread t[ship_N];
    for (int i = 0; i < ship_N; i++) {
        t[i] = thread(ship_thread, mission_list[i]);
    }
    
    // wait till every thread
    for (int i = 0; i < ship_N; i++) {
        t[i].join();
    }

    // print results
    cout << "...\n";
    cout << "Ship unit: 12; port A unit: " << port_A_unit << "; port B unit: " << port_B_unit << "\n";
    cout << "Sucess: " << succ << "; Delayed: " << delay << "\n\n\n";
    succ = 0;    delay = 0;

    
    // free allocated memory
    delete A_to_B;
    delete A;
    delete B;
    for(int i = 0; i < 3; i++) {
        delete[] mission_list[i];
    }
    delete[] mission_list;
}

int main()
{
    // define variables
    unsigned int ship_span = 9;
    unsigned int port_span = 3;
    unsigned int deadline = ship_span + port_span + port_span + 1;
    unsigned int port_A_unit = 0;
    unsigned int port_B_unit = 0;
    
    // assign port units
    port_A_unit = 4;
    port_B_unit = 4;
    // sub main
    sub_main(ship_span, port_span, deadline, port_A_unit, port_B_unit);

    // assign port units
    port_A_unit = 2;
    port_B_unit = 2;
    // sub main
    sub_main(ship_span, port_span, deadline, port_A_unit, port_B_unit);

    return 0;
}

