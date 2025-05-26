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
    HttpsRequest(const HttpsRequest& other);
    HttpsRequest(HttpsRequest&& other) noexcept;
    virtual ~HttpsRequest() noexcept;
    HttpsRequest& operator=(const HttpsRequest& other);
    HttpsRequest& operator=(HttpsRequest&& other) noexcept;

    //! \brief Set the base URL.
    void setUrl(const std::string& url) const;
    void setUrl(const char* url) const;

    void setBaseUrl(std::string baseUrl) { mBaseUrl = std::move(baseUrl); }
    std::string getBaseUrl() const { return mBaseUrl; }

    void setPage(std::string page) { mPage = std::move(page); }
    std::string getPage() const { return mPage; }

    //! \brief Set the content type (e.g. text/html)
    void setContentType(const std::string& type) const;
    void setContentType(const char* type) const;

    //! \brief Set the cookie.
    void setCookie(const std::string& cookie) const;

    void setBeginTag(std::string begin) { mBegin = std::move(begin); }
    void setEndTag(std::string end) { mEnd = std::move(end); }

    //! \brief Set the beginning and end tags.
    //! These tags will be given to HtmlContent.
    void setBeginAndEndTags(const std::string& begin, const std::string& end)
    {
        setBeginTag(begin);
        setEndTag(end);
    }

    std::string getBeginTag() const { return mBegin; }
    std::string getEndTag() const { return mEnd; }

    //! \brief Set content to post and enables post mode.
    void setPostContent(const std::string& content);

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
    std::string mBaseUrl;
    std::string mPage;
    std::string mReadBuffer;
    std::string mSessionFilePath;
    std::string mBegin;
    std::string mEnd;
    std::string mPostContent;
    bool mRequestHandled = false;
};
