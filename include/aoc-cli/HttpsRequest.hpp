#pragma once

#include <string>
#include <utility>

#include <curl/curl.h>

#include "aoc-cli/HtmlContent.hpp"
#include "aoc-cli/aoc-cli_export.hpp"

//! \brief Performs HTTPS requests.
class AOC_CLI_EXPORT https_request
{
public:
    https_request();
    https_request(const https_request& other);
    https_request(https_request&& other) noexcept;
    virtual ~https_request() noexcept;
    auto operator=(const https_request& other) -> https_request&;
    auto operator=(https_request&& other) noexcept -> https_request&;

    //! \brief Set the base URL.
    void set_url(const std::string& url) const;
    void set_url(const char* url) const;

    void set_base_url(std::string base_url)
    {
        m_base_url = std::move(base_url);
    }

    auto get_base_url() const -> std::string { return m_base_url; }

    void set_page(std::string page) { m_page = std::move(page); }

    auto get_page() const -> std::string { return m_page; }

    //! \brief Set the content type (e.g. text/html)
    void set_content_type(const std::string& type) const;
    void set_content_type(const char* type) const;

    //! \brief Set the cookie.
    void set_cookie(const std::string& cookie) const;

    void set_begin_tag(std::string begin) { m_begin = std::move(begin); }

    void set_end_tag(std::string end) { m_end = std::move(end); }

    //! \brief Set the beginning and end tags.
    //! These tags will be given to HtmlContent.
    void set_begin_and_end_tags(const std::string& begin,
                                const std::string& end)
    {
        set_begin_tag(begin);
        set_end_tag(end);
    }

    auto get_begin_tag() const -> std::string { return m_begin; }

    auto get_end_tag() const -> std::string { return m_end; }

    //! \brief Set content to post and enables post mode.
    void set_post_content(const std::string& content);

    //! \brief Perform the request.
    //! Only performs a request once.
    auto operator()() -> html_content;

protected:
    //! \brief Use GET.
    void use_get() const;

    //! \brief Use POST.
    void use_post() const;

private:
    AOC_CLI_CPP_SUPPRESS_C4251
    CURL* m_curl = nullptr;
    std::string m_base_url;
    std::string m_page;
    std::string m_read_buffer;
    std::string m_session_file;
    std::string m_begin;
    std::string m_end;
    std::string m_post_content;
    bool m_request_handled = false;
};
