# Distributed HTTP Load Testing Engine

A high-performance, containerized, distributed HTTP load generation and benchmark engine built using modern **C++20**, raw **POSIX Sockets**, and **multi-threaded ThreadPool orchestration**.

This project demonstrates core systems engineering concepts including low-level network programming, multi-threaded task scheduling, distributed coordination protocols, and containerized deployment.

---

## Key Features

* **Low-Level Networking**: Implements a low-level client/server socket abstraction wrapper directly over raw POSIX socket APIs (`socket()`, `bind()`, `listen()`, `accept()`, `connect()`, `send()`, `recv()`).
* **On-the-Fly HTTP Parser**: A custom HTTP/1.1 serializer and parser that scans headers dynamically for `Content-Length` to achieve safe keep-alive socket boundaries without blocking.
* **ThreadPool Orchestration**: A multi-threaded task pool with templates that enqueues calls asynchronously, binds parameters, and returns `std::future` objects using modern C++ synchronization primitives (`std::mutex`, `std::condition_variable`).
* **Distributed Metrics Aggregation**: The Coordinator acts as a master node, handshaking worker nodes, broadcasting trigger sequences, and aggregating metrics to calculate weighted global average latencies and P95 latency profiles.
* **Containerized Deployment**: Ready-to-run multi-node benchmarking configured via Docker Compose.

---

## Project Structure

```text
Load-Testing-Engine/
├── CMakeLists.txt              # Root CMake configuration
├── Dockerfile                  # Multi-stage Docker builder (Coordinator & Worker)
├── docker-compose.yml          # Container networks (Nginx, Coordinator, Workers)
├── coordinator.conf            # Coordinator test run parameters
├── worker-1.conf               # Worker 1 network configurations
├── worker-2.conf               # Worker 2 network configurations
├── include/                    # Header interfaces
│   ├── common/                 # Shared utilities (sockets, http, thread_pool, logs, timer)
│   ├── coordinator/            # Coordinator definitions
│   └── worker/                 # Worker definitions
├── src/                        # Implementations
│   ├── common/                 # Socket wrappers, Http client, config loader
│   ├── coordinator/            # Coordinator TCP server and metrics aggregator
│   └── worker/                 # Worker load generator loop
└── tests/                      # Unit Tests (GoogleTest)
    ├── common/
    ├── coordinator/
    └── worker/
```

---

## Quick Start (Docker Compose)

The easiest way to execute a load test is using Docker Compose. This automatically spins up a target Nginx server, the Coordinator, and two Workers.

1. **Run the Load Test**:
   ```bash
   docker compose up --build --abort-on-container-exit
   ```
   *The `--abort-on-container-exit` flag ensures Nginx and worker containers tear down automatically as soon as the Coordinator outputs the final aggregated benchmark report.*

2. **Inspect Aggregated Report**:
   ```text
   ================= AGGREGATED BENCHMARK REPORT =================
   Total Requests:       30723
   Successful Requests:  30723
   Failed Requests:      0
   Average Latency:      2.98031 ms
   P95 Latency:          1.63035 ms
   Requests per Second:  6144.6
   ===============================================================
   ```

---

## Manual Compilation & Testing (Local Host)

You can also compile and test the codebase locally on a Linux/WSL system:

1. **Build the Project**:
   ```bash
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   ```

2. **Run Unit Tests**:
   ```bash
   ctest --test-dir build --output-on-failure
   ```

3. **Run Executables Standalone**:
   * Run the Coordinator:
     ```bash
     ./build/src/coordinator/coordinator coordinator.conf
     ```
   * Run a Worker:
     ```bash
     ./build/src/worker/worker worker-1.conf
     ```

---

## Configuration Settings

### Coordinator Configuration (`coordinator.conf`)
```ini
port = 8080                          # Listening port for worker registrations
worker_hosts = worker-1, worker-2    # List of expected workers to register
target_url = http://sample-server/   # URL to benchmark
concurrency = 8                      # Parallel threads to launch per worker
duration_seconds = 5                 # Load test duration in seconds
```

### Worker Configuration (`worker-1.conf`)
```ini
coordinator_host = coordinator       # Hostname/IP of the coordinator
coordinator_port = 8080              # Coordinator port
concurrency = 4                      # Local fallback concurrency
target_url = http://sample-server/   # Local fallback target
duration_seconds = 5                 # Local fallback duration
```
