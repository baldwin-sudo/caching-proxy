// to be included once
#pragma once

#include <string>
#include <optional>
#include "httplib.h"
#include "cache.hpp"

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
};

class proxy
{
    safe_cache cache;
    httplib::Client client;

public:
    explicit proxy(const std::string &origin_url);

    Response Get(const std::string &path);

    void invalidate_cache();
};