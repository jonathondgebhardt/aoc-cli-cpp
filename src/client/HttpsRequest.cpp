#include "HttpsRequest.hpp"

#include <iostream>

namespace
{
    // https://stackoverflow.com/questions/9786150/save-curl-content-result-into-a-string-in-c
    size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
    {
        ((std::string*)userp)->append((char*)contents, size * nmemb);
        return size * nmemb;
    }
}

HttpsRequest::HttpsRequest()
{
    mCurl = curl_easy_init();

    if(!mCurl)
    {
        throw std::runtime_error("failed to initialize CURL environment");
    }

    // Disable progress bar
    curl_easy_setopt(mCurl, CURLOPT_NOPROGRESS, 1L);

    // Include user agent information in the header
    // https://www.reddit.com/r/adventofcode/wiki/faqs/automation/
    const auto userAgent =
        "https://github.com/jonathondgebhardt/aoc-cli-cpp by jonathon.gebhardt@gmail.com";
    curl_easy_setopt(mCurl, CURLOPT_USERAGENT, userAgent);

    curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, &mReadBuffer);
}

HttpsRequest::HttpsRequest(const HttpsRequest& other)
{
    mCurl = curl_easy_duphandle(other.mCurl);
    mBaseUrl = other.mBaseUrl;
    mPage = other.mPage;
    mReadBuffer = other.mReadBuffer;
    mSessionFilePath = other.mSessionFilePath;
    mBegin = other.mBegin;
    mEnd = other.mEnd;
    mRequestHandled = other.mRequestHandled;
}

HttpsRequest::HttpsRequest(HttpsRequest&& other) noexcept
{
    mCurl = std::exchange(other.mCurl, nullptr);
    mBaseUrl = std::exchange(other.mBaseUrl, std::string());
    mPage = std::exchange(other.mPage, std::string());
    mReadBuffer = std::exchange(other.mReadBuffer, std::string());
    mSessionFilePath = std::exchange(other.mSessionFilePath, std::string());
    mBegin = std::exchange(other.mBegin, std::string());
    mEnd = std::exchange(other.mEnd, std::string());
    mRequestHandled = other.mRequestHandled;
}

HttpsRequest::~HttpsRequest() noexcept
{
    if(mCurl)
    {
        curl_easy_cleanup(mCurl);
    }
}

HttpsRequest& HttpsRequest::operator=(const HttpsRequest& other)
{
    if(this != &other)
    {
        if(mCurl)
        {
            curl_easy_cleanup(mCurl);
        }

        mCurl = curl_easy_duphandle(other.mCurl);
        mBaseUrl = other.mBaseUrl;
        mPage = other.mPage;
        mReadBuffer = other.mReadBuffer;
        mSessionFilePath = other.mSessionFilePath;
        mBegin = other.mBegin;
        mEnd = other.mEnd;
        mRequestHandled = other.mRequestHandled;
    }

    return *this;
}

HttpsRequest& HttpsRequest::operator=(HttpsRequest&& other) noexcept
{
    if(this != &other)
    {
        if(mCurl)
        {
            curl_easy_cleanup(mCurl);
        }

        mCurl = std::exchange(other.mCurl, nullptr);
        mBaseUrl = std::exchange(other.mBaseUrl, std::string());
        mPage = std::exchange(other.mPage, std::string());
        mReadBuffer = std::exchange(other.mReadBuffer, std::string());
        mSessionFilePath = std::exchange(other.mSessionFilePath, std::string());
        mBegin = std::exchange(other.mBegin, std::string());
        mEnd = std::exchange(other.mEnd, std::string());
        mRequestHandled = other.mRequestHandled;
    }

    return *this;
}

void HttpsRequest::setUrl(const std::string& url) const { setUrl(url.c_str()); }

void HttpsRequest::setUrl(const char* url) const { curl_easy_setopt(mCurl, CURLOPT_URL, url); }

void HttpsRequest::setContentType(const std::string& type) const { setContentType(type.c_str()); }

void HttpsRequest::setContentType(const char* type) const
{
    curl_slist* list = nullptr;
    const std::string content = std::string("Content-Type: ") + type;
    list = curl_slist_append(list, content.c_str());
    curl_easy_setopt(mCurl, CURLOPT_HTTPHEADER, list);
}

void HttpsRequest::setCookie(const std::string& cookie) const
{
    curl_easy_setopt(mCurl, CURLOPT_COOKIE, cookie.c_str());
}

void HttpsRequest::setPostContent(const std::string& content)
{
    // Copy the post content to guarantee no lifetime issues. CURLOPT_POSTFIELDS does not copy
    // the post content.
    mPostContent = content;
    curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, mPostContent.c_str());

    // Make the request a post one.
    curl_easy_setopt(mCurl, CURLOPT_POST, 1L);
}

HtmlContent HttpsRequest::operator()()
{
    if(!mReadBuffer.empty() && mRequestHandled)
    {
        return HtmlContent{mReadBuffer};
    }

    setUrl(std::format("{}/{}", mBaseUrl, mPage));

    if(auto res = curl_easy_perform(mCurl); res == CURLE_OK)
    {
        mRequestHandled = true;
        return HtmlContent{mReadBuffer, mBegin, mEnd};
    }
    else
    {
        long httpCode = 0;
        curl_easy_getinfo(mCurl, CURLINFO_RESPONSE_CODE, &httpCode);

        throw std::runtime_error(std::format("could not perform HTTPS request: {}, {}",
                                             curl_easy_strerror(res), httpCode));
    }
}

void HttpsRequest::useGet() const
{
    curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, &mReadBuffer);
}

void HttpsRequest::usePost() const
{
    curl_easy_setopt(mCurl, CURLOPT_POST, 1L);
    curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, &mReadBuffer);
}
