#include <chrono>
#include <cstdint>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <print>
#include <stdexcept>
#include <string>
#include <thread>

#include "aoc-cli/RequestManager.hpp"

#include <sys/stat.h>

#include "aoc-cli/HtmlContent.hpp"
#include "aoc-cli/HtmlFormatter.hpp"
#include "aoc-cli/HttpsRequest.hpp"
#include "aoc-cli/System.hpp"

namespace
{
auto get_session_cookie(const std::string& session_file) -> std::string
{
    std::ifstream ifs{session_file};

    if (!ifs.is_open()) {
        throw std::runtime_error(
            std::format("could not open session file: {}", session_file));
    }

    std::string line;
    std::getline(ifs, line);
    return "session=" + line;
}

auto read_file_into_string(const std::string& file) -> std::string
{
    if (std::filesystem::exists(file)) {
        std::ifstream ifs(file);
        return {(std::istreambuf_iterator<char>(ifs)),
                std::istreambuf_iterator<char>()};
    }

    return {};
}

auto create_if_does_not_exist(const std::filesystem::path& path) -> bool
{
    if (!std::filesystem::exists(path)) {
        try {
            std::filesystem::create_directories(path);
            return true;
        } catch (...) {
            std::cerr << "Could not create directory '" << path << "'\n";
            // std::println(std::cerr, "Could not create directory '{}'", path);
        }
    }

    return false;
}

//! Updates the time last modified on the file.
//! \param file The file.
void touch_file(const std::string& file)
{
    if (std::filesystem::exists(file)) {
        std::filesystem::remove(file);
    }

    const std::filesystem::path path{file};
    if (const auto parent = path.parent_path();
        !std::filesystem::exists(parent))
    {
        // TODO: Should this be responsible for this?
        if (!std::filesystem::create_directories(parent)) {
            throw std::runtime_error(std::format(
                "could not create working directory '{}'", parent.string()));
        }
    }

    std::ofstream ofs;
    ofs.open(file);

    if (!ofs.good()) {
        throw std::runtime_error(
            std::format("could not touch book-keeping file '{}'", file));
    }
}

}  // namespace

void request_manager::set_session_file(const std::string& session_file)
{
    m_session_cookie = get_session_cookie(session_file);
}

auto request_manager::read_or_download(https_request& request) -> std::string
{
    const auto home = get_home_path();
    if (home.empty()) {
        throw std::runtime_error("could not get home path");
    }

    const auto& file = std::format(
        "{}/{}/{}.html", home, m_download_prefix, request.get_page());
    if (std::filesystem::exists(file)) {
        // FIXME: If the user pipes the output to a file, this string will show
        // up.
        std::println("File '{}' found on system", file);

        const html_content html{read_file_into_string(file),
                                request.get_begin_tag(),
                                request.get_end_tag()};
        return html_formatter{html}();
    }

    std::println("File '{}' not found on system, downloading...", file);
    const auto content = do_request(request);

    create_if_does_not_exist(std::filesystem::path{file}.parent_path());

    std::println("Downloaded '{}'", file);
    std::ofstream ofs{file};
    ofs << content.plain() << "\n";

    return html_formatter{content}();
}

auto request_manager::do_request(https_request& request) const -> html_content
{
    // TODO: This probably shouldn't be set by this.
    request.set_cookie(m_session_cookie);

    if (const auto wait_time = get_time_to_wait();
        wait_time.has_value() && wait_time.value() > 0.0)
    {
        std::println(
            "In an effort to prevent overloading AoC servers, waiting to "
            "perform HTTPS " "request ({} seconds)",
            wait_time.value());
        std::this_thread::sleep_for(
            std::chrono::seconds(static_cast<std::int64_t>(wait_time.value())));
    }

    touch_file(m_bookkeeping_file);

    return request();
}

auto request_manager::get_time_to_wait() const -> std::optional<double>
{
#ifdef WIN32
#    define stat _stat
#endif

    if (!std::filesystem::exists(m_bookkeeping_file)) {
        return std::nullopt;
    }

    struct stat file_stat{};
    if (const auto res = stat(m_bookkeeping_file.c_str(), &file_stat); res != 0)
    {
        throw std::runtime_error(std::format(
            "could not stat file '{}': {}", m_bookkeeping_file, res));
    }

    // https://stackoverflow.com/a/58153628
    const std::time_t time = std::time(nullptr);
    std::tm p_t_info{};
#ifdef WIN32
    if (localtime_s(&pTInfo, &t) != 0)
#else
    if (localtime_r(&time, &p_t_info) == nullptr)
#endif
    {
        throw std::runtime_error("failed to get system time");
    }

    return m_wait_time - difftime(time, file_stat.st_mtime);
}
