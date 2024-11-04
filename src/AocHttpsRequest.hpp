#pragma once

#include "HttpsRequest.hpp"

// TODO: Consider deleting these specializations

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
        // Copy the post content to guarantee no lifetime issues. CURLOPT_POSTFIELDS does not copy
        // the post content.
        mPostContent = content;
        curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, mPostContent.c_str());
    }

private:
    std::string mPostContent;
};
