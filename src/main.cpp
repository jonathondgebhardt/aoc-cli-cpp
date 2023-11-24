#include <cstdlib>
#include <cxxopts.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "AocHttpsRequest.hpp"
#include "HtmlContent.hpp"
#include "HtmlFormatter.hpp"
#include "HttpsRequest.hpp"
#include "Printer.hpp"
#include "Throttler.hpp"

const char* DOWNLOAD_PREFIX = ".aoc-cli";
const char* INPUT_PREFIX = "input";
const char* PUZZLE_PREFIX = "puzzle";

size_t WIDTH = 0;
double THROTTLE_TIME = 3.0;
std::string YEAR;
std::string DAY;
std::string SESSION_FILE;

std::string GetCurrentYear()
{
    // https://stackoverflow.com/a/58153628
    std::time_t t = std::time(nullptr);
    std::tm* const pTInfo = std::localtime(&t);

    auto currentYear = 1900 + pTInfo->tm_year;

    // AoC starts December 1st. If it's not December yet, use last year.
    if(pTInfo->tm_mon < 11)
    {
        --currentYear;
    }

    return {std::to_string(currentYear)};
}

// FIXME: This should be getting the last unlocked day. Not sure if I wanna make an HTTPS request
// just for that though.
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

bool CreateIfDoesNotExist(const std::filesystem::path& path)
{
    if(!std::filesystem::exists(path))
    {
        try
        {
            std::filesystem::create_directories(path);
            return true;
        }
        catch(...)
        {
            std::cerr << "Could not create directory '" << path << "'\n";
        }
    }

    return false;
}

std::string ReadOrDownload(const std::string& file, HttpsRequest* request)
{
    if(std::filesystem::exists(file))
    {
        std::cout << "File '" << file << "' found on the system\n";
        return ReadFileIntoString(file);
    }

    std::cout << "File '" << file << "' not found on the system, downloading...\n";

    Throttler t{request, THROTTLE_TIME,
                GetHomePath() + "/" + std::string(DOWNLOAD_PREFIX) + "/.lastgetrequest"};
    if(const auto content = t.handleRequest())
    {
        HtmlFormatter plain{*content};

        if(const auto home = GetHomePath(); !home.empty())
        {
            CreateIfDoesNotExist(std::filesystem::path{file}.parent_path());

            std::cout << "Downloaded '" << file << "'\n";
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

    AocGetRequest request;
    request.setPage(YEAR + "/day/" + DAY);
    request.setContentType("text/html");
    request.setSessionFilePath(SESSION_FILE);
    request.setBeginAndEndTags(R"(<article class="day-desc">)", "</article>");

    return ReadOrDownload(puzzle, &request);
}

std::string GetPuzzleInput()
{
    const auto home = GetHomePath();
    const auto input = home + "/" + DOWNLOAD_PREFIX + "/" + INPUT_PREFIX + "/" + DAY + ".txt";

    AocGetRequest request;
    request.setPage(YEAR + "/day/" + DAY + "/input");
    request.setContentType("text/plain");
    request.setSessionFilePath(SESSION_FILE);

    return ReadOrDownload(input, &request);
}

// FIXME: Make caching more intelligent.
// The sample input is contained within the puzzle description. If the description is already
// downloaded, I should be able to skip an HTTPS request. Should I keep the raw HTML somewhere on
// the filesystem? Should I care??
std::string GetPuzzleInputSample()
{
    const auto home = GetHomePath();
    const auto puzzle =
        home + "/" + DOWNLOAD_PREFIX + "/" + INPUT_PREFIX + "/" + DAY + "_sample.txt";

    AocGetRequest request;
    request.setPage(YEAR + "/day/" + DAY);
    request.setContentType("text/html");
    request.setSessionFilePath(SESSION_FILE);
    request.setBeginAndEndTags("<pre><code>", "</code></pre>");

    return ReadOrDownload(puzzle, &request);
}

// FIXME: The calendar retrieved by this function makes it look like you've solved every problem.
//  Go you!
// aoc-cli-rust handles this by looking for 'calendar-verycomplete' as two stars,
// 'calendar-complete' as one star, and anything else as no stars.
std::string GetCalendar()
{
    AocGetRequest request;
    request.setPage(YEAR);
    request.setContentType("text/html");
    request.setBeginAndEndTags("<main>", "</main>");
    request.setSessionFilePath(SESSION_FILE);

    Throttler t{&request, THROTTLE_TIME,
                GetHomePath() + "/" + std::string(DOWNLOAD_PREFIX) + "/.lastgetrequest"};
    if(const auto content = t.handleRequest())
    {
        HtmlFormatter plain{*content};
        return plain();
    }

    return {};
}

std::string GetPrivateLeaderBoard(const std::string& leaderBoardId)
{
    AocGetRequest request;
    request.setPage(YEAR + "/leaderboard/private/view/" + leaderBoardId);
    request.setContentType("text/html");
    request.setBeginAndEndTags("<article>", "</article>");
    request.setSessionFilePath(SESSION_FILE);

    // FIXME: This request yields nothing
    Throttler t{&request, THROTTLE_TIME,
                GetHomePath() + "/" + std::string(DOWNLOAD_PREFIX) + "/.lastgetrequest"};
    if(const auto content = t.handleRequest())
    {
        HtmlFormatter plain{*content};
        return plain();
    }

    return {};
}

std::string SubmitAnswer(const std::string& answer)
{
    AocPostRequest request;

    // TODO: Don't hardcode
    request.setPage("2022/day/12/answer");

    request.setContentType("application/x-www-form-urlencoded");
    request.setSessionFilePath(SESSION_FILE);

    // TODO: Use AocPostRequest::setPostContent

    // FIXME: This request yields nothing
    Throttler t{&request, THROTTLE_TIME,
                GetHomePath() + "/" + std::string(DOWNLOAD_PREFIX) + "/.lastgetrequest"};
    if(const auto content = t.handleRequest())
    {
        return (*content)();
    }

    return {};
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

        // TODO: Make sure YEAR and DAY are within sensible bounds
        // If year is not provided, assume it's this year or the previous year's AoC if it's not yet
        // December.
        YEAR = result["year"].as<std::string>();

        // If day is not provided, assume it's the last day completed by the user.
        DAY = result["day"].as<std::string>();

        if(command == "read")
        {
            // Surely there's a more elegant way to do this
            if(result["input-only"].count() > 0 && result["input-only"].as<bool>())
            {
                // Don't enforce a width on input because that changes the meaning of the input.
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
            Printer{SubmitAnswer(std::string())}();
        }
        else if(command == "private-leaderboard")
        {
            // TODO: Don't hardcode
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
