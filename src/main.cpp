#include <cstdlib>
#include <cxxopts.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <regex>

#include "AocHttpsRequest.hpp"
#include "AocRequestManager.hpp"
#include "HtmlContent.hpp"
#include "HtmlFormatter.hpp"
#include "HttpsRequest.hpp"
#include "Printer.hpp"
#include "System.hpp"

namespace
{
    AocRequestManager REQUEST_MANAGER = AocRequestManager::Instance();

    const char* DOWNLOAD_PREFIX = ".aoc-cli";
    const char* INPUT_PREFIX = "input";
    const char* PUZZLE_PREFIX = "puzzle";

    size_t WIDTH = 0;
    std::string YEAR; // TODO: Allow this to be defined with a preprocessor directive?
    std::string DAY;
    std::string SESSION_FILE;
}

void ValidateYear()
{
    if(const auto year = std::stoi(YEAR); year < 2015 || year > std::stoi(GetCurrentYear()))
    {
        throw std::runtime_error(std::format("invalid year: {}", year));
    }
}

void ValidateDay()
{
    if(const auto day = std::stoi(DAY); day < 0 || day > 25)
    {
        throw std::runtime_error(std::format("invalid day: {}", day));
    }
}

std::string GetPuzzleDescription()
{
    AocGetRequest request;
    request.setPage(std::format("{}/day/{}", YEAR, DAY));
    request.setContentType("text/html");
    request.setBeginAndEndTags(R"(<article class="day-desc">)", "</article>");

    const auto puzzle =
        std::format("{}/{}/{}/{}.txt", GetHomePath(), DOWNLOAD_PREFIX, PUZZLE_PREFIX, DAY);
    return REQUEST_MANAGER.readOrDownload(puzzle, &request);
}

std::string GetPuzzleInput()
{
    AocGetRequest request;
    request.setPage(YEAR + "/day/" + DAY + "/input");
    request.setContentType("text/plain");

    const auto input =
        std::format("{}/{}/{}/{}.txt", GetHomePath(), DOWNLOAD_PREFIX, INPUT_PREFIX, DAY);
    return REQUEST_MANAGER.readOrDownload(input, &request);
}

// FIXME: Make caching more intelligent.
// The sample input is contained within the puzzle description. If the description is already
// downloaded, I should be able to skip an HTTPS request. Should I keep the raw HTML somewhere on
// the filesystem? Should I care??
std::string GetPuzzleInputSample()
{
    AocGetRequest request;
    request.setPage(YEAR + "/day/" + DAY);
    request.setContentType("text/html");
    request.setBeginAndEndTags("<pre><code>", "</code></pre>");

    const auto puzzle =
        std::format("{}/{}/{}/{}_sample.txt", GetHomePath(), DOWNLOAD_PREFIX, INPUT_PREFIX, DAY);
    return REQUEST_MANAGER.readOrDownload(puzzle, &request);
}

// FIXME: The calendar retrieved by this function makes it look like you've solved every problem.
//  Go you!
//
// aoc-cli-rust handles this by looking for 'calendar-verycomplete' as two stars,
// 'calendar-complete' as one star, and anything else as no stars.
//
// clang-format off
// <a aria-label="Day 11, one star" href="/2022/day/11" class="calendar-day11 calendar-complete"> ... 
// <a aria-label="Day 10, two stars" href="/2022/day/10" class="calendar-day10 calendar-verycomplete"> ...
// clang-format on
//
// I'm using libxml so I lose this information entirely.
std::string GetCalendar()
{
    AocGetRequest request;
    request.setPage(YEAR);
    request.setContentType("text/html");
    request.setBeginAndEndTags(R"(<pre class="calendar">)", "</pre>");

    const auto content = REQUEST_MANAGER.doRequest(&request);
    const auto html = content();

    std::array<std::string, 25> dayStatus;
    std::regex days{"class=\"calendar-day(\\d{1,2})(\\s?.*?)\">"};
    for(auto i = std::sregex_iterator(html.begin(), html.end(), days), end = std::sregex_iterator();
        i != end; ++i)
    {
        const auto match = *i;
        if(match.size() < 2)
        {
            throw std::runtime_error(std::format("unexpected regex result: {}", match.str()));
        }

        const auto dayNumber = static_cast<std::size_t>(std::stoi(match[1].str())) - 1;
        if(dayNumber > 25)
        {
            throw std::runtime_error(std::format("unexpected day number: {}", dayNumber));
        }

        const auto stars = [&]() -> std::string
        {
            if(match[2].str() == " calendar-verycomplete")
            {
                return "**";
            }
            else if(match[2].str() == " calendar-complete")
            {
                return "*";
            }

            return {};
        }();

        dayStatus[dayNumber] = stars;
    }

    std::stringstream ss;
    for(auto day = 1; const auto& stars : dayStatus)
    {
        ss << std::setw(2) << day << ": " << stars << "\n";
        ++day;
    }

    return ss.str();
}

std::string GetPrivateLeaderBoard(const std::string& leaderBoardId)
{
    AocGetRequest request;
    request.setPage(std::format("{}/leaderboard/private/view/{}", YEAR, leaderBoardId));
    request.setContentType("text/html");
    request.setBeginAndEndTags(R"(<form method="post">)", "</form>");

    const auto content = REQUEST_MANAGER.doRequest(&request);
    auto html = HtmlFormatter::Format(content);

    // Not an ideal solution, but it works for now.

    // Remove header that gets formatted incorrectly.
    const std::string daysHeader = "12345678910111213141516171819202122232425";
    html.erase(html.find(daysHeader), daysHeader.size());

    // Remove day status that gets formatted incorrectly.
    const std::string stars = "*************************  ";
    auto idx = html.find(stars);
    while(idx != std::string::npos)
    {
        html.erase(idx, stars.size());
        idx = html.find(stars);
    }

    // Remove preceding whitespace.
    const std::string whitespace = "\n      \n";
    if(const auto idx = html.find(whitespace); idx != std::string::npos)
    {
        html.erase(idx, whitespace.size());
    }

    return html;
}

std::string SubmitAnswer(const std::string& answer)
{
    AocPostRequest request;

    // TODO: Don't hardcode
    request.setPage(std::format("{}/day/{}/answer", YEAR, DAY));
    request.setBeginAndEndTags("<main>", R"(</main>)");
    request.setPostContent(std::format("level={}&answer={}", 1, 42));

    const auto content = REQUEST_MANAGER.doRequest(&request);
    return HtmlFormatter::Format(content);
}

int main(int argc, char** argv)
{
    try
    {
        // TODO: Add a brief description
        cxxopts::Options options{"aoc-cli", "A command line utility for Advent of Code"};

        // TODO: Don't blatantly copy+paste from rust aoc-cli :(
        // clang-format off
        options.add_options()("command", std::string(), cxxopts::value<std::string>()->default_value("read"));
        options.parse_positional({"command"});

        options.add_options()
            ("y,year", "Puzzle year", cxxopts::value<std::string>()->default_value(GetCurrentYear()))
            ("d,day", "Puzzle day", cxxopts::value<std::string>()->default_value(GetCurrentDay()))
            ("s,session-file", "Path to session cookie file", cxxopts::value<std::string>()->default_value(GetHomePath() + "/.adventofcode.session"))
            ("i,input-only", "Download puzzle input only", cxxopts::value<bool>()->default_value("false"))
            ("sample-only", "Download puzzle input sample only", cxxopts::value<bool>()->default_value("false"))
            ("p,puzzle-only", "Download puzzle description only", cxxopts::value<bool>()->default_value("false"))
            ("w,width", "Width at which to wrap output", cxxopts::value<size_t>()->default_value("0"))
            ("h,help", "Print help information", cxxopts::value<bool>()->default_value("false"))
            ("v,version", "Print version information", cxxopts::value<std::string>())
            ;

        options.custom_help("[COMMANDS]");

        // This is a bit of a hack: take advantage of being able to write on the same "line" as the
        // first line of the help message. This string should show up before the rest of the options
        // in the help message.
        options.positional_help(
            "[OPTIONS]\n\n"
                    "Commands:\n\n"
                    "  calendar                Show Advent of Code calendar and stars collected\n"
                    "  download                Save puzzle description and input to files\n"
                    "  read                    Read puzzle statement (the default command)\n"
                    "  submit                  Submit puzzle answer\n"
                    "  private-leaderboard     Show the state of a private leaderboard\n\n"
            "Options:"
        );
        // clang-format on

        const auto result = options.parse(argc, argv);

        if(result.count("help"))
        {
            std::cout << options.help() << "\n";
            return EXIT_SUCCESS;
        }

        const auto command = result["command"].as<std::string>();

        WIDTH = result["width"].as<size_t>();

        SESSION_FILE = result["session-file"].as<std::string>();
        REQUEST_MANAGER.setSessionFile(SESSION_FILE);

        YEAR = result["year"].as<std::string>();
        ValidateYear();

        // If day is not provided, assume it's the last day completed by the user.
        DAY = result["day"].as<std::string>();
        ValidateDay();

        if(command == "read")
        {
            // Surely there's a more elegant way to do this
            if(result["input-only"].count() > 0 && result["input-only"].as<bool>())
            {
                // Don't enforce a width on input because that changes the meaning of the input.
                // TODO: Check for the following string
                // Puzzle inputs differ by user.  Please log in to get your puzzle input.
                Printer{GetPuzzleInput()}();
            }
            else if(result["sample-only"].count() > 0 && result["sample-only"].as<bool>())
            {
                Printer{GetPuzzleInputSample(), WIDTH}();
            }
            else if(result["puzzle-only"].count() > 0 && result["puzzle-only"].as<bool>())
            {
                Printer{GetPuzzleDescription(), WIDTH}();
            }
            else
            {
                // Don't enforce a width on input because that changes the meaning of the input.
                Printer{GetPuzzleInput()}();
                Printer{GetPuzzleInputSample()}();

                Printer{GetPuzzleDescription(), WIDTH}();
            }
        }
        else if(command == "download")
        {
            // Surely there's a more elegant way to do this
            if(result["input-only"].count() > 0 && result["input-only"].as<bool>())
            {
                GetPuzzleInput();
            }
            else if(result["sample-only"].count() > 0 && result["sample-only"].as<bool>())
            {
                GetPuzzleInputSample();
            }
            else if(result["puzzle-only"].count() > 0 && result["puzzle-only"].as<bool>())
            {
                GetPuzzleDescription();
            }
            else
            {
                GetPuzzleInput();
                GetPuzzleInputSample();
                GetPuzzleDescription();
            }
        }
        else if(command == "calendar")
        {
            // I don't think it makes sense to allow the user to specify the width. The calendar was
            // designed for a certain width, so deal with it.
            Printer{GetCalendar()}();
        }
        else if(command == "submit")
        {
            // TODO: Get answer from command line
            Printer{SubmitAnswer(std::string())}();
        }
        else if(command == "private-leaderboard")
        {
            // TODO: Get leader board from command line?
            Printer{GetPrivateLeaderBoard("192073")}();
        }
        else
        {
            std::cerr << "Unrecognized command '" << command << "'\n";
            std::cout << options.help() << "\n";
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }
    catch(cxxopts::exceptions::parsing& e)
    {
        std::cerr << "Error parsing arguments: " << e.what() << "\n";
    }
    catch(cxxopts::exceptions::specification& e)
    {
        std::cerr << "Error defining options: " << e.what() << "\n";
    }
    catch(cxxopts::exceptions::exception& e)
    {
        std::cerr << "Error : " << e.what() << "\n";
    }
    catch(std::exception& e)
    {
        std::cerr << "Error : " << e.what() << "\n";
    }
    catch(...)
    {
        std::cerr << "Unknown exception\n";
    }

    return EXIT_FAILURE;
}
