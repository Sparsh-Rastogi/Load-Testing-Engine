#pragma once

#include "common/http.hpp"
#include "common/socket.hpp"
#include <string>

namespace common {

class HttpClient {
public:
    HttpClient() = default;

    // TODO(Developer): Implement an HTTP client.
    // - Connect to a target.
    // - Send HTTP Requests.
    // - Parse and return HTTP Responses.
    // - Measure latency.
    HttpResponse send_request(const std::string& host, int port, const HttpRequest& request);
};

}  // namespace common
