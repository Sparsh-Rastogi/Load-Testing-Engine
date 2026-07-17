# Build Stage
FROM ubuntu:24.04 AS builder
RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY . .
RUN cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build

# Coordinator Stage
FROM ubuntu:24.04 AS coordinator
RUN apt-get update && apt-get install -y iproute2 && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=builder /app/build/src/coordinator/coordinator .
COPY coordinator.conf .
EXPOSE 8080
ENTRYPOINT ["./coordinator"]

# Worker Stage
FROM ubuntu:24.04 AS worker
RUN apt-get update && apt-get install -y iproute2 && rm -rf /var/lib/apt/lists/*
WORKDIR /app
COPY --from=builder /app/build/src/worker/worker .
COPY worker-1.conf .
COPY worker-2.conf .
ENTRYPOINT ["./worker"]
