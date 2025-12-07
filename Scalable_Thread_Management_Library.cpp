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
