#include "common/http_client.hpp"

namespace common {

HttpResponse HttpClient::send_request([[maybe_unused]] const std::string& host,
                                       [[maybe_unused]] int port,
                                       [[maybe_unused]] const HttpRequest& request) {
    // TODO(Developer): Send request over TCP socket and receive response, measuring latency.
    return HttpResponse{};
}

}  // namespace common
