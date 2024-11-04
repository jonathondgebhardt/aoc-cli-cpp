#pragma once

#include <string>

//! \brief Extracts content from HTML between a beginning and end tag. Example: <main> and </main>.
//! Extracted content can be accessed via operator().
class HtmlContent
{
public:
    //! \brief No extraction needs to happen, accept content as is.
    explicit HtmlContent(std::string content) : mContent(std::move(content)) {}

    //! \brief Retrieves all content between begin and end tags. If begin and end are empty, same
    //! behavior as constructor taking only content.
    HtmlContent(std::string content, std::string begin, std::string end)
        : mContent(std::move(content)), mBegin(std::move(begin)), mEnd(std::move(end))
    {
        extract();
    }

    HtmlContent() = default;
    HtmlContent(const HtmlContent&) = default;
    HtmlContent(HtmlContent&&) noexcept = default;
    ~HtmlContent() noexcept = default;
    HtmlContent& operator=(const HtmlContent&) = default;
    HtmlContent& operator=(HtmlContent&&) noexcept = default;

    void setContent(std::string content) { mContent = std::move(content); }

    std::string extract();
    HtmlContent extract(const std::string& begin, const std::string& end)
    {
        return HtmlContent{extract(), begin, end};
    }

    std::string plain() const { return mContent; }
    std::string extracted() const { return mExtractedContent; }

private:
    // TODO: Use std::string_view?
    std::string mContent;
    std::string mExtractedContent;
    std::string mBegin;
    std::string mEnd;
};
