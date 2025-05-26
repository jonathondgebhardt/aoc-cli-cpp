#include <array>
#include <format>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>

#include "aoc-cli/Client.hpp"

#include "aoc-cli/HtmlFormatter.hpp"
#include "aoc-cli/RequestManager.hpp"

void client::calendar() const
{
    https_request request;
    request.set_base_url(m_base_url);
    request.set_page(m_year);
    request.set_content_type("text/html");
    request.set_begin_and_end_tags(R"(<pre class="calendar">)", "</pre>");

    const auto content = request_manager::instance().do_request(request);
    const auto& html = content.extracted();

    std::array<std::string, 25> day_status;
    std::regex days{"class=\"calendar-day(\\d{1,2})(\\s?.*?)\">"};
    for (auto i = std::sregex_iterator(html.begin(), html.end(), days),
              end = std::sregex_iterator();
         i != end;
         ++i)
    {
        const auto& match = *i;
        if (match.size() < 2) {
            throw std::runtime_error(
                std::format("unexpected regex result: {}", match.str()));
        }

        const auto day_number =
            static_cast<std::size_t>(std::stoi(match[1].str())) - 1;
        if (day_number > 25) {
            throw std::runtime_error(
                std::format("unexpected day number: {}", day_number));
        }

        const auto stars = [&]() -> std::string
        {
            if (match[2].str() == " calendar-verycomplete") {
                return "**";
            }

            if (match[2].str() == " calendar-complete") {
                return "*";
            }

            return {};
        }();

        day_status[day_number] = stars;
    }

    std::stringstream ss;
    for (auto day = 1; const auto& stars : day_status) {
        std::cout << std::setw(2) << day << ": " << stars << "\n";
        ++day;
    }

    // The calendar is only 6 columns wide due to the way I format it. Don't
    // impose a width because that would make it confusing.
    printer printer{ss.str()};
    printer();
}

void client::download(const download_config& config)
{
    auto read_downloads = m_read_downloads;

    if (config.m_download_input) {
        read_downloads = config.m_read_input;
        download_puzzle_input();
    }

    if (config.m_download_puzzle) {
        read_downloads = config.m_read_puzzle;
        download_puzzle_description();
    }

    if (config.m_download_sample_input) {
        read_downloads = config.m_read_sample_input;
        download_puzzle_sample_input();
    }

    read_downloads = read_downloads;
}

void client::download_puzzle_input()
{
    https_request request;
    request.set_base_url(m_base_url);
    request.set_page(std::format("{}/day/{}/input", m_year, m_day));
    request.set_content_type("text/plain");

    // Switching to doRequest from readOrDownload breaks download. Only read
    // works. I still reach out to the network, but I don't do anything with the
    // request result.
    // FIXME: download is broken.
    if (const auto content = request_manager::instance().do_request(request);
        m_read_downloads)
    {
        // Don't enforce a width on input because that changes the meaning of
        // the input.
        printer printer{html_formatter{content}()};
        printer();
    }
}

void client::download_puzzle_description()
{
    https_request request;
    request.set_base_url(m_base_url);
    request.set_page(std::format("{}/day/{}", m_year, m_day));
    request.set_content_type("text/html");
    request.set_begin_and_end_tags(R"(<article class="day-desc">)",
                                   "</article>");

    // TODO: Implement part support

    // Switching to doRequest from readOrDownload breaks download. Only read
    // works. I still reach out to the network, but I don't do anything with the
    // request result.
    // FIXME: download is broken.
    if (const auto content = request_manager::instance().do_request(request);
        m_read_downloads)
    {
        m_printer.set_content(html_formatter{content}());
        m_printer();
    }
}

void client::download_puzzle_sample_input()
{
    https_request request;
    request.set_base_url(m_base_url);
    request.set_page(std::format("{}/day/{}", m_year, m_day));
    request.set_content_type("text/html");
    request.set_begin_and_end_tags("<pre><code>", "</code></pre>");

    // TODO: Implement part support

    // Switching to doRequest from readOrDownload breaks download. Only read
    // works. I still reach out to the network, but I don't do anything with the
    // request result.
    // FIXME: download is broken.
    if (const auto content = request_manager::instance().do_request(request);
        m_read_downloads)
    {
        // Don't enforce a width on input because that changes the meaning of
        // the input.
        printer printer{html_formatter{content}()};
        printer();
    }
}

void client::submit()
{
    https_request request;
    request.set_base_url(m_base_url);
    request.set_page(std::format("{}/day/{}/answer", m_year, m_day));
    request.set_begin_and_end_tags("<main>", R"(</main>)");

    // TODO: I should automatically handle the part to submit.
    request.set_post_content(
        std::format("level={}&answer={}", m_part, m_answer));

    // TODO: Further cleanup content:
    /*
    That's not the right answer.  If you're stuck, make sure you're using the
    full input data; there are also some general tips on the about page, or you
    can ask for hints on the subreddit.  Please wait one minute before trying
    again. [Return to Day 1]
    */
    /*
    That's the right answer!  You are one gold star closer to powering the
    weather machine. [Continue to Part Two]
    */
    /*
    You don't seem to be solving the right level.  Did you already complete it?
    [Return to Day 1]
    */
    /*
    That's the right answer!  You are one gold star closer to powering the
    weather machine.You have completed Day 1! You can [Shareon Twitter Mastodon]
    this victory or [Return to Your Advent Calendar].
    */
    const auto content = request_manager::instance().do_request(request);
    m_printer.set_content(html_formatter::format(content));
    m_printer();
}

void client::private_leaderboard()
{
    https_request request;
    request.set_base_url(m_base_url);
    request.set_page(std::format(
        "{}/leaderboard/private/view/{}", m_year, m_private_leaderboard_id));
    request.set_content_type("text/html");
    request.set_begin_and_end_tags(R"(<form method="post">)", "</form>");

    const auto content = request_manager::instance().do_request(request);
    auto html = html_formatter::format(content);

    // Not an ideal solution, but it works for now.

    // Remove header that gets formatted incorrectly.
    const std::string days_header = "12345678910111213141516171819202122232425";
    html.erase(html.find(days_header), days_header.size());

    {
        // Remove day status that gets formatted incorrectly.
        const std::string stars = "*************************  ";
        auto idx = html.find(stars);
        while (idx != std::string::npos) {
            html.erase(idx, stars.size());
            idx = html.find(stars);
        }
    }

    {
        // Remove preceding whitespace.
        const std::string whitespace = "\n      \n";
        if (const auto idx = html.find(whitespace); idx != std::string::npos) {
            html.erase(idx, whitespace.size());
        }
    }

    // Due to the above formatting, the leaderboard is only 6 columns wide, so
    // avoid formatting.
    printer printer{html};
    printer();
}

