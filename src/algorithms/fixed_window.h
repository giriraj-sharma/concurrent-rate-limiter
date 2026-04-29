#pragma once
#include "i_rate_limiter.h"
#include <unordered_map>
#include <mutex>

class FixedWindowLimiter : public IRateLimiter {
public:
    FixedWindowLimiter(int max_requests, int window_seconds);
    bool allow(const std::string& client_id) override;
    std::string name() const override { return "fixed_window"; }
private:
    struct WindowState {
        long long window_start_ms = 0;
        int       count           = 0;
    };
    int       max_requests_;
    long long window_ms_;
    std::unordered_map<std::string, WindowState> client_state_;
    std::mutex mutex_;
    static long long now_ms();
};
