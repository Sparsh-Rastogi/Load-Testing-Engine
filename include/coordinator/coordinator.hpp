#pragma once

#include "common/config.hpp"
#include "common/socket.hpp"
#include <string>
#include <vector>

namespace coordinator {

class Coordinator {
public:
    explicit Coordinator(const common::CoordinatorConfig& config);

    // Starts listening for workers, awaits all registrations, triggers test, collects and prints metrics.
    void start();
    void stop();

private:
    common::CoordinatorConfig config_;
    common::Socket server_socket_;
    std::vector<common::Socket> worker_sockets_;
    bool running_{false};
};

}  // namespace coordinator
