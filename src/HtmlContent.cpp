#include "HtmlContent.hpp"

#include <regex>

HtmlContent::HtmlContent(const std::string& content, const std::string& begin,
                         const std::string& end)
{
    // TODO: HTML is not case sensitive, should make all tags the same case
    const auto beginIndex = content.find(begin) + begin.size();
    const auto length = content.find(end) - beginIndex;
    mContent = content.substr(beginIndex, length);
}

std::string HtmlContent::operator()() const
{
    return mContent;
}

HtmlContent HtmlContent::operator()(const std::string& begin, const std::string& end) const
{
    if(const auto extracted = operator()(); !extracted.empty())
    {
        return {extracted, begin, end};
    }

    return {std::string(), std::string(), std::string()};
}
