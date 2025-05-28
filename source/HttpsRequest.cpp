#include <cstddef>
#include <cstdint>
#include <format>
#include <stdexcept>
#include <utility>

#include "aoc-cli/HttpsRequest.hpp"

#include <curl/curl.h>
#include <curl/easy.h>

#include "aoc-cli/HtmlContent.hpp"

namespace
{
// https://stackoverflow.com/questions/9786150/save-curl-content-result-into-a-string-in-c
auto write_callback(void* contents,
                    size_t size,
                    size_t nmemb,
                    void* userp) -> size_t
{
    static_cast<std::string*>(userp)->append(static_cast<char*>(contents),
                                             size * nmemb);
    return size * nmemb;
}
}  // namespace

https_request::https_request()
    : m_curl{curl_easy_init()}
{
    if (m_curl == nullptr) {
        throw std::runtime_error("failed to initialize CURL environment");
    }

    // Disable progress bar
    curl_easy_setopt(m_curl, CURLOPT_NOPROGRESS, 1L);

    // Include user agent information in the header
    // https://www.reddit.com/r/adventofcode/wiki/faqs/automation/
    const auto *const user_agent =
        "https://github.com/jonathondgebhardt/aoc-cli-cpp by "
        "jonathon.gebhardt@gmail.com";
    curl_easy_setopt(m_curl, CURLOPT_USERAGENT, user_agent);

    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_read_buffer);
}

https_request::https_request(const https_request& other)
    : m_curl{curl_easy_duphandle(other.m_curl)}
    , m_base_url{other.m_base_url}
    , m_page{other.m_page}
    , m_read_buffer{other.m_read_buffer}
    , m_session_file{other.m_session_file}
    , m_begin{other.m_begin}
    , m_end{other.m_end}
    , m_request_handled{other.m_request_handled}
{
}

https_request::https_request(https_request&& other) noexcept
    : m_curl{std::exchange(other.m_curl, nullptr)}
    , m_base_url{std::exchange(other.m_base_url, std::string())}
    , m_page{std::exchange(other.m_page, std::string())}
    , m_read_buffer{std::exchange(other.m_read_buffer, std::string())}
    , m_session_file{std::exchange(other.m_session_file, std::string())}
    , m_begin{std::exchange(other.m_begin, std::string())}
    , m_end{std::exchange(other.m_end, std::string())}
    , m_request_handled{other.m_request_handled}
{
}

https_request::~https_request() noexcept
{
    if (m_curl != nullptr) {
        curl_easy_cleanup(m_curl);
    }
}

auto https_request::operator=(const https_request& other) -> https_request&
{
    if (this != &other) {
        if (m_curl != nullptr) {
            curl_easy_cleanup(m_curl);
        }

        m_curl = curl_easy_duphandle(other.m_curl);
        m_base_url = other.m_base_url;
        m_page = other.m_page;
        m_read_buffer = other.m_read_buffer;
        m_session_file = other.m_session_file;
        m_begin = other.m_begin;
        m_end = other.m_end;
        m_request_handled = other.m_request_handled;
    }

    return *this;
}

auto https_request::operator=(https_request&& other) noexcept -> https_request&
{
    if (this != &other) {
        if (m_curl != nullptr) {
            curl_easy_cleanup(m_curl);
        }

        m_curl = std::exchange(other.m_curl, nullptr);
        m_base_url = std::exchange(other.m_base_url, std::string());
        m_page = std::exchange(other.m_page, std::string());
        m_read_buffer = std::exchange(other.m_read_buffer, std::string());
        m_session_file = std::exchange(other.m_session_file, std::string());
        m_begin = std::exchange(other.m_begin, std::string());
        m_end = std::exchange(other.m_end, std::string());
        m_request_handled = other.m_request_handled;
    }

    return *this;
}

void https_request::set_url(const std::string& url) const
{
    set_url(url.c_str());
}

void https_request::set_url(const char* url) const
{
    curl_easy_setopt(m_curl, CURLOPT_URL, url);
}

void https_request::set_content_type(const std::string& type) const
{
    set_content_type(type.c_str());
}

void https_request::set_content_type(const char* type) const
{
    curl_slist* list = nullptr;
    const std::string content = std::string("Content-Type: ") + type;
    list = curl_slist_append(list, content.c_str());
    curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, list);
}

void https_request::set_cookie(const std::string& cookie) const
{
    curl_easy_setopt(m_curl, CURLOPT_COOKIE, cookie.c_str());
}

void https_request::set_post_content(const std::string& content)
{
    // Copy the post content to guarantee no lifetime issues. CURLOPT_POSTFIELDS
    // does not copy the post content.
    m_post_content = content;
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, m_post_content.c_str());

    // Make the request a post one.
    curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
}

auto https_request::operator()() -> html_content
{
    if (!m_read_buffer.empty() && m_request_handled) {
        return html_content{m_read_buffer};
    }

    set_url(std::format("{}/{}", m_base_url, m_page));

    auto res = curl_easy_perform(m_curl);
    if (res == CURLE_OK) {
        m_request_handled = true;
        return html_content{m_read_buffer, m_begin, m_end};
    }

    std::int64_t http_code = 0;
    curl_easy_getinfo(m_curl, CURLINFO_RESPONSE_CODE, &http_code);

    throw std::runtime_error(
        std::format("could not perform HTTPS request: {}, {}",
                    curl_easy_strerror(res),
                    http_code));
}

void https_request::use_get() const
{
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_read_buffer);
}

void https_request::use_post() const
{
    curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &m_read_buffer);
}
