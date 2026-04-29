// Stress test: 5 clients x 30 requests = 150 concurrent requests.
// Run with ThreadSanitizer to verify zero data races:
//   cmake -DCMAKE_CXX_FLAGS="-fsanitize=thread -g" .. && make && ./stress_test
#include "core/rate_limiter_service.h"
#include "core/config.h"
#include "utils/logger.h"
#include <thread>
#include <vector>
#include <iostream>
#include <chrono>

static void simulate_client(RateLimiterService& service,
                             const std::string& client_id,
                             int num_requests)
{
    for (int i = 0; i < num_requests; ++i) {
        auto result = service.check(client_id);
        Logger::log(result.client_id, result.algorithm,
                    result.allowed, result.timestamp_ms);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

int main() {
    AppConfig config = load_config("config.json");
    RateLimiterService service(config);

    const int NUM_CLIENTS = 5;
    const int REQS_EACH   = 30;

    std::cout << "Launching " << NUM_CLIENTS << " clients x "
              << REQS_EACH << " requests = "
              << NUM_CLIENTS * REQS_EACH << " total\n\n";

    std::vector<std::thread> threads;
    threads.reserve(NUM_CLIENTS);

    auto t0 = std::chrono::steady_clock::now();

    for (int i = 0; i < NUM_CLIENTS; ++i) {
        std::string cid = "stress_client_" + std::to_string(i);
        threads.emplace_back(simulate_client, std::ref(service), cid, REQS_EACH);
    }
    for (auto& t : threads) t.join();

    double elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - t0).count();

    service.print_summary();
    std::cout << "\nCompleted in " << elapsed << "s\n";
    return 0;
}
