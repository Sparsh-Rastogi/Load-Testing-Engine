#include "common/http.hpp"

namespace common {

std::string HttpRequest::serialize() const {
    // TODO(Developer): Serialize HTTP request parameters into raw string representation.
    return "";
}

HttpResponse HttpResponse::parse([[maybe_unused]] const std::string& raw_response) {
    // TODO(Developer): Parse HTTP response string.
    return HttpResponse{};
}

}  // namespace common
