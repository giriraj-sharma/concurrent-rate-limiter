#pragma once
#include "config.h"
#include "algorithms/i_rate_limiter.h"
#include <memory>
#include <unordered_map>
#include <mutex>
#include <atomic>

struct RequestResult {
    std::string client_id;
    std::string algorithm;
    bool        allowed;
    long long   timestamp_ms;
};

struct Stats {
    long long total = 0, allowed = 0, rejected = 0;
};

class RateLimiterService {
public:
    explicit RateLimiterService(const AppConfig& config);
    RequestResult check(const std::string& client_id);
    void print_summary() const;
private:
    IRateLimiter* get_or_create_limiter(const std::string& client_id);
    static std::unique_ptr<IRateLimiter> make_limiter(const ClientConfig& cfg);
    static long long now_ms();

    AppConfig config_;
    mutable std::mutex map_mutex_;
    std::unordered_map<std::string, std::unique_ptr<IRateLimiter>> limiter_map_;
    mutable std::mutex stats_mutex_;
    std::unordered_map<std::string, Stats> per_client_stats_;
    std::atomic<long long> total_requests_{0};
    std::atomic<long long> total_allowed_{0};
    std::atomic<long long> total_rejected_{0};
};
