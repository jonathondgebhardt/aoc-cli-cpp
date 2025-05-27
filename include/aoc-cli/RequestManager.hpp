#pragma once

#include <format>
#include <optional>
#include <string>

#include <curl/curl.h>

#include "HttpsRequest.hpp"
#include "System.hpp"

//! \brief Manages making HTTPS requests to Advent of Code.
//! Subsequent requests will be delayed in an attempt to avoid spamming Advent
//! of COde.
class request_manager
{
public:
    static auto instance() -> request_manager&
    {
        static request_manager manager;
        return manager;
    }

    // TODO: Move should be possible. Copy could be possible with reference
    // counting to prevent unnecessary global cleanup.
    request_manager(const request_manager&) = delete;
    request_manager(request_manager&&) noexcept = delete;

    ~request_manager() { curl_global_cleanup(); }

    auto operator=(const request_manager&) -> request_manager& = delete;
    auto operator=(request_manager&&) noexcept -> request_manager& = delete;

    // TODO: Consider doing file i/o outside of this class and just accepting
    // the session cookie instead.
    //! \brief Set the session cookie file
    void set_session_file(const std::string& session_file);

    // TODO: Consider removing this. Requests should always be delayed.
    //! \brief Set the time to wait between HTTPS requests.
    void set_wait_time(const double wait_time) { m_wait_time = wait_time; }

    //! \brief Set the download prefix to save bookkeeping files.
    void set_bookkeep_file(std::string file)
    {
        m_bookkeeping_file = std::move(file);
    }

    //! \brief Tries to read the file and makes the HTTPS request if it can't be
    //! found.
    auto read_or_download(https_request& request) -> std::string;

    // TODO: Consider making this private. Encapsulating making an HTTPS request
    // or reading from the filesystem is the point of this class.
    //! \brief Performs the HTTPS request and delays it if necessary.
    auto do_request(https_request& request) const -> html_content;

private:
    request_manager() { curl_global_init(CURL_GLOBAL_DEFAULT); }

    auto get_time_to_wait() const -> std::optional<double>;

    std::string m_session_cookie;
    std::string m_download_prefix{".aoc-cli"};
    std::string m_bookkeeping_file{std::format(
        "{}/{}/.lastgetrequest", get_home_path(), m_download_prefix)};
    double m_wait_time{3.0};
};
