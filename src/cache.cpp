#include "cache.hpp"
#include <iostream>

std::optional<std::string> safe_cache::get(const std::string &key)
{
    std::shared_lock lock(mu_);
    auto it = cache_.find(key);
    if (it == cache_.end())
    {
        std::cout << "cache_log :cache miss !" << std::endl;
        return std::nullopt;
    }
    std::cout << "cache_log :cache hit !" << std::endl;

    lock.unlock();
    return it->second;
}

void safe_cache::put(const std::string &key, const std::string &val)
{
    // acquire lock
    std::unique_lock lock(mu_);
    cache_[key] = val;
    // release lock
    lock.unlock();
}

void safe_cache::invalidate()
{
    // acquire lock
    std::unique_lock lock(mu_);
    std::cout << "cache_log :cache invalidated !" << std::endl;
    cache_.clear();
    // release lock
    lock.unlock();
}