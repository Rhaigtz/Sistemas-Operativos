#include <iostream>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <functional>
using namespace std;

condition_variable cv;
mutex mu;
int capacidad = 3;
queue<int> buffer;

class monitor
{
public:
    int capacidad;
    queue<int> buffer;
    monitor(int capacidad, queue<int> buffer)
    {
        this->capacidad = capacidad;
        this->buffer = buffer;
    }
    bool noLleno() { return buffer.size() < capacidad; }
    bool contieneAlgo() { return buffer.size() != 0; }
    void agregar()
    {
        while (true)
        {
            unique_lock<mutex> locker(mu);
            cv.wait(locker, std::bind(&monitor::noLleno, this));
            buffer.push(1);
            cout << "Productor " << this_thread::get_id() << " produciendo..." << buffer.size() << endl;
            cv.notify_one();
            this_thread::sleep_for(chrono::seconds(2));
        }
    }
    void quitar()
    {
        while (true)
        {
            unique_lock<mutex> locker(mu);
            cv.wait(locker, std::bind(&monitor::contieneAlgo, this));
            buffer.pop();
            cout << "Consumidor " << this_thread::get_id() << " consumiendo..." << buffer.size() << endl;
            cv.notify_one();
            this_thread::sleep_for(chrono::seconds(2));
        }
    }
};

int main()
{
    monitor mon(capacidad, buffer);
    thread productor1(&monitor::agregar, &mon);
    thread consumidor1(&monitor::quitar, &mon);
    thread consumidor2(&monitor::quitar, &mon);
    productor1.join();
    consumidor1.join();
    consumidor2.join();

    return 0;
}