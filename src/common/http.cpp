#include "common/http.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace common {

namespace {
// Local trim helper
std::string trim(const std::string& str) {
    auto first = std::find_if_not(str.begin(), str.end(),
                                  [](unsigned char ch) { return std::isspace(ch); });
    if (first == str.end()) {
        return "";
    }
    auto last = std::find_if_not(str.rbegin(), str.rend(),
                                 [](unsigned char ch) { return std::isspace(ch); })
                    .base();
    return std::string(first, last);
}
}  // namespace

std::string HttpRequest::serialize() const {
    std::string raw;
    raw += method + " " + path + " " + version + "\r\n";

    bool has_content_length = false;
    for (const auto& [key, val] : headers) {
        raw += key + ": " + val + "\r\n";

        std::string lower_key = key;
        std::transform(lower_key.begin(), lower_key.end(), lower_key.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        if (lower_key == "content-length") {
            has_content_length = true;
        }
    }

    if (!body.empty() && !has_content_length) {
        raw += "Content-Length: " + std::to_string(body.length()) + "\r\n";
    }

    raw += "\r\n";
    raw += body;
    return raw;
}

HttpResponse HttpResponse::parse(const std::string& raw_response) {
    HttpResponse response{};

    auto header_end = raw_response.find("\r\n\r\n");
    std::string header_part;
    std::string body_part;

    if (header_end == std::string::npos) {
        header_end = raw_response.find("\n\n");
        if (header_end == std::string::npos) {
            header_part = raw_response;
        } else {
            header_part = raw_response.substr(0, header_end);
            body_part = raw_response.substr(header_end + 2);
        }
    } else {
        header_part = raw_response.substr(0, header_end);
        body_part = raw_response.substr(header_end + 4);
    }

    std::istringstream stream(header_part);
    std::string line;

    // 1. Parse status line
    if (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        std::istringstream status_stream(line);
        std::string version;
        std::string status_code_str;
        std::string status_message;

        status_stream >> version >> status_code_str;
        std::getline(status_stream, status_message);

        response.version = version;
        try {
            response.status_code = std::stoi(status_code_str);
        } catch (...) {
            response.status_code = 0;
        }
        response.status_message = trim(status_message);
    }

    // 2. Parse headers
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        if (line.empty()) {
            continue;
        }

        auto colon = line.find(':');
        if (colon == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, colon);
        std::string val = line.substr(colon + 1);

        response.headers[trim(key)] = trim(val);
    }

    response.body = body_part;
    return response;
}

}  // namespace common
