#pragma once

#include <string>

namespace common {

class Socket {
public:
    Socket();
    ~Socket();

    // Prevent copy, allow move
    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&& other) noexcept;
    Socket& operator=(Socket&& other) noexcept;

    // TODO(Developer): Implement standard POSIX TCP socket operations.
    // - socket(), connect(), send(), recv(), close()
    // - Provide RAII safety to prevent resource leaks.

    bool connect(const std::string& host, int port);
    bool send(const std::string& data);
    std::string receive(size_t max_bytes = 4096);
    void close();

    // Server-side operations
    bool bind(int port);
    bool listen(int backlog = 10);
    Socket accept();

    int fd() const { return fd_; }

    // Construct wrapper from an existing file descriptor
    explicit Socket(int fd);

private:
    int fd_{-1};
};

}  // namespace common
