#include <cstdlib>
#include <cxxopts.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "HtmlContent.hpp"
#include "HtmlFormatter.hpp"
#include "HttpsRequest.hpp"
#include "Printer.hpp"

const char* DOWNLOAD_PREFIX = ".aoc-cli";
const char* INPUT_PREFIX = "input";
const char* PUZZLE_PREFIX = "puzzle";

size_t WIDTH = 0;
std::string YEAR;
std::string DAY;
std::string SESSION_FILE;

std::string GetCurrentYear()
{
    // https://stackoverflow.com/a/58153628
    std::time_t t = std::time(nullptr);
    std::tm* const pTInfo = std::localtime(&t);
    return {std::to_string(1900 + pTInfo->tm_year)};
}

std::string GetCurrentDay()
{
    // https://stackoverflow.com/a/58153628
    std::time_t t = std::time(nullptr);
    std::tm* const pTInfo = std::localtime(&t);
    return {std::to_string(pTInfo->tm_mday)};
}

std::string GetHomePath()
{
#ifdef WIN32
    //  TODO: Test this
    if(const auto homeDrive = std::getenv("HOMEDRIVE"))
    {
        if(const auto homePath = std::getenv("HOMEPATH"))
        {
            // Surely there's a more elegant way to do this.
            return std::string(homeDrive) + "/" + std::string(homePath);
        }
    }
#else
    return std::getenv("HOME");
#endif
}

std::string ReadFileIntoString(const std::string& file)
{
    if(std::filesystem::exists(file))
    {
        std::ifstream t(file);
        return {(std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>()};
    }

    return {};
}

std::string ReadOrDownload(const std::string& file, HttpsRequest& request)
{
    if(std::filesystem::exists(file))
    {
        return ReadFileIntoString(file);
    }

    if(const auto content = request())
    {
        HtmlFormatter plain{*content};

        if(const auto home = GetHomePath(); !home.empty())
        {
            // FIXME: If the directories don't exist, this doesn't write out
            std::ofstream ofs{file};
            ofs << plain() << "\n";
        }

        return plain();
    }

    return {};
}

std::string GetPuzzleDescription()
{
    const auto home = GetHomePath();
    const auto puzzle = home + "/" + DOWNLOAD_PREFIX + "/" + PUZZLE_PREFIX + "/" + DAY + ".txt";

    HttpsRequest request;
    request.setUrl("https://adventofcode.com/" + YEAR + "/day/" + DAY);
    request.setContentType("text/html");
    request.setSessionFilePath(SESSION_FILE);
    request.setBeginAndEndTags(R"(<article class="day-desc">)", "</article>");

    return ReadOrDownload(puzzle, request);
}

std::string GetPuzzleInput()
{
    const auto home = GetHomePath();
    const auto input = home + "/" + DOWNLOAD_PREFIX + "/" + INPUT_PREFIX + "/" + DAY + ".txt";

    HttpsRequest request;
    request.setUrl("https://adventofcode.com/" + YEAR + "/day/" + DAY + "/input");
    request.setContentType("text/plain");
    request.setSessionFilePath(SESSION_FILE);

    return ReadOrDownload(input, request);
}

int main(int argc, char** argv)
{
    try
    {
        // clang-format off
        // Advent of Code command-line tool
        //
        // Usage: aoc [OPTIONS] [COMMAND]
        //
        // Commands:
        // calendar             Show Advent of Code calendar and stars collected [aliases: c]
        // download             Save puzzle description and input to files [aliases: d]
        // read                 Read puzzle statement (the default command) [aliases: r]
        // submit               Submit puzzle answer [aliases: s]
        // private-leaderboard  Show the state of a private leaderboard [aliases: p]
        // help                 Print this message or the help of the given subcommand(s)
        //
        // Options:
        // -d, --day <DAY>            Puzzle day [default: last unlocked day (during Advent of Code month)]
        // -y, --year <YEAR>          Puzzle year [default: year of current or last Advent of Code event]
        // -s, --session-file <PATH>  Path to session cookie file [default: ~/.adventofcode.session]
        // -w, --width <WIDTH>        Width at which to wrap output [default: terminal width]
        // -o, --overwrite            Overwrite files if they already exist
        // -I, --input-only           Download puzzle input only
        // -P, --puzzle-only          Download puzzle description only
        // -i, --input-file <PATH>    Path where to save puzzle input [default: input]
        // -p, --puzzle-file <PATH>   Path where to save puzzle description [default: puzzle.md]
        // -q, --quiet                Restrict log messages to errors only
        //     --debug                Enable debug logging
        // -h, --help                 Print help information
        // -V, --version              Print version information
        // clang-format on

        // TODO: Add a brief description
        cxxopts::Options options{"aoc-cli", ""};

        // TODO: Don't blatantly copy+paste from rust aoc-cli :(
        // clang-format off
        // We want these "commands" to be mutually exclusive. Use argparse instead?
        // argparse v3.0 has mutual exclusion. Our version of vcpkg has up to v2.9.
        options.add_options()
            ("h,help", "Print help information", cxxopts::value<bool>()->default_value("false"))
            ("c,calendar", "Show Advent of Code calendar and stars collected", cxxopts::value<bool>()->default_value("false"))

            // Download and read are basically the same thing: in order to read, a download has to happen first. Maybe read actually shows it to the console and download just downloads.
            ("download", "Save puzzle description and input to files", cxxopts::value<bool>()->default_value("false"))
            ("r,read", "Read puzzle statement (the default command)", cxxopts::value<bool>()->default_value("true"))

            ("s,submit", "Submit puzzle answer", cxxopts::value<bool>()->default_value("false"))
            ("p,private-leaderboard", "Show the state of a private leaderboard", cxxopts::value<bool>()->default_value("false"))
            ;

        options.add_options()
            ("y,year", "Puzzle year [default: year of current or last Advent of Code event]", cxxopts::value<std::string>()->default_value(GetCurrentYear()))
            ("d,day", "Puzzle day [default: last unlocked day (during Advent of Code month)]", cxxopts::value<std::string>()->default_value(GetCurrentDay()))
            ("session-file", "Path to session cookie file [default: ~/.adventofcode.session]", cxxopts::value<std::string>()->default_value(GetHomePath() + "/.adventofcode.session"))
            ("I,input-only", "Download puzzle input only", cxxopts::value<bool>())
            ("P,puzzle-only", "Download puzzle description only", cxxopts::value<bool>())
            ("w,width", "Width at which to wrap output [default: terminal width]", cxxopts::value<size_t>()->default_value("0"))
            ("o,overwrite", "Overwrite files if they already exist", cxxopts::value<bool>())
            ("v,version", "Print version information", cxxopts::value<std::string>())
            ;
        // clang-format on

        const auto result = options.parse(argc, argv);

        WIDTH = result["width"].as<size_t>();

        SESSION_FILE = result["session-file"].as<std::string>();

        // If year is not provided, assume it's this year.
        YEAR = result["year"].as<std::string>();

        // If day is not provided, assume it's the last day completed by the user.
        DAY = result["day"].as<std::string>();

        if(result["read"].as<bool>())
        {
            if(!YEAR.empty() && !DAY.empty())
            {
                if(result["I"].count() > 0 && result["I"].as<bool>())
                {
                    Printer(GetPuzzleInput(), WIDTH)();
                }

                if(result["P"].count() > 0 && result["P"].as<bool>())
                {
                    Printer(GetPuzzleDescription(), WIDTH)();
                }
            }
        }
    }
    catch(cxxopts::exceptions::parsing& e)
    {
        std::cerr << "Error parsing arguments: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch(cxxopts::exceptions::specification& e)
    {
        std::cerr << "Error defining options: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch(cxxopts::exceptions::exception& e)
    {
        std::cerr << "Error : " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch(std::exception& e)
    {
        std::cerr << "Error : " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch(...)
    {
        std::cerr << "Unknown exception\n";
        return EXIT_FAILURE;
    }

    //    {
    //        std::cout << "Input:\n";
    //
    //        HttpsRequest request;
    //        request.setUrl("https://adventofcode.com/2022/day/1/input");
    //        request.setContentType("text/plain");
    //        if(const auto content = request())
    //        {
    //            std::cout << *content << "\n";
    //        }
    //
    //        std::cout << "\n";
    //    }

    //    {
    // Beginning of sample input starts with "<pre><code>" and ends with "</code></pre>"
    // Ex:
    // <pre><code>A Y
    // B X
    // C Z
    // </code></pre>
    //    const std::string startTags = "<pre><code>";
    //    const auto beginPos = (*content).find(startTags) + startTags.size();
    //    const auto endTags = "</code></pre>";
    //    const auto size = (*content).find(endTags) - beginPos;
    //
    //        std::cout << "Sample input:\n";
    //
    //        HttpsRequest request;
    //        request.setUrl("https://adventofcode.com/2022/day/1");
    //        request.setContentType("text/html");
    //        if(const auto content = request())
    //        {
    //            std::cout << *content << "\n";
    //        }
    //
    //        std::cout << "\n";
    //    }

    //    {
    //        // clang-format off
    //        // <a aria-label="Day 1, two stars" href="/2022/day/1" class="calendar-day1
    //        calendar-verycomplete"><span class="calendar-color-u">  ~    ~  ~      ~     ~ ~ ~
    //        ~  ~  ~   ~   </span>  <span class="calendar-day"> 1</span> <span
    //        class="calendar-mark-complete">*</span><span
    //        class="calendar-mark-verycomplete">*</span></a>
    //        // clang-format on
    //        std::cout << "Calendar:\n";
    //
    //        HttpsRequest request;
    //        request.setUrl("https://adventofcode.com/2022");
    //        request.setContentType("text/plain");
    //        if(const auto content = request())
    //        {
    //            std::cout << *content << "\n";
    //        }
    //
    //        std::cout << "\n";
    //    }

    return EXIT_SUCCESS;
}