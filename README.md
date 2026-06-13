# caching-proxy

A simple HTTP caching proxy in C++. Sits between a client and an origin server, caches responses in memory, and serves repeated requests without hitting the origin again.

## Structure

```bash
src/
  cache.cpp      thread-safe in-memory cache
  proxy.cpp      forwards requests, checks cache first
  main.cpp       HTTP server that wires everything together
tests/
  test_cache.cpp cache unit + concurrency tests
  test_proxy.cpp proxy integration + concurrency tests
```

## Build

```bash
mkdir build && cd build
cmake ..
make
```

## Run

```bash
./app <port> <origin>
./app 8080 http://jsonplaceholder.typicode.com
```

Then:

```bash
curl http://localhost:8080/todos/1   # cache miss
curl http://localhost:8080/todos/1   # cache hit
curl -X DELETE http://localhost:8080/cache  # invalidate
```

## Test

```bash
./test_cache
./test_proxy
```

## How it works

- First request to a path → forwarded to origin, response cached
- Subsequent requests → served from cache, origin not contacted
- `DELETE /cache` → clears everything, next requests go to origin again
- Cache reads are concurrent (`shared_mutex`), writes and invalidations are exclusive

## Future Features :

- Add a per request time-based invalidation (TTL)
- Add a per path-invalidation e.g : Delete /cache?path=/todos/1 .
- Persist cache in disk to survive restarts
- Handle other types of responses ( images , files ...) .
