#pragma once

#include <optional>
#include <regex>
#include <string>
#include <utility>

//! Extracts content from HTML between a begin and end tag. Example: <main> and </main>. Extracted
//! content can be accessed via operator().
class HtmlContent
{
public:
    HtmlContent(const std::string& content, const std::string& begin, const std::string& end);
    ~HtmlContent() noexcept = default;

    //    template <typename Type>
    //    void excludeTags(Type&& arg) noexcept
    //    {
    //        std::regex re{arg};
    //        mContent = std::regex_replace(mContent, re, std::string());
    //    }
    //
    //    template <typename... Args>
    //    void excludeTags(Args&&... args) noexcept
    //    {
    //        (excludeTags(std::forward<Args>(args)), ...);
    //    }

    //    void excludeContent(const std::string& begin, const std::string& end);

    std::string operator()() const;
    HtmlContent operator()(const std::string& begin, const std::string& end) const;

private:
    // TODO: Use std::string_view?
    std::string mContent;
};
