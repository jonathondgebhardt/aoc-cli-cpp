#pragma once

#include "HttpsRequest.hpp"

//! \brief A specialization of HttpsRequest specific to Advent of Code.
class AocHttpsRequest : public HttpsRequest
{
public:
    ~AocHttpsRequest() noexcept override = default;
    void setPage(const std::string& page) const { setUrl("https://adventofcode.com/" + page); }
    void setSession(const std::string& session) const
    {
        setCookie(std::format("session={}", session));
    }
};

//! \brief An Advent of Code specific GET request.
class AocGetRequest : public AocHttpsRequest
{
public:
    AocGetRequest() { useGet(); }
    ~AocGetRequest() noexcept override = default;
};

//! \brief An Advent of Code specific POST request.
class AocPostRequest : public AocHttpsRequest
{
public:
    AocPostRequest() { usePost(); }
    ~AocPostRequest() noexcept override = default;

    void setPostContent(const std::string& content)
    {
        // curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, content.c_str());
        curl_easy_setopt(mCurl, CURLOPT_COPYPOSTFIELDS, content.c_str());
    }
};
