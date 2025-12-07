# Scalable Thread Management Library

A simple, scalable thread management library in C++ featuring a basic thread pool and semaphore class. It’s designed to help manage concurrent tasks efficiently in C++ applications.

## Features

- **ThreadPool**: Easily offload tasks to a pool of worker threads.
- **Semaphore**: Synchronization primitive to coordinate parallel execution (expand in code as needed).
- Queue-based task scheduling with thread-safe addition of tasks.
- Clean shutdown of worker threads.

## Technologies Used

- C++11 (or newer): utilises `<thread>`, `<mutex>`, `<condition_variable>`
- No external dependencies

## Installation & Build

1. **Clone the repository:**
   ```bash
   git clone https://github.com/anujsinghx7/Scalable-Thread-Management-Library.git
   cd Scalable-Thread-Management-Library
   ```

2. **Compile the code (using g++):**
   ```bash
   g++ -std=c++11 -pthread main.cpp -o threadpool
   ```

   *(Make sure you have a C++11+ compatible compiler and pthreads available, as on Ubuntu.)*

3. **Run the program:**
   ```bash
   ./threadpool
   ```

## Usage Example

Here’s what your `main.cpp` does:
```cpp
#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
using namespace std;


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

            task(); // Execute task
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


void exampleTask(int id) {
    cout << "Thread running task: " << id << "\n";
    this_thread::sleep_for(chrono::milliseconds(100));
}

int main() {

    ThreadPool pool(4);        // 4 worker threads

    for (int i = 1; i <= 50; i++) {
        pool.addTask([i]() {
            exampleTask(i);
        });
    }

    pool.shutdown();

    cout << "All tasks completed!\n";
    return 0;
}

}
```

## Output

You should see output similar to:
```
Thread running task: 1
Thread running task: 2
...
All tasks completed!
```

## License

MIT License. See [LICENSE](LICENSE) for details.

## Contribution

Pull requests and suggestions are welcome!

## Support

Open an issue in this repo for questions.

---

*You can copy your code into `main.cpp`, and follow these instructions to try out the library on your Ubuntu system!*
