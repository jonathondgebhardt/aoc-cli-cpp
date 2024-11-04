#pragma once

#include <string>
#include <utility>

#include <curl/curl.h>

#include "HtmlContent.hpp"

//! \brief Performs HTTPS requests.
class HttpsRequest
{
public:
    HttpsRequest();
    HttpsRequest(const HttpsRequest&) = delete;

    HttpsRequest(HttpsRequest&& other) noexcept
    {
        mCurl = std::exchange(other.mCurl, nullptr);
        mReadBuffer = std::exchange(other.mReadBuffer, std::string());
        mSessionFilePath = std::exchange(other.mSessionFilePath, std::string());
        mBegin = std::exchange(other.mBegin, std::string());
        mEnd = std::exchange(other.mEnd, std::string());
        mGetRequested = other.mGetRequested;
    }

    virtual ~HttpsRequest() noexcept;
    HttpsRequest& operator=(const HttpsRequest&) = delete;

    HttpsRequest& operator=(HttpsRequest&& other) noexcept
    {
        if(mCurl)
        {
            curl_easy_cleanup(mCurl);
        }

        mCurl = std::exchange(other.mCurl, nullptr);
        mReadBuffer = std::exchange(other.mReadBuffer, std::string());
        mSessionFilePath = std::exchange(other.mSessionFilePath, std::string());
        mBegin = std::exchange(other.mBegin, std::string());
        mEnd = std::exchange(other.mEnd, std::string());
        mGetRequested = other.mGetRequested;

        return *this;
    }

    //! \brief Set the base URL.
    void setUrl(const std::string& url) const;
    void setUrl(const char* url) const;

    //! \brief Set the content type (e.g. text/html)
    void setContentType(const std::string& type) const;
    void setContentType(const char* type) const;

    //! \brief Set the cookie.
    void setCookie(const std::string& cookie) const;

    //! \brief Set the beginning and end tags.
    //! These tags will be given to HtmlContent.
    void setBeginAndEndTags(const std::string& begin, const std::string& end);

    //! \brief Perform the request.
    //! Only performs a request once.
    HtmlContent operator()();

protected:
    //! \brief Use GET.
    void useGet() const;

    //! \brief Use POST.
    void usePost() const;

    CURL* mCurl = nullptr;

private:
    std::string mReadBuffer;
    std::string mSessionFilePath;
    std::string mBegin;
    std::string mEnd;
    bool mGetRequested = false;
};
