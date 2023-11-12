#pragma once

#include <string>

//! Extracts content from HTML between a begin and end tag. Example: <main> and </main>. Extracted
//! content can be accessed via operator().
class HtmlContent
{
public:
    //! No extraction needs to happen, accept content as is.
    explicit HtmlContent(const std::string& content);

    //! Retrieves all content between begin and end tags. If begin and end are empty, same behavior
    //! as constructor taking only content.
    HtmlContent(const std::string& content, const std::string& begin, const std::string& end);
    ~HtmlContent() noexcept = default;

    //! Yields extracted content.
    std::string operator()() const;

    //! Further extracts content between begin and end.
    HtmlContent operator()(const std::string& begin, const std::string& end) const;

private:
    // TODO: Use std::string_view?
    std::string mContent;
};
