#pragma once

#include <sstream>

#include "HtmlContent.hpp"

//! \brief Extracts all plain text from HtmlContent and attempts to format it.
class HtmlFormatter
{
public:
    explicit HtmlFormatter(const HtmlContent& content) : HtmlFormatter(content()) {}
    explicit HtmlFormatter(const std::string& html) { extractContent(html); }

    HtmlFormatter() = default;
    HtmlFormatter(const HtmlFormatter&) = default;
    HtmlFormatter(HtmlFormatter&&) noexcept = default;
    ~HtmlFormatter() noexcept = default;
    HtmlFormatter& operator=(const HtmlFormatter&) = default;
    HtmlFormatter& operator=(HtmlFormatter&&) noexcept = default;

    void setHtml(const HtmlContent& html) { extractContent(html()); }
    void setHtml(const std::string& html) { extractContent(html); }

    //! \brief Yields formatted plain text.
    std::string operator()() const { return mStream.str(); }

    static std::string Format(const HtmlContent& content)
    {
        const HtmlFormatter formatter{content};
        return formatter();
    }

private:
    //! \brief Extracts plain text from the HTML content.
    //! \param html The valid HTML content.
    void extractContent(const std::string& html);

    std::stringstream mStream;
};
