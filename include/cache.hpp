#pragma once

#include <unordered_map>
#include <string>
#include <shared_mutex>
#include <optional>

class safe_cache
{
private:
    std::unordered_map<std::string, std::string> cache_;
    std::shared_mutex mu_;

public:
    std::optional<std::string> get(const std::string &key);

    void put(const std::string &key, const std::string &val);

    void invalidate();
};