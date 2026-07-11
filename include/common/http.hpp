#pragma once

#include <string>
#include <unordered_map>

namespace common {

struct HttpRequest {
    std::string method{"GET"};
    std::string path{"/"};
    std::string version{"HTTP/1.1"};
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    // TODO(Developer): Implement serialization to HTTP raw string.
    std::string serialize() const;
};

struct HttpResponse {
    std::string version;
    int status_code{0};
    std::string status_message;
    std::unordered_map<std::string, std::string> headers;
    std::string body;

    // TODO(Developer): Implement HTTP response parsing.
    static HttpResponse parse(const std::string& raw_response);
};

}  // namespace common
