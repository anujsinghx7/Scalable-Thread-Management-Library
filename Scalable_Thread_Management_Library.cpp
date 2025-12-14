#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

using namespace std;

// Semaphore Class Implementation
class Semaphore {
private:
    mutex mtx;
    condition_variable cv;
    int count;

public:
    Semaphore(int c = 0) : count(c) {}

    void acquire() {
        unique_lock<mutex> lock(mtx);
        cv.wait(lock, [&]() { return count > 0; });
        count--;
    }

    void release() {
        unique_lock<mutex> lock(mtx);
        count++;
        cv.notify_one();
    }
};

class ThreadPool {
private:
    vector<thread> workers;
    queue<function<void()>> tasks;

    mutex qlock;
    condition_variable cv;
    bool stop = false;

public:

    ThreadPool(int nThreads) {
        for (int i = 0; i < nThreads; i++) {
            workers.emplace_back([this]() { workerThread(); });
        }
    }

void workerThread() {
        while (true) {
            function<void()> task;

            {   // Acquire lock
                unique_lock<mutex> lock(qlock);

                cv.wait(lock, [&]() { return stop || !tasks.empty(); });

                if (stop && tasks.empty()) return;

                task = tasks.front();
                tasks.pop();
            }

            task(); 
        }
    }


void exampleTask(int id) {
    {
        lock_guard<mutex> lock(coutLock);
        cout << "[START] Task " << id << " running\n";
    }

    this_thread::sleep_for(chrono::milliseconds(300)); 

    {
        lock_guard<mutex> lock(coutLock);
        cout << "[END]   Task " << id << " finished\n";
    }
}

int main() {

    ThreadPool pool(4);        

    for (int i = 1; i <= 50; i++) {
        pool.addTask([i]() {
            exampleTask(i);
        });
    }

    pool.shutdown();

    cout << "All tasks completed!\n";
    return 0;
}


