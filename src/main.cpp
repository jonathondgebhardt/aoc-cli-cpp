#include <cstdlib>
#include <cxxopts.hpp>
#include <iostream>

#include "HtmlContent.hpp"
#include "HtmlFormatter.hpp"
#include "HttpsRequest.hpp"
#include "Printer.hpp"

size_t WIDTH = 120;

cxxopts::ParseResult GetArgs(int argc, char** argv)
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
    options.add_options()
        ("h,help", "Print help information", cxxopts::value<bool>()->default_value("false"))
        ("c,calendar", "Show Advent of Code calendar and stars collected", cxxopts::value<bool>()->default_value("false"))
        ("download", "Save puzzle description and input to files", cxxopts::value<bool>()->default_value("false"))
        ("r,read", "Read puzzle statement (the default command)", cxxopts::value<bool>()->default_value("true"))
        ("s,submit", "Submit puzzle answer", cxxopts::value<bool>()->default_value("false"))
        ("p,private-leaderboard", "Show the state of a private leaderboard", cxxopts::value<bool>()->default_value("false"))
        ;

    options.add_options()
        ("y,year", "Puzzle year [default: year of current or last Advent of Code event]", cxxopts::value<std::string>())
        ("session-file", "Path to session cookie file [default: ~/.adventofcode.session]", cxxopts::value<std::string>())
        ("w,width", "Width at which to wrap output [default: terminal width]", cxxopts::value<std::string>())
        ("o,overwrite", "Overwrite files if they already exist", cxxopts::value<std::string>())
        ("d,day", "Puzzle day [default: last unlocked day (during Advent of Code month)]", cxxopts::value<std::string>())
        ("v,version", "Print version information", cxxopts::value<std::string>())
        ;
    // clang-format on

    const auto result = options.parse(argc, argv);

    options.show_positional_help();

    return result;
}

void Read(const std::string& year, const std::string& day)
{
    HttpsRequest request;
    request.setUrl("https://adventofcode.com/" + year + "/day/" + day);
    request.setContentType("text/html");
    if(const auto content = request())
    {
        HtmlContent hc{*content, "<article class=\"day-desc\">", "</article>"};
        Printer(HtmlFormatter(hc)(), 80)();
    }
}

int main(int argc, char** argv)
{
    try
    {
        const auto result = GetArgs(argc, argv);

        const auto year = result["year"].as<std::string>();
        const auto day = result["day"].as<std::string>();

        if(result["read"].as<bool>())
        {
            if(!year.empty() && !day.empty())
            {
                Read(year, day);
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