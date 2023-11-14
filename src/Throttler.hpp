#pragma once

#include "HttpsRequest.hpp"

class Throttler
{
public:
    Throttler(HttpsRequest* request, double time, const std::string& file);
    ~Throttler() noexcept = default;

    std::optional<HtmlContent> handleRequest();

private:
    HttpsRequest* mRequest;
    std::string mFile;
    double mTime = 0.0;
    double mWaitTime = 0.0;
};