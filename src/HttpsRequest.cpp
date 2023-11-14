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
    curl_global_init(CURL_GLOBAL_DEFAULT);

    mCurl = curl_easy_init();

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

    curl_global_cleanup();
}

void HttpsRequest::setUrl(const std::string& url)
{
    setUrl(url.c_str());
}

void HttpsRequest::setUrl(const char* url)
{
    curl_easy_setopt(mCurl, CURLOPT_URL, url);
}

void HttpsRequest::setContentType(const std::string& type)
{
    setContentType(type.c_str());
}

void HttpsRequest::setContentType(const char* type)
{
    curl_slist* list = nullptr;
    const std::string content = std::string("Content-Type: ") + type;
    list = curl_slist_append(list, content.c_str());
    curl_easy_setopt(mCurl, CURLOPT_HTTPHEADER, list);
}

void HttpsRequest::setSessionFilePath(const std::string& path)
{
    mSessionFilePath = path;

    if(const auto cookie = getCookie(); !cookie.empty())
    {
        curl_easy_setopt(mCurl, CURLOPT_COOKIE, cookie.c_str());
    }
    else
    {
        std::cerr << "Could not load session file: " << path << "\n";
    }
}

void HttpsRequest::setBeginAndEndTags(const std::string& begin, const std::string& end)
{
    mBegin = begin;
    mEnd = end;
}

std::optional<HtmlContent> HttpsRequest::operator()()
{
    if(!mReadBuffer.empty() && mGetRequested)
    {
        return HtmlContent{mReadBuffer};
    }

    if(mCurl)
    {
        auto res = curl_easy_perform(mCurl);
        long httpCode = 0;
        curl_easy_getinfo(mCurl, CURLINFO_RESPONSE_CODE, &httpCode);
        std::cout << "http code: " << httpCode;
        if(res == CURLE_OK)
        {
            mGetRequested = true;
            return HtmlContent{mReadBuffer, mBegin, mEnd};
        }
        else
        {
            std::cerr << "Could not perform HTTPS request: " << curl_easy_strerror(res) << "\n";
        }
    }
    else
    {
        std::cerr << "Could initialize CURL environment\n";
    }

    return {};
}

std::optional<HtmlContent> HttpsRequest::operator()(const std::string& begin,
                                                    const std::string& end)
{
    if(const auto content = operator()())
    {
        return (*content)(begin, end);
    }

    return {};
}
std::string HttpsRequest::getCookie() const
{
    std::ifstream ifs{mSessionFilePath};
    if(ifs.is_open())
    {
        std::string line;
        std::getline(ifs, line);
        return "session=" + line;
    }

    return {};
}

void HttpsRequest::useGet()
{
    curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, &mReadBuffer);
}

void HttpsRequest::usePost()
{
    curl_easy_setopt(mCurl, CURLOPT_POST, 1L);
    curl_easy_setopt(mCurl, CURLOPT_READFUNCTION, WriteCallback);
    curl_easy_setopt(mCurl, CURLOPT_READDATA, &mReadBuffer);
}
