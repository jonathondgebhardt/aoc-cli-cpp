#pragma once

#include <curl/curl.h>
#include <optional>
#include <string>

#include "HtmlContent.hpp"

//! \brief Performs HTTPS requests.
class HttpsRequest
{
public:
    HttpsRequest();
    virtual ~HttpsRequest() noexcept;

    HttpsRequest(const HttpsRequest&) = default;
    HttpsRequest& operator=(const HttpsRequest&) = delete;

    //! \brief Set the base URL.
    void setUrl(const std::string& url) const;
    void setUrl(const char* url) const;

    //! \brief Set the content type (eg text/html)
    void setContentType(const std::string& type) const;
    void setContentType(const char* type) const;

    // TODO: Delete me.
    void setSessionFilePath(const std::string& path);

    //! \brief Set the cookie.
    void setCookie(const std::string& cookie) const;

    //! \brief Set the begin and end tags.
    //! These tags will be given to HtmlContent.
    void setBeginAndEndTags(const std::string& begin, const std::string& end);

    //! \brief Perform the request.
    //! Only performs a request once.
    std::optional<HtmlContent> operator()();

protected:
    //! \brief Use GET.
    void useGet() const;

    //! \brief Use POST.
    void usePost() const;

    CURL* mCurl = nullptr;

private:
    // TODO: Delete me.
    std::string getCookie() const;

    std::string mReadBuffer;
    std::string mSessionFilePath;
    std::string mBegin;
    std::string mEnd;
    bool mGetRequested = false;
};
