#include "rate_limiter_service.h"
#include "algorithms/fixed_window.h"
#include "algorithms/sliding_window.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <stdexcept>

RateLimiterService::RateLimiterService(const AppConfig& config) : config_(config) {}

std::unique_ptr<IRateLimiter> RateLimiterService::make_limiter(const ClientConfig& cfg) {
    if (cfg.algorithm == "fixed_window")
        return std::make_unique<FixedWindowLimiter>(cfg.max_requests, cfg.window_seconds);
    if (cfg.algorithm == "sliding_window")
        return std::make_unique<SlidingWindowLimiter>(cfg.max_requests, cfg.window_seconds);
    throw std::runtime_error("Unknown algorithm: " + cfg.algorithm);
}

IRateLimiter* RateLimiterService::get_or_create_limiter(const std::string& client_id) {
    std::lock_guard<std::mutex> lock(map_mutex_);
    auto it = limiter_map_.find(client_id);
    if (it == limiter_map_.end()) {
        auto res = limiter_map_.emplace(client_id, make_limiter(config_.get(client_id)));
        return res.first->second.get();
    }
    return it->second.get();
}

RequestResult RateLimiterService::check(const std::string& client_id) {
    IRateLimiter* limiter = get_or_create_limiter(client_id);
    bool allowed = limiter->allow(client_id);
    ++total_requests_;
    if (allowed) ++total_allowed_; else ++total_rejected_;
    {
        std::lock_guard<std::mutex> lock(stats_mutex_);
        auto& s = per_client_stats_[client_id];
        ++s.total;
        if (allowed) ++s.allowed; else ++s.rejected;
    }
    return {client_id, limiter->name(), allowed, now_ms()};
}

void RateLimiterService::print_summary() const {
    std::cout << "\n===== SUMMARY =====\n"
              << "Total    : " << total_requests_ << "\n"
              << "Allowed  : " << total_allowed_  << "\n"
              << "Rejected : " << total_rejected_ << "\n"
              << "\n--- Per-client breakdown ---\n"
              << std::left
              << std::setw(22) << "Client"
              << std::setw(10) << "Total"
              << std::setw(10) << "Allowed"
              << std::setw(10) << "Rejected" << "\n"
              << std::string(52, '-') << "\n";
    std::lock_guard<std::mutex> lock(stats_mutex_);
    for (const auto& [id, s] : per_client_stats_)
        std::cout << std::left
                  << std::setw(22) << id
                  << std::setw(10) << s.total
                  << std::setw(10) << s.allowed
                  << std::setw(10) << s.rejected << "\n";
}

long long RateLimiterService::now_ms() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()).count();
}
