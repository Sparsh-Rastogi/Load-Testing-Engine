#include "common/socket.hpp"
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>

namespace common {

Socket::Socket() {
    fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
}

Socket::~Socket() {
    close();
}

Socket::Socket(Socket&& other) noexcept : fd_(other.fd_) {
    other.fd_ = -1;
}

Socket& Socket::operator=(Socket&& other) noexcept {
    if (this != &other) {
        close();
        fd_ = other.fd_;
        other.fd_ = -1;
    }
    return *this;
}

bool Socket::connect(const std::string& host, int port) {
    if (fd_ == -1) {
        fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ == -1) {
            return false;
        }
    }

    struct addrinfo hints {};
    hints.ai_family = AF_INET;        // IPv4
    hints.ai_socktype = SOCK_STREAM;  // TCP

    struct addrinfo* res = nullptr;
    std::string port_str = std::to_string(port);
    int s = ::getaddrinfo(host.c_str(), port_str.c_str(), &hints, &res);
    if (s != 0) {
        return false;
    }

    bool connected = false;
    for (struct addrinfo* rp = res; rp != nullptr; rp = rp->ai_next) {
        if (::connect(fd_, rp->ai_addr, rp->ai_addrlen) == 0) {
            connected = true;
            break;
        }
    }

    ::freeaddrinfo(res);
    return connected;
}

bool Socket::send(const std::string& data) {
    if (fd_ == -1) {
        return false;
    }

    size_t total_sent = 0;
    size_t len = data.length();
    const char* buf = data.c_str();

    while (total_sent < len) {
        ssize_t sent = ::send(fd_, buf + total_sent, len - total_sent, 0);
        if (sent <= 0) {
            return false;
        }
        total_sent += static_cast<size_t>(sent);
    }
    return true;
}

std::string Socket::receive(size_t max_bytes) {
    if (fd_ == -1) {
        return "";
    }

    std::string response;
    response.resize(max_bytes);
    ssize_t received = ::recv(fd_, &response[0], max_bytes, 0);
    if (received <= 0) {
        return "";
    }
    response.resize(static_cast<size_t>(received));
    return response;
}

void Socket::close() {
    if (fd_ != -1) {
        ::close(fd_);
        fd_ = -1;
    }
}

Socket::Socket(int fd) : fd_(fd) {}

bool Socket::bind(int port) {
    if (fd_ == -1) {
        fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
        if (fd_ == -1) {
            return false;
        }
    }

    int opt = 1;
    ::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr {};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(static_cast<uint16_t>(port));

    return ::bind(fd_, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == 0;
}

bool Socket::listen(int backlog) {
    if (fd_ == -1) {
        return false;
    }
    return ::listen(fd_, backlog) == 0;
}

Socket Socket::accept() {
    if (fd_ == -1) {
        return Socket(-1);
    }
    struct sockaddr_in client_addr {};
    socklen_t client_len = sizeof(client_addr);
    int client_fd =
        ::accept(fd_, reinterpret_cast<struct sockaddr*>(&client_addr), &client_len);
    return Socket(client_fd);
}

}  // namespace common
