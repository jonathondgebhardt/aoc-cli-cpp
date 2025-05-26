#pragma once

#include <sstream>

#include "HtmlContent.hpp"

//! \brief Extracts all plain text from HtmlContent and attempts to format it.
class html_formatter
{
public:
    explicit html_formatter(const html_content& content)
        : html_formatter(content.extracted())
    {
    }

    explicit html_formatter(const std::string& html) { extract_content(html); }

    html_formatter() = default;
    html_formatter(const html_formatter&) = delete;
    html_formatter(html_formatter&&) noexcept = default;
    ~html_formatter() noexcept = default;
    auto operator=(const html_formatter&) -> html_formatter& = delete;
    auto operator=(html_formatter&&) noexcept -> html_formatter& = default;

    void set_html(const html_content& html)
    {
        extract_content(html.extracted());
    }

    void set_html(const std::string& html) { extract_content(html); }

    //! \brief Yields formatted plain text.
    auto operator()() const -> std::string { return m_stream.str(); }

    static auto format(const html_content& content) -> std::string
    {
        const html_formatter formatter{content};
        return formatter();
    }

private:
    //! \brief Extracts plain text from the HTML content.
    //! \param html The valid HTML content.
    void extract_content(const std::string& html);

    std::stringstream m_stream;
};
