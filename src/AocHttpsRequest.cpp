#include "AocHttpsRequest.hpp"

void AocHttpsRequest::setPage(const std::string& page)
{
    setUrl("https://adventofcode.com/" + page);
}

AocGetRequest::AocGetRequest() : AocHttpsRequest()
{
    useGet();
}

AocPostRequest::AocPostRequest() : AocHttpsRequest()
{
    usePost();
}

void AocPostRequest::setPostContent(const std::string& content)
{
    curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, content.c_str());
}
