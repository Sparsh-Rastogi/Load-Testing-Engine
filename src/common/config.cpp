#include "common/config.hpp"
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace common {

namespace {
// Trim whitespace helper
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

// Split string by a delimiter
std::vector<std::string> split(const std::string& str, char delim) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);
    while (std::getline(tokenStream, token, delim)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}
}  // namespace

WorkerConfig Config::load_worker_config(const std::string& filepath) {
    WorkerConfig config{};
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return config;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        auto delim_pos = line.find('=');
        if (delim_pos == std::string::npos) {
            continue;
        }

        std::string key = trim(line.substr(0, delim_pos));
        std::string val = trim(line.substr(delim_pos + 1));

        if (key == "coordinator_host") {
            config.coordinator_host = val;
        } else if (key == "coordinator_port") {
            config.coordinator_port = std::stoi(val);
        } else if (key == "concurrency") {
            config.concurrency = std::stoi(val);
        } else if (key == "target_url") {
            config.target_url = val;
        } else if (key == "duration_seconds") {
            config.duration_seconds = std::stoi(val);
        }
    }

    return config;
}

CoordinatorConfig Config::load_coordinator_config(const std::string& filepath) {
    CoordinatorConfig config{};
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return config;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        auto delim_pos = line.find('=');
        if (delim_pos == std::string::npos) {
            continue;
        }

        std::string key = trim(line.substr(0, delim_pos));
        std::string val = trim(line.substr(delim_pos + 1));

        if (key == "port") {
            config.port = std::stoi(val);
        } else if (key == "worker_hosts") {
            config.worker_hosts = split(val, ',');
        } else if (key == "target_url") {
            config.target_url = val;
        } else if (key == "concurrency") {
            config.concurrency = std::stoi(val);
        } else if (key == "duration_seconds") {
            config.duration_seconds = std::stoi(val);
        }
    }

    return config;
}

}  // namespace common
