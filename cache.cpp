#include <unordered_map>
#include <string>
#include <shared_mutex>
#include <optional>

class safe_cache
{
    std::unordered_map<std::string, std::string> cache;
    // multi readers , one writer thread
    std::shared_mutex mu_;

public:
std::optional<std::string> get(std::string key){
    std::shared_lock lock(mu_);
    auto it = cache.find(key);
    if (it == cache.end()) return std::nullopt ;
    return it->second ;
    
}
    void put(std::string key, std::string val)
    {
        // acquiring lock
        std::unique_lock lock(mu_);
        
        cache.insert(key, val);
        // releasing lock lock
        lock.unlock();
    }
    void invalidate(){
        
        std::unique_lock lock(mu_);
        cache.clear();
        lock.unlock();
    }
};
