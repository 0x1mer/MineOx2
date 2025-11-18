#pragma once

#include <thread>
#include <condition_variable>
#include <iostream>
#include <atomic>
#include <windows.h>
#include <emmintrin.h>
#include <array>
#include <cstddef>

#include "concurrentqueue.h"
#include "WorkerStatus.h"
#include "ITask.h"
#include "IWorker.h"
#include "WorkerConfig.h"

class ThreadPool;

// Branch prediction helpers
#if defined(__GNUC__) || defined(__clang__)
#define LIKELY(x) (__builtin_expect(!!(x), 1))
#define UNLIKELY(x) (__builtin_expect(!!(x), 0))
#else
#define LIKELY(x) (x)
#define UNLIKELY(x) (x)
#endif

// ==========================================================
//                       WORKER
// ==========================================================
struct alignas(CACHE_LINE_SIZE) Worker : public IWorker
{
private:
    size_t sizeOfQueue_;
    size_t coreIndex = NO_AFFINITY;

#if WORKER_ENABLE_STATS
    alignas(CACHE_LINE_SIZE) std::atomic<uint64_t> executedTasks{0};
#else
    alignas(CACHE_LINE_SIZE) std::atomic<uint64_t> executedTasks{0};
#endif
    alignas(CACHE_LINE_SIZE) std::array<char, CACHE_LINE_SIZE> pad_executed_{};

    alignas(CACHE_LINE_SIZE) std::atomic<size_t> queueCount{0};
    alignas(CACHE_LINE_SIZE) std::array<char, CACHE_LINE_SIZE> pad_queuecount_{};

    alignas(CACHE_LINE_SIZE) std::atomic<WorkerStatus> status_{WorkerStatus::Running};
    alignas(CACHE_LINE_SIZE) std::array<char, CACHE_LINE_SIZE> pad_status_{};

    std::condition_variable cv;
    std::mutex cvMtx;

    // queue of ITask, but Task manages its own memory (SBO/heaps), so ok
    alignas(CACHE_LINE_SIZE) moodycamel::ConcurrentQueue<std::unique_ptr<ITask>> tasks;
    alignas(CACHE_LINE_SIZE) std::array<char, CACHE_LINE_SIZE> pad_tasks_{};

    static constexpr int SPIN_TRIES = WORKER_SPIN_TRIES;
    static constexpr int YIELD_TRIES = WORKER_YIELD_TRIES;
    static constexpr std::size_t BATCH_SIZE = WORKER_BATCH_SIZE;

private:
    friend class ThreadPool;
    explicit Worker(size_t sizeOfQueue = 4096) noexcept
        : sizeOfQueue_(sizeOfQueue)
    {
    }

public:
    std::thread thread;

    // ---------------- helpers ----------------
    static inline void PinToCore(size_t idx) noexcept
    {
        if (idx == NO_AFFINITY)
            return;
        HANDLE h = GetCurrentThread();
        DWORD_PTR mask = (1ull << idx);
        SetThreadAffinityMask(h, mask);
    }

    inline bool IsStopped() const noexcept
    {
        return status_.load(std::memory_order_relaxed) == WorkerStatus::Stopped;
    }

    inline bool IsPaused() const noexcept
    {
        return status_.load(std::memory_order_relaxed) == WorkerStatus::Paused;
    }

    inline bool HasTasks() const noexcept
    {
        return queueCount.load(std::memory_order_relaxed) > 0;
    }

    inline void NotifyOneLocked() noexcept
    {
        cv.notify_one();
    }

    // ---------------- IWorker API ----------------
    inline void SetAffinityIndex(size_t idx) noexcept override
    {
        coreIndex = idx;
    }

    void Start() override
    {
        thread = std::thread(&Worker::Run, this);
    }

    ~Worker()
    {
        Stop();
        if (thread.joinable())
            thread.join();
    }

    AddTaskResult AddTask(std::unique_ptr<ITask> t) override
    {
        const size_t prev = queueCount.fetch_add(1, std::memory_order_relaxed);
        if (UNLIKELY(prev >= sizeOfQueue_))
        {
            queueCount.fetch_sub(1, std::memory_order_relaxed);
            return AddTaskResult::QueueFull(std::move(t));
        }

        tasks.enqueue(std::move(t));

        if (prev == 0)
        {
            std::lock_guard<std::mutex> lk(cvMtx);
            cv.notify_one();
        }
        return AddTaskResult::Ok();
    }

    // ======================================================
    //                   MAIN LOOP (UPDATED)
    // ======================================================
    void Run()
    {
        if (coreIndex != NO_AFFINITY)
        {
            PinToCore(coreIndex);
        }

        std::unique_lock<std::mutex> lock(cvMtx);

        while (true)
        {
            // Worker active → run tasks
            if (LIKELY(!IsStopped()) && LIKELY(!IsPaused()))
            {
                lock.unlock();

                // ----------------- 1. FAST BULK DEQUEUE -----------------
                std::unique_ptr<ITask> batch[BATCH_SIZE];
                size_t got = tasks.try_dequeue_bulk(batch, BATCH_SIZE);

                if (got > 0)
                {
                    queueCount.fetch_sub(got, std::memory_order_relaxed);

                    for (size_t i = 0; i < got; ++i)
                    {
                        (*batch[i])(); // No try/catch → Task handles errors internally
                    }

#if WORKER_ENABLE_STATS
                    executedTasks.fetch_add(got, std::memory_order_relaxed);
#endif
                    lock.lock();
                    continue;
                }

                // ----------------- 2. SPIN TRY SINGLE -----------------
                std::unique_ptr<ITask> task;
                bool gotSingle = false;

                for (int spin = 0; spin < SPIN_TRIES; ++spin)
                {
                    if (tasks.try_dequeue(task))
                    {
                        gotSingle = true;
                        break;
                    }
                    _mm_pause();
                }

                // ----------------- 3. YIELD TRY SINGLE -----------------
                if (!gotSingle)
                {
                    for (int y = 0; y < YIELD_TRIES; ++y)
                    {
                        if (tasks.try_dequeue(task))
                        {
                            gotSingle = true;
                            break;
                        }
                        std::this_thread::yield();
                    }
                }

                // ----------------- 4. SLEEP WAIT -----------------
                if (!gotSingle)
                {
                    lock.lock();
                    if (!HasTasks() && !IsStopped())
                    {
                        cv.wait(lock, [&]
                                { return IsStopped() || HasTasks() || !IsPaused(); });
                    }
                    continue;
                }

                // ----------------- 5. EXECUTE ONE -----------------
                queueCount.fetch_sub(1, std::memory_order_relaxed);
                (*task)(); // no try/catch

#if WORKER_ENABLE_STATS
                executedTasks.fetch_add(1, std::memory_order_relaxed);
#endif

                lock.lock();
                continue;
            }

            // ---------------- PAUSED / STOPPED ----------------
            if (IsStopped())
                break;

            if (IsPaused())
            {
                cv.wait(lock, [&]
                        { return !IsPaused() || IsStopped(); });
                continue;
            }
        }
    }

    inline void Pause() noexcept override
    {
        status_.store(WorkerStatus::Paused, std::memory_order_relaxed);
    }

    inline void Resume() noexcept override
    {
        status_.store(WorkerStatus::Running, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lk(cvMtx);
        cv.notify_one();
    }

    inline void Stop() noexcept override
    {
        status_.store(WorkerStatus::Stopped, std::memory_order_relaxed);
        std::lock_guard<std::mutex> lk(cvMtx);
        cv.notify_one();
    }

    inline WorkerStatus GetStatus() const noexcept override
    {
        return status_.load(std::memory_order_relaxed);
    }

    inline size_t GetQueueSize() noexcept override
    {
        return queueCount.load(std::memory_order_relaxed);
    }

#if WORKER_ENABLE_STATS
    inline uint64_t GetExecutedTasks() const noexcept { return executedTasks.load(std::memory_order_relaxed); }
#else
    inline uint64_t GetExecutedTasks() const noexcept { return 0; }
#endif
};