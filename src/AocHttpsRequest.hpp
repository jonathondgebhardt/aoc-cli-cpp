#pragma once

#include "HttpsRequest.hpp"

class AocHttpsRequest : public HttpsRequest
{
public:
    ~AocHttpsRequest() noexcept override = default;

    void setPage(const std::string& page);
};

class AocGetRequest : public AocHttpsRequest
{
public:
    AocGetRequest();
    ~AocGetRequest() noexcept override = default;
};

class AocPostRequest : public AocHttpsRequest
{
public:
    AocPostRequest();
    ~AocPostRequest() noexcept override = default;

    void setPostContent(const std::string& content);
};
