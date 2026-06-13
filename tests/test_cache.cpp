// tests/test_cache.cpp
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include "cache.hpp"
#include <assert.h>

void test_basic()
{
    std::cout << "=== Basic get/put ===\n";
    safe_cache c;

    c.put("key1", "value1");
    auto r1 = c.get("key1");
    assert(r1.has_value() && r1.value() == "value1");

    auto r2 = c.get("missing");
    assert(!r2.has_value());

    std::cout << "PASS\n";
}

void test_invalidate()
{
    std::cout << "\n=== Invalidate ===\n";
    safe_cache c;

    c.put("a", "1");
    c.put("b", "2");
    c.invalidate();

    assert(!c.get("a").has_value());
    assert(!c.get("b").has_value());

    std::cout << "PASS\n";
}

void test_overwrite()
{
    std::cout << "\n=== Overwrite existing key ===\n";
    safe_cache c;

    c.put("key", "old");
    c.put("key", "new");
    auto r = c.get("key");

    assert(r.has_value() && r.value() == "new");
    std::cout << "PASS\n";
}

void test_concurrent_writes()
{
    std::cout << "\n=== Concurrent writes ===\n";
    safe_cache c;
    constexpr int N = 50;
    std::vector<std::thread> threads;

    for (int i = 0; i < N; ++i)
        threads.emplace_back([&, i]()
                             { c.put("key" + std::to_string(i), "val" + std::to_string(i)); });

    for (auto &t : threads)
        t.join();

    // verify all writes are done
    std::atomic<int> found{0};
    for (int i = 0; i < N; ++i)
        if (c.get("key" + std::to_string(i)).has_value())
            ++found;

    assert(found == N);
    std::cout << "PASS (" << found << "/" << N << " keys found)\n";
}

void test_concurrent_reads()
{
    std::cout << "\n=== Concurrent reads ===\n";
    safe_cache c;
    c.put("shared", "data");
    constexpr int N = 50;
    std::atomic<int> hits{0};
    std::vector<std::thread> threads;

    for (int i = 0; i < N; ++i)
        threads.emplace_back([&]()
                             {
            if (c.get("shared").has_value()) ++hits; });

    for (auto &t : threads)
        t.join();
    assert(hits == N);
    std::cout << "PASS (" << hits << "/" << N << " hits)\n";
}

void test_concurrent_reads_and_invalidations()
{
    std::cout << "\n=== Concurrent reads + invalidations ===\n";
    safe_cache c;
    c.put("key", "value");
    constexpr int READERS = 30;
    constexpr int INVALIDATORS = 5;
    std::atomic<int> ok{0};
    std::vector<std::thread> threads;

    for (int i = 0; i < READERS; ++i)
        threads.emplace_back([&]()
                             {
            c.get("key"); 
            ++ok; });

    for (int i = 0; i < INVALIDATORS; ++i)
        threads.emplace_back([&]()
                             { c.invalidate(); });

    for (auto &t : threads)
        t.join();
    assert(ok == READERS);
    std::cout << "PASS (no crash, no deadlock)\n";
}

int main()
{
    test_basic();
    test_invalidate();
    test_overwrite();
    test_concurrent_writes();
    test_concurrent_reads();
    test_concurrent_reads_and_invalidations();

    std::cout << "\nAll cache tests passed.\n";
    return 0;
}