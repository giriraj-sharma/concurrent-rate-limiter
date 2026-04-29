#pragma once
#include "i_rate_limiter.h"
#include <unordered_map>
#include <deque>
#include <mutex>

// Sliding Window Log: stores timestamp of every request in a deque.
// More accurate than Fixed Window — no burst at window boundary.
// Trade-off: uses O(max_requests) memory per client vs O(1) for Fixed Window.
class SlidingWindowLimiter : public IRateLimiter {
public:
    SlidingWindowLimiter(int max_requests, int window_seconds);
    bool allow(const std::string& client_id) override;
    std::string name() const override { return "sliding_window"; }
private:
    int       max_requests_;
    long long window_ms_;
    std::unordered_map<std::string, std::deque<long long>> request_log_;
    std::mutex mutex_;
    static long long now_ms();
};
