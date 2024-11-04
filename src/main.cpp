#include <array>
#include <cstdlib>
#include <cxxopts.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <regex>

#include "AocClient.hpp"
#include "AocHttpsRequest.hpp"
#include "AocRequestManager.hpp"
#include "HtmlContent.hpp"
#include "HtmlFormatter.hpp"
#include "HttpsRequest.hpp"
#include "Printer.hpp"
#include "System.hpp"

namespace
{
    const char* DOWNLOAD_PREFIX = ".aoc-cli";
    const char* INPUT_PREFIX = "input";
    const char* PUZZLE_PREFIX = "puzzle";

    std::uint16_t WIDTH = 0;
    std::string YEAR; // TODO: Allow this to be defined with a preprocessor directive?
    std::string DAY;
    std::string SESSION_FILE;
}

// void ValidateYear()
// {
//     if(const auto year = std::stoi(YEAR); year < 2015 || year > std::stoi(GetCurrentYear()))
//     {
//         throw std::runtime_error(std::format("invalid year: {}", year));
//     }
// }

// void ValidateDay()
// {
//     if(const auto day = std::stoi(DAY); day < 0 || day > 25)
//     {
//         throw std::runtime_error(std::format("invalid day: {}", day));
//     }
// }

std::string GetPuzzleDescription()
{
    AocGetRequest request;
    request.setPage(std::format("{}/day/{}", YEAR, DAY));
    request.setContentType("text/html");
    request.setBeginAndEndTags(R"(<article class="day-desc">)", "</article>");

    const auto puzzle =
        std::format("{}/{}/{}/{}.txt", GetHomePath(), DOWNLOAD_PREFIX, PUZZLE_PREFIX, DAY);
    return AocRequestManager::Instance().readOrDownload(puzzle, &request);
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
    return AocRequestManager::Instance().readOrDownload(puzzle, &request);
}

int main(int argc, char** argv)
{
    try
    {
        // TODO: Add a brief description
        cxxopts::Options options{"aoc-cli", "A command line utility for Advent of Code"};

        // TODO: Don't blatantly copy+paste from rust aoc-cli :(
        // clang-format off
        options.add_options()
            ("command", std::string(), cxxopts::value<std::string>()->default_value("read"))
            ("command_option", std::string(), cxxopts::value<std::string>())
            ;
        options.parse_positional({"command", "command_option"});

        options.add_options()
            ("y,year", "Puzzle year", cxxopts::value<std::string>()->default_value(GetCurrentYear()))
        // TODO: If it's out of season, should I default the day?
            ("d,day", "Puzzle day", cxxopts::value<std::string>()->default_value(GetCurrentDay()))
        // TODO: Implement smart part detection? It probably shouldn't default to 1.
            ("p,part", "Puzzle part", cxxopts::value<std::string>()->default_value("1"))
            ("s,session-file", "Path to session cookie file", cxxopts::value<std::string>()->default_value(GetHomePath() + "/.adventofcode.session"))
            ("input-only", "Download puzzle input only", cxxopts::value<bool>()->default_value("false"))
            ("sample-only", "Download puzzle input sample only", cxxopts::value<bool>()->default_value("false"))
            ("puzzle-only", "Download puzzle description only", cxxopts::value<bool>()->default_value("false"))
            ("w,width", "Width at which to wrap output", cxxopts::value<std::uint16_t>()->default_value("0"))
            ("h,help", "Print help information", cxxopts::value<bool>()->default_value("false"))
            ("v,version", "Print version information")
            ;

        options.custom_help("[COMMANDS]");

        // This is a bit of a hack: take advantage of being able to write on the same "line" as the
        // first line of the help message. This string should show up before the rest of the options
        // in the help message.
        options.positional_help(
            "[OPTIONS]\n\n"
                    "Commands:\n\n"
                    "  calendar                               Show Advent of Code calendar and stars collected\n"
                    "  download                               Save puzzle description and input to files\n"
                    "  read                                   Read puzzle statement (the default command)\n"
                    "  submit ANSWER                          Submit puzzle answer\n"
                    "  private-leaderboard LEADERBOARD_ID     Show the state of a private leaderboard\n\n"
            "Options:"
        );
        // clang-format on

        const auto result = options.parse(argc, argv);

        if(result.count("help"))
        {
            std::cout << options.help() << "\n";
            return EXIT_SUCCESS;
        }

        if(result["version"].count())
        {
            // TODO: Come up with a better versioning scheme. Would like to include date.
            std::cout << argv[0] << " v0.1\n";
            return EXIT_SUCCESS;
        }

        const auto command = result["command"].as<std::string>();

        SESSION_FILE = result["session-file"].as<std::string>();
        AocRequestManager::Instance().setSessionFile(SESSION_FILE);

        AocClient client;
        client.setBaseUrl("https://adventofcode.com");
        client.setPrinter(Printer{result["width"].as<std::uint16_t>()});
        client.setYear(result["year"].as<std::string>());
        // ValidateYear();
        client.setDay(result["day"].as<std::string>());
        // ValidateDay();

        // TODO: Add more granular control over what is printed.
        const auto shouldRead = command == "read";
        client.setReadDownloads(shouldRead);

        if(command == "read")
        {
            // TODO: Implement part support
            // Surely there's a more elegant way to do this
            if(result["input-only"].count() && result["input-only"].as<bool>())
            {
                // Don't enforce a width on input because that changes the meaning of the input.
                // TODO: Check for the following string
                // Puzzle inputs differ by user.  Please log in to get your puzzle input.
                // Printer{GetPuzzleInput()}();
                client.downloadPuzzleInput();
            }
            else if(result["sample-only"].count() && result["sample-only"].as<bool>())
            {
                Printer{GetPuzzleInputSample(), WIDTH}();
            }
            else if(result["puzzle-only"].count() && result["puzzle-only"].as<bool>())
            {
                Printer{GetPuzzleDescription(), WIDTH}();
            }
            else
            {
                // GetPuzzleInput();
                client.downloadPuzzleInput();

                GetPuzzleInputSample();

                Printer{GetPuzzleDescription(), WIDTH}();
            }
        }
        else if(command == "download")
        {
            // TODO: Implement part support
            // Surely there's a more elegant way to do this
            if(result["input-only"].count() && result["input-only"].as<bool>())
            {
                // GetPuzzleInput();
                client.downloadPuzzleInput();
            }
            else if(result["sample-only"].count() && result["sample-only"].as<bool>())
            {
                GetPuzzleInputSample();
            }
            else if(result["puzzle-only"].count() && result["puzzle-only"].as<bool>())
            {
                GetPuzzleDescription();
            }
            else
            {
                // GetPuzzleInput();
                client.downloadPuzzleInput();
                GetPuzzleInputSample();
                GetPuzzleDescription();
            }
        }
        else if(command == "calendar")
        {
            // The calendar is only 6 columns wide due to the way I format it. Don't impose a width
            // because that would make it confusing.
            client.calendar();
        }
        else if(command == "submit")
        {
            const auto answer = result["command_option"].as<std::string>();
            const auto part = result["part"].as<std::string>();
            client.setAnswer(answer);
            client.setPart(part);
            client.submit();
        }
        else if(command == "private-leaderboard")
        {
            // TODO: Validate leaderboard id.
            const auto leaderboardId = result["command_option"].as<std::string>();
            client.setPrivateLeaderboardId(leaderboardId);
            client.privateLeaderboard();
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
