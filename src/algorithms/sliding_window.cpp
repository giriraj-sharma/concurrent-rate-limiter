#include "sliding_window.h"
#include <chrono>

SlidingWindowLimiter::SlidingWindowLimiter(int max_requests, int window_seconds)
    : max_requests_(max_requests),
      window_ms_(static_cast<long long>(window_seconds) * 1000) {}

bool SlidingWindowLimiter::allow(const std::string& client_id) {
    long long now    = now_ms();
    long long cutoff = now - window_ms_;
    std::lock_guard<std::mutex> lock(mutex_);
    auto& log = request_log_[client_id];
    // Evict timestamps outside the sliding window
    while (!log.empty() && log.front() <= cutoff)
        log.pop_front();
    if (static_cast<int>(log.size()) < max_requests_) {
        log.push_back(now);
        return true;   // ALLOWED
    }
    return false;      // RATE_LIMITED
}

long long SlidingWindowLimiter::now_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()).count();
}
