#include "HtmlContent.hpp"

HtmlContent::HtmlContent(const std::string& content, const std::string& begin,
                         const std::string& end)
{
    // TODO: HTML is not case sensitive, should make all tags the same case. Use libxml instead?
    // TODO: Use regex instead? I've gone down this road with iterative regex matches and had a lot
    // of trouble.
    if(!begin.empty() && !end.empty())
    {
        auto beginIndex = content.find(begin);
        while(beginIndex != std::string::npos)
        {
            beginIndex += begin.size();

            auto endIndex = content.find(end, beginIndex);
            if(endIndex == std::string::npos)
            {
                break;
            }

            const auto length = endIndex - beginIndex;
            mContent += content.substr(beginIndex, length);

            beginIndex = content.find(begin, endIndex);
        }
    }
    else
    {
        mContent = content;
    }
}

HtmlContent HtmlContent::operator()(const std::string& begin, const std::string& end) const
{
    if(const auto extracted = operator()(); !extracted.empty())
    {
        return {extracted, begin, end};
    }

    return {std::string(), std::string(), std::string()};
}
