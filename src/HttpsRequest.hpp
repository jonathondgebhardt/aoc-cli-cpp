#pragma once

#include <curl/curl.h>
#include <optional>
#include <string>

#include "HtmlContent.hpp"

class HttpsRequest
{
public:
    HttpsRequest();
    virtual ~HttpsRequest() noexcept;

    HttpsRequest(const HttpsRequest&) = default;
    HttpsRequest& operator=(const HttpsRequest&) = delete;

    void setUrl(const std::string& url);
    void setUrl(const char* url);

    void setContentType(const std::string& type);
    void setContentType(const char* type);

    void setSessionFilePath(const std::string& path);

    void setBeginAndEndTags(const std::string& begin, const std::string& end);

    std::optional<HtmlContent> operator()();
    std::optional<HtmlContent> operator()(const std::string& begin, const std::string& end);

private:
    std::string getCookie() const;

    CURL* mCurl = nullptr;
    std::string mReadBuffer;
    std::string mSessionFilePath;
    std::string mBegin;
    std::string mEnd;
    bool mGetRequested = false;
};
