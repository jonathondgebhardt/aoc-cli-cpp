#pragma once

#include <string>

#include <curl/curl.h>

#include "HttpsRequest.hpp"

//! \brief Manages making HTTPS requests to Advent of Code.
//! Subsequent requests will be delayed in an attempt to avoid spamming Advent of COde.
class AocRequestManager
{
public:
    static AocRequestManager& Instance()
    {
        static AocRequestManager manager;
        return manager;
    }

    // TODO: Move should be possible. Copy could be possible with reference counting to prevent
    // unnecessary global cleanup.
    AocRequestManager(const AocRequestManager&) = delete;
    AocRequestManager(AocRequestManager&&) noexcept = delete;
    ~AocRequestManager() { curl_global_cleanup(); }
    AocRequestManager& operator=(const AocRequestManager&) = delete;
    AocRequestManager& operator=(AocRequestManager&&) noexcept = delete;

    // TODO: Consider doing file i/o outside of this class and just accepting the session cookie
    // instead.
    //! \brief Set the session cookie file
    void setSessionFile(const std::string& sessionFile);

    // TODO: Consider removing this. Requests should always be delayed.
    //! \brief Set the time to wait between HTTPS requests.
    void setWaitTime(const double waitTime) { mWaitTime = waitTime; }

    //! \brief Set the download prefix to save book-keeping files.
    void setDownloadPrefix(const std::string& downloadPrefix) { mDownloadPrefix = downloadPrefix; }

    //! \brief Tries to read the file and makes the HTTPS request if it can't be found.
    std::string readOrDownload(const std::string& file, HttpsRequest* request);

    // TODO: Consider making this private. Encapsulating making an HTTPS request or reading from the
    // filesystem is the point of this class.
    //! \brief Performs the HTTPS request and delays it if necessary.
    HtmlContent doRequest(HttpsRequest* request);

private:
    AocRequestManager() { curl_global_init(CURL_GLOBAL_DEFAULT); }

    std::string mSessionCookie;
    std::string mDownloadPrefix{".aoc-cli"};
    double mWaitTime{3.0};
};