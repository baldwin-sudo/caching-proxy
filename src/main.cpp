// src/main.cpp
#include <iostream>
#include "httplib.h"
#include "proxy.hpp"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: app <port> <origin_url>\n";
        std::cerr << "Example: app 8080 http://jsonplaceholder.typicode.com\n";
        return 1;
    }

    int port = std::stoi(argv[1]);
    std::string origin = argv[2];

    proxy p(origin);
    httplib::Server svr;

    // catch-all GET handler
    svr.Get(".*", [&p](const httplib::Request &req, httplib::Response &res)
            {
        auto result = p.Get(req.path);
        res.status = result.status;
        res.set_content(result.body, "application/json"); });

    // invalidate cache route
    svr.Delete("/cache", [&p](const httplib::Request &, httplib::Response &res)
               {
    p.invalidate_cache();
    res.set_content("cache cleared", "text/plain"); });

    std::cout << "Listening on port " << port << " → " << origin << "\n";
    svr.listen("0.0.0.0", port);

    return 0;
}