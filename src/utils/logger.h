#pragma once
#include <iostream>
#include <mutex>
#include <string>

// Thread-safe logger — prevents garbled output when multiple threads print
class Logger {
public:
    static void log(const std::string& client_id,
                    const std::string& algorithm,
                    bool allowed,
                    long long timestamp_ms)
    {
        static std::mutex log_mutex;
        std::lock_guard<std::mutex> lock(log_mutex);
        std::cout << "[" << timestamp_ms << "] "
                  << client_id << " | "
                  << algorithm << " | "
                  << (allowed ? "ALLOWED" : "RATE_LIMITED") << "\n";
    }
};
