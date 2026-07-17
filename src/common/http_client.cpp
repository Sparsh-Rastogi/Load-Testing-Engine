#include "common/http_client.hpp"
#include "common/timer.hpp"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace common {

HttpResponse HttpClient::send_request(const std::string& host, int port,
                                       const HttpRequest& request) {
    HttpResponse response;
    Timer timer;
    timer.start();

    Socket socket;
    if (!socket.connect(host, port)) {
        response.status_code = -1;
        response.status_message = "Connection Failed";
        response.latency_ms = timer.stop();
        return response;
    }

    // Inject Connection: close header if not already present
    HttpRequest final_request = request;
    bool has_connection_header = false;
    for (const auto& [key, val] : final_request.headers) {
        std::string lower_key = key;
        std::transform(lower_key.begin(), lower_key.end(), lower_key.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        if (lower_key == "connection") {
            has_connection_header = true;
            break;
        }
    }
    if (!has_connection_header) {
        final_request.headers["Connection"] = "close";
    }

    std::string serialized = final_request.serialize();
    if (!socket.send(serialized)) {
        response.status_code = -2;
        response.status_message = "Send Failed";
        response.latency_ms = timer.stop();
        return response;
    }

    std::string raw_response;
    size_t content_length = 0;
    bool parsed_headers = false;
    size_t header_end_pos = std::string::npos;

    while (true) {
        std::string chunk = socket.receive(4096);
        if (chunk.empty()) {
            break;
        }
        raw_response += chunk;

        if (!parsed_headers) {
            header_end_pos = raw_response.find("\r\n\r\n");
            if (header_end_pos == std::string::npos) {
                header_end_pos = raw_response.find("\n\n");
            }

            if (header_end_pos != std::string::npos) {
                std::string header_part = raw_response.substr(0, header_end_pos);
                std::istringstream stream(header_part);
                std::string line;

                while (std::getline(stream, line)) {
                    if (!line.empty() && line.back() == '\r') {
                        line.pop_back();
                    }
                    auto colon = line.find(':');
                    if (colon != std::string::npos) {
                        std::string key = line.substr(0, colon);
                        std::string val = line.substr(colon + 1);

                        auto trim = [](const std::string& s) {
                            auto f = std::find_if_not(s.begin(), s.end(), ::isspace);
                            if (f == s.end()) return std::string("");
                            auto l = std::find_if_not(s.rbegin(), s.rend(), ::isspace).base();
                            return std::string(f, l);
                        };

                        std::string clean_key = trim(key);
                        std::transform(clean_key.begin(), clean_key.end(), clean_key.begin(),
                                       [](unsigned char c) { return std::tolower(c); });
                        if (clean_key == "content-length") {
                            try {
                                content_length = std::stoul(trim(val));
                            } catch (...) {
                                content_length = 0;
                            }
                            break;
                        }
                    }
                }
                parsed_headers = true;
            }
        }

        if (parsed_headers && content_length > 0) {
            size_t boundary_len = (raw_response.find("\r\n\r\n") != std::string::npos) ? 4 : 2;
            size_t received_body_bytes = raw_response.length() - (header_end_pos + boundary_len);
            if (received_body_bytes >= content_length) {
                break;
            }
        }
    }

    if (raw_response.empty()) {
        response.status_code = -3;
        response.status_message = "Empty Response";
        response.latency_ms = timer.stop();
        return response;
    }

    response = HttpResponse::parse(raw_response);
    response.latency_ms = timer.stop();
    return response;
}

}  // namespace common
