#include "config.h"
#include <fstream>
#include <stdexcept>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static ClientConfig parse_cfg(const json& j) {
    ClientConfig cfg;
    if (j.contains("max_requests"))   cfg.max_requests   = j["max_requests"];
    if (j.contains("window_seconds")) cfg.window_seconds = j["window_seconds"];
    if (j.contains("algorithm"))      cfg.algorithm      = j["algorithm"];
    return cfg;
}

AppConfig load_config(const std::string& path) {
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Cannot open config: " + path);
    json j;
    try { f >> j; }
    catch (const json::exception& e) {
        throw std::runtime_error(std::string("JSON parse error: ") + e.what());
    }
    AppConfig cfg;
    if (j.contains("default")) cfg.default_config = parse_cfg(j["default"]);
    if (j.contains("clients"))
        for (auto& [id, val] : j["clients"].items())
            cfg.per_client[id] = parse_cfg(val);
    return cfg;
}

const ClientConfig& AppConfig::get(const std::string& client_id) const {
    auto it = per_client.find(client_id);
    return (it != per_client.end()) ? it->second : default_config;
}
