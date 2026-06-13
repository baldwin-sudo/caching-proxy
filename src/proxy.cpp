#include "proxy.hpp"

proxy::proxy(const std::string &origin_url)
    : client(origin_url)
{
}

Response proxy::Get(const std::string &path)
{
    auto cached = cache.get(path);
    if (cached)
        return Response{200, *cached};

    auto res = client.Get(path);
    if (!res)
        return Response{502, "Bad Gateway"};

    cache.put(path, res->body);
    return Response{res->status, res->body};
}

void proxy::invalidate_cache()
{

    cache.invalidate();
}