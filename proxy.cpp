#include "cache.cpp"
#include "include/httplib.h"
struct Response
{
    int status;
    std::string body;
};
enum RequestType
{
    GET,
    POST,

};
struct Request
{
    RequestType type;
    std::string path;
    // add payload
};
class proxy
{

    safe_cache cache;
    httplib::Client client;

public:
    proxy(std::string origin_url) : client(origin_url)
    {
        // expects this format "http://example-server.com" or " http://example-server.com:PORT";
    }
    Response Get(std::string path)
    {
        // check if there is a cached response
        auto res_cached = cache.get(path);
        if (res_cached != std::nullopt)
        {
            return Response{200,
                            res_cached.value()};
        }

        // otherwise make the request   and cache it
        auto res = client.Get(path);
        cache.put(path, res->body);
        return Response{res->status,
                        res->body};
    }
    void invalidate_cache(){
        cache.invalidate();
    }
};
