#pragma once

#include <string>

//! \brief Extracts content from HTML between a beginning and end tag. Example:
//! <main> and </main>. Extracted content can be accessed via operator().
class html_content
{
public:
    //! \brief No extraction needs to happen, accept content as is.
    explicit html_content(std::string content)
        : m_content(std::move(content))
    {
    }

    //! \brief Retrieves all content between begin and end tags. If begin and
    //! end are empty, same behavior as constructor taking only content.
    html_content(std::string content, std::string begin, std::string end)
        : m_content(std::move(content))
        , m_begin(std::move(begin))
        , m_end(std::move(end))
    {
        extract();
    }

    html_content() = default;
    html_content(const html_content&) = default;
    html_content(html_content&&) noexcept = default;
    ~html_content() noexcept = default;
    auto operator=(const html_content&) -> html_content& = default;
    auto operator=(html_content&&) noexcept -> html_content& = default;

    void set_content(std::string content) { m_content = std::move(content); }

    auto extract() -> std::string;

    auto extract(const std::string& begin, const std::string& end)
        -> html_content
    {
        return html_content{extract(), begin, end};
    }

    auto plain() const -> std::string { return m_content; }

    auto extracted() const -> std::string { return m_extracted_content; }

private:
    // TODO: Use std::string_view?
    std::string m_content;
    std::string m_extracted_content;
    std::string m_begin;
    std::string m_end;
};
