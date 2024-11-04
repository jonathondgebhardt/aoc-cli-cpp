#pragma once

#include <optional>
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
    Throttler(HttpsRequest* request, const double time, std::string file)
        : mRequest(request), mFile(std::move(file)), mWaitTime(time)
    {
        if(!request)
        {
            throw std::runtime_error("refusing to throttle invalid request");
        }
    }

    Throttler() = default;
    Throttler(const Throttler&) = delete;

    Throttler(Throttler&& other) noexcept
    {
        mRequest = std::exchange(other.mRequest, nullptr);
        mFile = std::move(other.mFile);
        mWaitTime = other.mWaitTime;
    }

    ~Throttler() noexcept = default;
    Throttler& operator=(const Throttler&) = delete;

    Throttler& operator=(Throttler&& other) noexcept
    {
        mRequest = std::exchange(other.mRequest, nullptr);
        mFile = std::move(other.mFile);
        mWaitTime = other.mWaitTime;

        return *this;
    }

    //! \brief If necessary, waits the amount of time, then makes the request.
    [[nodiscard]] HtmlContent handleRequest() const;

private:
    //! Gets the time the last HTTPS request was made.
    //! \return The elapsed time since the last HTTPS request or nullopt if there's no time to wait.
    [[nodiscard]] std::optional<double> getTimeToWait() const;

    HttpsRequest* mRequest{nullptr};
    std::string mFile;
    double mWaitTime = 0.0;
};