#include "core/rate_limiter_service.h"
#include "core/config.h"
#include "utils/logger.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::string config_path = "config.json";
    if (argc > 1) config_path = argv[1];

    AppConfig config;
    try {
        config = load_config(config_path);
    } catch (const std::exception& e) {
        std::cerr << "Config error: " << e.what() << "\n";
        return 1;
    }

    RateLimiterService service(config);
    std::cout << "Rate Limiter ready. Type a client_id and press Enter:\n";
    std::cout << "(Try: client_A  client_B  client_C  or anything else)\n\n";

    std::string client_id;
    while (std::getline(std::cin, client_id)) {
        if (client_id.empty()) continue;
        auto result = service.check(client_id);
        Logger::log(result.client_id, result.algorithm,
                    result.allowed, result.timestamp_ms);
    }

    service.print_summary();
    return 0;
}
