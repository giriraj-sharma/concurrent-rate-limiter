#pragma once
#include <string>
#include <unordered_map>

struct ClientConfig {
    int         max_requests   = 10;
    int         window_seconds = 60;
    std::string algorithm      = "sliding_window";
};

struct AppConfig {
    ClientConfig                                  default_config;
    std::unordered_map<std::string, ClientConfig> per_client;
    // Returns per-client config if defined, otherwise returns default
    const ClientConfig& get(const std::string& client_id) const;
};

AppConfig load_config(const std::string& path);
