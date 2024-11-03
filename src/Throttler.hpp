#pragma once

#include <stdexcept>

#include "HttpsRequest.hpp"

//! \brief Attempts to avoid making HTTPS requests too often by waiting an arbitrary amount of time
//! between requests.
class Throttler
{
public:
    //! \param request The request to make.
    //! \param time The amount of time to wait between requests.
    //! \param file The path to write the timestamp file to.
    Throttler(HttpsRequest* request, double time, const std::string& file)
        : mRequest(request), mTime(time), mFile(file)
    {
        if(!request)
        {
            throw std::runtime_error("refusing to throttle invalid request");
        }
    }

    ~Throttler() noexcept = default;

    //! \brief If necessary, waits the amount of time, then makes the request.
    std::optional<HtmlContent> handleRequest();

private:
    HttpsRequest* mRequest{nullptr};
    std::string mFile;
    double mTime = 0.0;
};