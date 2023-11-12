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

    // TODO: Return HtmlContent
    std::optional<HtmlContent> operator()();
    std::optional<HtmlContent> operator()(const std::string& begin, const std::string& end) const;

private:
    CURL* mCurl = nullptr;
    std::string mReadBuffer;
    bool mGetRequested = false;
};
