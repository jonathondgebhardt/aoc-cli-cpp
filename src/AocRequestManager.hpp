#pragma once

#include <string>

#include "HttpsRequest.hpp"

//! \brief Manages making HTTPS requests to Advent of Code.
//! Subsequent requests will be delayed in an attempt to avoid spamming Advent of COde.
class AocRequestManager
{
public:
    static AocRequestManager& Instance();
    ~AocRequestManager();

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
    AocRequestManager();

    std::string mSessionCookie;
    std::string mDownloadPrefix{".aoc-cli"};
    double mWaitTime{3.0};
};