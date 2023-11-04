#include "HtmlContent.hpp"

#include <regex>

namespace
{
    std::string RemoveAll(const std::string& x, const std::string& remove)
    {
        std::regex re{remove};
        return std::regex_replace(x, re, std::string());
    }

    std::string RemoveAll(const std::string& x, const std::string& remove1,
                          const std::string& remove2)
    {
        return RemoveAll(RemoveAll(x, remove1), remove2);
    }
}

HtmlContent::HtmlContent(const std::string& content, const std::string& begin,
                         const std::string& end, bool deep)
{
    const auto beginIndex = content.find(begin) + begin.size();
    const auto length = deep ? content.find_last_of(end) : content.find(end) - beginIndex;

    // TODO: Use HtmlContent::exclude
    mContent = deep ? RemoveAll(content, begin, end) : content.substr(beginIndex, length);
}

void HtmlContent::excludeContent(const std::string& begin, const std::string& end)
{
    mContent = HtmlContent(mContent, begin, end)();
}

std::string HtmlContent::operator()() const
{
    return mContent;
}

HtmlContent HtmlContent::operator()(const std::string& begin, const std::string& end,
                                    bool deep) const
{
    if(const auto extracted = operator()(); !extracted.empty())
    {
        return {extracted, begin, end, deep};
    }

    return {std::string(), std::string(), std::string()};
}
