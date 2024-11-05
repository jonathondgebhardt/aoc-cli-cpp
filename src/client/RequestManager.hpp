#pragma once

#include <format>
#include <optional>
#include <string>

#include <curl/curl.h>

#include "HttpsRequest.hpp"
#include "System.hpp"

//! \brief Manages making HTTPS requests to Advent of Code.
//! Subsequent requests will be delayed in an attempt to avoid spamming Advent of COde.
class RequestManager
{
public:
    static RequestManager& Instance()
    {
        static RequestManager manager;
        return manager;
    }

    // TODO: Move should be possible. Copy could be possible with reference counting to prevent
    // unnecessary global cleanup.
    RequestManager(const RequestManager&) = delete;
    RequestManager(RequestManager&&) noexcept = delete;
    ~RequestManager() { curl_global_cleanup(); }
    RequestManager& operator=(const RequestManager&) = delete;
    RequestManager& operator=(RequestManager&&) noexcept = delete;

    // TODO: Consider doing file i/o outside of this class and just accepting the session cookie
    // instead.
    //! \brief Set the session cookie file
    void setSessionFile(const std::string& sessionFile);

    // TODO: Consider removing this. Requests should always be delayed.
    //! \brief Set the time to wait between HTTPS requests.
    void setWaitTime(const double waitTime) { mWaitTime = waitTime; }

    //! \brief Set the download prefix to save book-keeping files.
    void setBookkeepFile(std::string file) { mBookkeepingFile = std::move(file); }

    //! \brief Tries to read the file and makes the HTTPS request if it can't be found.
    std::string readOrDownload(HttpsRequest& request);

    // TODO: Consider making this private. Encapsulating making an HTTPS request or reading from the
    // filesystem is the point of this class.
    //! \brief Performs the HTTPS request and delays it if necessary.
    HtmlContent doRequest(HttpsRequest& request) const;

private:
    RequestManager() { curl_global_init(CURL_GLOBAL_DEFAULT); }

    std::optional<double> getTimeToWait() const;

    std::string mSessionCookie;
    std::string mDownloadPrefix{".aoc-cli"};
    std::string mBookkeepingFile{
        std::format("{}/{}/.lastgetrequest", GetHomePath(), mDownloadPrefix)};
    double mWaitTime{3.0};
};