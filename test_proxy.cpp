#include "proxy.cpp"
#include "include/httplib.h"

int test_proxy() {
    // point proxy at a real public API for testing
    proxy p("https://jsonplaceholder.typicode.com");

    // --- test GET, cache miss ---
    std::cout << "=== First request (cache miss) ===\n";
    auto res1 = p.Get("/todos/1");
    std::cout << "Status: " << res1.status << "\n";
    std::cout << "Body: " << res1.body << "\n";

    // --- test GET, cache hit ---
    std::cout << "\n=== Second request (cache hit) ===\n";
    auto res2 = p.Get("/todos/1");  // same path, should hit cache
    std::cout << "Status: " << res2.status << "\n";
    std::cout << "Body: " << res2.body << "\n";

    // --- test invalidate ---
    std::cout << "\n=== After invalidate ===\n";
    p.invalidate_cache();
    auto res3 = p.Get("/todos/1");  // cache miss again
    std::cout << "Status: " << res3.status << "\n";

    return 0;
}