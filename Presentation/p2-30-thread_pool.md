# ECE4821J Topic 30: Thread Pool

## Thread Pool

A **thread pool** is a collection of pre-allocated threads that are managed and reused to execute tasks efficiently.  
Instead of creating and destroying threads for each individual task, a thread pool maintains a set number of threads to handle tasks as they arrive.

## How to implement in C++?

We are going use the `<thread>`, `<future>`, and `<queue>` libraries.

```Cpp
#include <iostream>
#include <vector>
#include <queue>
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <functional>

// ThreadPool class that manages a pool of threads to handle tasks concurrently
class ThreadPool {
public:
    // Constructor: starts the thread pool with a specified number of threads
    ThreadPool(size_t numThreads) {
        start(numThreads);
    }

    // Destructor
    ~ThreadPool() {
        stop();
    }

    // Method to add a new task to the pool
    template<typename T>
    auto enqueue(T task) -> std::future<decltype(task())> {
        // Wrap the task in a packaged_task, allowing us to obtain a future for the result
        auto wrapper = std::make_shared<std::packaged_task<decltype(task())()>>(std::move(task));
        
        // Lock the mutex to safely add the task to the queue
        {
            std::unique_lock<std::mutex> lock{mEventMutex};
            // Add a lambda that runs the wrapped task to the task queue
            mTasks.emplace([=] {
                (*wrapper)();
            });
        }

        // Notify one thread that a new task is available
        mEventVar.notify_one();
        
        // Return the future to the caller, allowing them to wait for the result
        return wrapper->get_future();
    }

private:
    std::vector<std::thread> mThreads; // Vector to store worker threads
    std::condition_variable mEventVar; // Condition variable to notify threads of new tasks
    std::mutex mEventMutex; // Mutex to protect shared resources (like task queue)
    bool mStopping = false; // Flag to indicate if the pool is stopping

    std::queue<std::function<void()>> mTasks; // Queue to hold tasks awaiting execution

    // Function to start the thread pool by creating and running threads
    void start(size_t numThreads) {
        for (auto i = 0u; i < numThreads; ++i) {
            mThreads.emplace_back([=] {
                // Each thread continuously checks for new tasks until the pool is stopping
                while (true) {
                    std::function<void()> task;

                    {
                        // Acquire lock to safely access the task queue
                        std::unique_lock<std::mutex> lock{mEventMutex};

                        // Wait until there is a task or stopping is true
                        mEventVar.wait(lock, [=] { return mStopping || !mTasks.empty(); });

                        // If stopping is true and no more tasks, break out of the loop to exit thread
                        if (mStopping && mTasks.empty())
                            break;

                        // Fetch the next task from the queue
                        task = std::move(mTasks.front());
                        mTasks.pop();
                    }

                    // Execute the task outside of the lock to allow other threads to continue
                    task();
                }
            });
        }
    }

    // Function to stop the thread pool and clean up resources
    void stop() noexcept {
        {
            // Lock to set the stopping flag and notify all threads
            std::unique_lock<std::mutex> lock{mEventMutex};
            mStopping = true;
        }

        // Wake up all threads so they can exit if stopping is true
        mEventVar.notify_all();

        // Join all threads to ensure they complete execution before destruction
        for (auto &thread : mThreads)
            thread.join();
    }
};

```

## Usage

```Cpp
int main() {
    // Create a thread pool with 4 worker threads
    ThreadPool pool{4};

    // Enqueue a task into the pool and get a future for its result
    auto future1 = pool.enqueue([] {
        std::cout << "Task 1 executed" << std::endl;
        return 1;  // The task returns the integer 1
    });

    // Enqueue another task and get a future for its result
    auto future2 = pool.enqueue([] {
        std::cout << "Task 2 executed" << std::endl;
        return 2;  // The task returns the integer 2
    });

    // Retrieve and print the results of the tasks using futures
    std::cout << "Task 1 result: " << future1.get() << std::endl;
    std::cout << "Task 2 result: " << future2.get() << std::endl;

    return 0;
}

```

## Advantages of Using a Thread Pool

- **Reduced Overhead**: Creating and destroying threads is time-consuming and resource-intensive. A thread pool minimizes this cost.

- **Better Resource Utilization**: A thread pool ensures that resources are reused and limits the number of concurrent threads, avoiding issues like CPU overload.

- **Task Management**: It enables easier management of tasks, especially when combined with tasks queues.

## Author
- **Jinlock Choi** 520370990012 jinlock99@sjtu.edu.cn