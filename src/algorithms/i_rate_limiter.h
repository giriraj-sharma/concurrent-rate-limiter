#pragma once
#include <string>

// Every algorithm must implement this interface.
// This lets us swap algorithms at runtime without changing the service layer.
class IRateLimiter {
public:
    virtual ~IRateLimiter() = default;
    virtual bool allow(const std::string& client_id) = 0;
    virtual std::string name() const = 0;
};
