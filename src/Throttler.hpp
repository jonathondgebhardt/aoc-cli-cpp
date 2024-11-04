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
        : mRequest(request), mWaitTime(time), mFile(file)
    {
        if(!request)
        {
            throw std::runtime_error("refusing to throttle invalid request");
        }
    }

    ~Throttler() noexcept = default;

    //! \brief If necessary, waits the amount of time, then makes the request.
    HtmlContent handleRequest() const;

private:
    //! Gets the time the last HTTPS request was made.
    //! \return The elapsed time since the last HTTPS request or nullopt if there's no time to wait.
    std::optional<double> getTimeToWait() const;

    HttpsRequest* mRequest{nullptr};
    std::string mFile;
    double mWaitTime = 0.0;
};