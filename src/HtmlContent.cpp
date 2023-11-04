#include "HtmlContent.hpp"

#include <regex>

namespace
{
    std::string ReplaceAll(const std::string& x, const std::string& replacement,
                           const std::initializer_list<std::string> replace)
    {
        auto result = x;
        for(const auto& rr : replace)
        {
            result = std::regex_replace(result, std::regex(rr), replacement);
        }

        return result;
    }

    std::string RemoveAll(const std::string& x, const std::string& remove)
    {
        return ReplaceAll(x, std::string(), {remove});
    }

    // TODO: Variadic function?
    std::string RemoveAll(const std::string& x, const std::string& remove1,
                          const std::string& remove2)
    {
        return RemoveAll(RemoveAll(x, remove1), remove2);
    }
}

HtmlContent::HtmlContent(const std::string& content, const std::string& begin,
                         const std::string& end)
{
    // TODO: HTML is not case sensitive, should make all tags the same case
    const auto beginIndex = content.find(begin) + begin.size();
    const auto length = content.find(end) - beginIndex;
    mContent = content.substr(beginIndex, length);
}

// void HtmlContent::excludeContent(const std::string& begin, const std::string& end)
//{
//     mContent = HtmlContent(mContent, begin, end)();
// }

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
