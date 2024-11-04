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
    HtmlContent(const std::string& content, const std::string& begin, const std::string& end);

    HtmlContent() = default;
    HtmlContent(const HtmlContent&) = default;
    HtmlContent(HtmlContent&&) noexcept = default;
    ~HtmlContent() noexcept = default;
    HtmlContent& operator=(const HtmlContent&) = default;
    HtmlContent& operator=(HtmlContent&&) noexcept = default;

    void setContent(std::string content) { mContent = std::move(content); }

    //! \brief Yields extracted content.
    std::string operator()() const { return mContent; }

    //! \brief Further extracts content between begin and end.
    HtmlContent operator()(const std::string& begin, const std::string& end) const;

private:
    // TODO: Use std::string_view?
    std::string mContent;
};
