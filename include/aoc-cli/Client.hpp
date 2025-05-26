#pragma once

#include <cstdint>
#include <string>

#include "Printer.hpp"

// Basically all functionality from main.cpp should be here besides argument
// parsing. Additional members should be added to facilitate testing, such as
// base url.

class client
{
public:
    //! \brief Print the calendar showing puzzle status.
    //! The calendar output width will not be restricted.
    void calendar() const;

    struct download_config
    {
        bool m_read_input{false};
        bool m_read_puzzle{false};
        bool m_read_sample_input{false};

        bool m_download_input{false};
        bool m_download_puzzle{false};
        bool m_download_sample_input{false};
    };

    void download(const download_config& config);

    void download_puzzle_input();
    void download_puzzle_description();
    void download_puzzle_sample_input();

    //! \brief Submit the answer and print the response.
    //! Year, day, part, and answer must be set.
    void submit();

    //! \brief Print the private leaderboard.
    //! Year and private leaderboard id must be set. Private leaderboard output
    //! width will not be restricted.
    void private_leaderboard();

    //! \brief Set the printer to use.
    // The printer is not used for all operations because changing the format
    // changes the meaning
    // (.e.g, puzzle input).
    void set_printer(printer printer) { m_printer = std::move(printer); }

    void set_base_url(std::string base_url)
    {
        m_base_url = std::move(base_url);
    }

    void set_session_file(std::string session_file)
    {
        m_session_file = std::move(session_file);
    }

    void set_https_request_base_url(std::string base_url)
    {
        m_https_request_base_url = std::move(base_url);
    }

    void set_private_leaderboard_id(std::string leaderboard_id)
    {
        m_private_leaderboard_id = std::move(leaderboard_id);
    }

    void set_answer(std::string answer) { m_answer = std::move(answer); }

    void set_printer_width(std::uint16_t printer_width)
    {
        m_printer_width = printer_width;
    }

    void set_year(std::string year) { m_year = std::move(year); }

    void set_day(std::string day) { m_day = std::move(day); }

    void set_part(std::string part) { m_part = std::move(part); }

    void set_read_downloads(const bool read_downloads)
    {
        m_read_downloads = read_downloads;
    }

private:
    printer m_printer;
    std::string m_base_url;
    std::string m_session_file;
    std::string m_https_request_base_url;
    std::string m_private_leaderboard_id;
    std::string m_answer;
    std::uint16_t m_printer_width{};
    std::string m_year;
    std::string m_day;
    std::string m_part;  // TODO: I only need a single bit to represent this.
    bool m_read_downloads{false};
};

