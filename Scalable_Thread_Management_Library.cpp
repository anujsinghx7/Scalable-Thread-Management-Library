#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <chrono>
using namespace std;

mutex coutLock;

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

            {
                unique_lock<mutex> lock(qlock);
                cv.wait(lock, [&]() { return stop || !tasks.empty(); });

                if (stop && tasks.empty()) return;

                task = tasks.front();
                tasks.pop();
            }

            task();
        }
    }

    void addTask(function<void()> f) {
        {
            lock_guard<mutex> lock(qlock);
            tasks.push(f);
        }
        cv.notify_one();
    }

    void shutdown() {
        {
            lock_guard<mutex> lock(qlock);
            stop = true;
        }

        cv.notify_all();
        for (auto &t : workers)
            if (t.joinable()) t.join();
    }
};

void exampleTask(int id){
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

    for (int i = 1; i <= 20; i++) {
        pool.addTask([i]() {
            exampleTask(i);
        });
    }

    pool.shutdown();
    cout << "All tasks completed!\n";
    return 0;
}
