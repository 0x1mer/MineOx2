#include "ThreadPool.h"
#include "TaskFactory.h"
#include "TaskCategoryStrategy.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() 
{
    try
    {
        ThreadPool& pool = ThreadPool::Instance();
        pool.SetStrategy(std::make_unique<TaskCategoryStrategy>());
        pool.Init(); // инициализация воркеров

        std::cout << "=== Adding simple tasks ===\n";

        // --- Простые задачи ---
        for (int i = 0; i < 8; ++i)
        {
            auto t = TaskFactory::MakeTask(
                [i]() {
                    std::cout << "[Task] Simple task " << i
                              << " running on thread " << std::this_thread::get_id() << "\n";
                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                },
                [](const std::exception &ex) {
                    std::cerr << "[ErrorHandler] Caught exception: " << ex.what() << "\n";
                }
            );

            auto res = pool.AddTask(TaskType::Light, std::move(t));
            if (!res.Ok())
                std::cerr << "[Warning] Task queue full for Light task " << i << "\n";
        }

        std::cout << "\n=== Adding future tasks ===\n";

        // --- Future задачи ---
        for (int i = 0; i < 5; ++i)
        {
            auto ft = TaskFactory::MakeFutureTask(
                [i]() {
                    std::cout << "[FutureTask] Job " << i << " executing on thread "
                              << std::this_thread::get_id() << "\n";
                    if (i == 2)
                        throw std::runtime_error("Oops");
                    std::this_thread::sleep_for(std::chrono::milliseconds(80));
                },
                [](TaskResult r) {
                    std::cout << "[FutureTask] Result callback, success=" << r.success << "\n";
                },
                [i](const std::exception &ex) {
                    std::cerr << "[FutureTask] Exception caught in job " << i << ": " << ex.what() << "\n";
                }
            );

            auto res = pool.AddTask(TaskType::Heavy, std::move(ft));
            if (!res.Ok())
                std::cerr << "[Warning] Task queue full for FutureTask " << i << "\n";
        }

        // Дадим задачам время выполниться
        std::this_thread::sleep_for(std::chrono::seconds(3));

        std::cout << "\n=== All tasks finished ===\n";
    }
    catch (const std::exception &ex)
    {
        std::cerr << "Exception in main: " << ex.what() << "\n";
    }

    return 0;
}