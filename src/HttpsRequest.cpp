#include "HttpsRequest.hpp"

#include <fstream>
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
}

HttpsRequest::~HttpsRequest() noexcept
{
    if(mCurl)
    {
        curl_easy_cleanup(mCurl);
    }
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

void HttpsRequest::setBeginAndEndTags(const std::string& begin, const std::string& end)
{
    mBegin = begin;
    mEnd = end;
}

HtmlContent HttpsRequest::operator()()
{
    if(!mReadBuffer.empty() && mGetRequested)
    {
        return HtmlContent{mReadBuffer};
    }

    if(auto res = curl_easy_perform(mCurl); res == CURLE_OK)
    {
        mGetRequested = true;
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
    curl_easy_setopt(mCurl, CURLOPT_READFUNCTION, WriteCallback);
    curl_easy_setopt(mCurl, CURLOPT_READDATA, &mReadBuffer);
}
