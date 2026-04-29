#include "fixed_window.h"
#include <chrono>

FixedWindowLimiter::FixedWindowLimiter(int max_requests, int window_seconds)
    : max_requests_(max_requests),
      window_ms_(static_cast<long long>(window_seconds) * 1000) {}

bool FixedWindowLimiter::allow(const std::string& client_id) {
    long long now = now_ms();
    // Acquire lock — one mutex protects the whole map.
    // Trade-off: simple and correct; a per-key lock would scale better.
    std::lock_guard<std::mutex> lock(mutex_);
    auto& state = client_state_[client_id];
    // If window has expired, reset it
    if (now - state.window_start_ms >= window_ms_) {
        state.window_start_ms = now;
        state.count = 0;
    }
    if (state.count < max_requests_) {
        ++state.count;
        return true;   // ALLOWED
    }
    return false;      // RATE_LIMITED
}

long long FixedWindowLimiter::now_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()).count();
}
