// tests/test_proxy.cpp
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include "proxy.hpp"

// single-threaded baseline
void test_basic(proxy &p)
{
    std::cout << "=== Basic ===" << std::endl;
    auto r1 = p.Get("/todos/1");
    std::cout << "Miss  — status: " << r1.status << std::endl;
    auto r2 = p.Get("/todos/1");
    std::cout << "Hit   — status: " << r2.status << std::endl;
    p.invalidate_cache();
    std::cout << "Cache cleared" << std::endl;
}

// many threads hitting the same path at the same time
void test_concurrent_same_path(proxy &p)
{
    std::cout << "\n=== Concurrent (same path) ===\n";
    constexpr int N = 20;
    std::atomic<int> hits{0}, misses{0}, errors{0};
    std::vector<std::thread> threads;

    for (int i = 0; i < N; ++i)
        threads.emplace_back([&]()
                             {
            auto r = p.Get("/todos/1");
            if      (r.status == 200) ++hits;
            else if (r.status == 0)   ++errors;
            else                      ++misses; });
    // waiting for all requests to finish before printing results .
    for (auto &t : threads)
        t.join();

    std::cout << N << " threads: "
              << hits << " ok, "
              << errors << " errors\n";
}

// threads hitting different paths at the same time
void test_concurrent_diff_paths(proxy &p)
{
    std::cout << "\n=== Concurrent (different paths) ===\n";
    constexpr int N = 10;
    std::atomic<int> ok{0}, errors{0};
    std::vector<std::thread> threads;

    for (int i = 1; i <= N; ++i)
        threads.emplace_back([&, i]()
                             {
            auto r = p.Get("/todos/" + std::to_string(i));
            if (r.status == 200) ++ok;
            else                 ++errors; });

    for (auto &t : threads)
        t.join();

    std::cout << N << " threads: "
              << ok << " ok, "
              << errors << " errors\n";
}

// interleaved reads and invalidations
void test_concurrent_invalidation(proxy &p)
{
    std::cout << "\n=== Concurrent (reads + invalidations) ===\n";
    constexpr int READERS = 16;
    constexpr int INVALIDATORS = 4;
    std::atomic<int> ok{0}, errors{0};
    std::vector<std::thread> threads;

    for (int i = 0; i < READERS; ++i)
        threads.emplace_back([&]()
                             {
            auto r = p.Get("/todos/2");
            if (r.status == 200) ++ok;
            else                 ++errors; });

    for (int i = 0; i < INVALIDATORS; ++i)
        threads.emplace_back([&]()
                             { p.invalidate_cache(); });

    for (auto &t : threads)
        t.join();

    std::cout << READERS << " readers + " << INVALIDATORS << " invalidators: "
              << ok << " ok, "
              << errors << " errors\n";
}

int main()
{
    proxy p("http://jsonplaceholder.typicode.com");

    test_basic(p);
    test_concurrent_same_path(p);
    test_concurrent_diff_paths(p);
    test_concurrent_invalidation(p);

    return 0;
}