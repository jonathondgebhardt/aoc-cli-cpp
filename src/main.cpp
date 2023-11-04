#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <vector>

#include "ArgParser.hpp"
#include "HtmlContent.hpp"
#include "HttpsRequest.hpp"
#include "Printer.hpp"

// Shamelessly stolen in lieu of writing my own parser
// https://stackoverflow.com/a/868894
class InputParser
{
  public:
    InputParser(int& argc, char** argv)
    {
        for(int i = 1; i < argc; ++i)
            this->tokens.push_back(std::string(argv[i]));
    }
    /// @author iain
    const std::string& getCmdOption(const std::string& option) const
    {
        std::vector<std::string>::const_iterator itr;
        itr = std::find(this->tokens.begin(), this->tokens.end(), option);
        if(itr != this->tokens.end() && ++itr != this->tokens.end())
        {
            return *itr;
        }
        static const std::string empty_string("");
        return empty_string;
    }
    /// @author iain
    bool cmdOptionExists(const std::string& option) const
    {
        return std::find(this->tokens.begin(), this->tokens.end(), option) != this->tokens.end();
    }

  private:
    std::vector<std::string> tokens;
};

void Read(const std::string& year, const std::string& day)
{
    HttpsRequest request;
    request.setUrl("https://adventofcode.com/" + year + "/day/" + day);
    request.setContentType("text/html");
    if(const auto content = request())
    {
        // TODO: Make the title of the day prompt stand out by adding a newline
        // TODO: There's a rogue "</codE>" in here...
        auto hc = HtmlContent{*content, "<main>", "</main>"};
        hc.excludeTags("<article.*?>", "</article>", "<h2.*?>", "</h2>", "<p>", "</p>",
                       "<pre><code>", "</code></pre>", "<ul>", "</ul>", "<li>", "</li>", "<code>",
                       "</code>", "<em.*?>", "</em>", "<a href.*?>", "</a>", "<span.*?>", "</span>",
                       "<p class.*?>");

        // TODO: Format <em>content</em> in a special way to make it stand out?

        Printer(hc(), 80)();
    }
}

// Commands:
// calendar             Show Advent of Code calendar and stars collected [aliases: c]
// download             Save puzzle description and input to files [aliases: d]
// read                 Read puzzle statement (the default command) [aliases: r]
// submit               Submit puzzle answer [aliases: s]
// private-leaderboard  Show the state of a private leaderboard [aliases: p]
// help                 Print this message or the help of the given subcommand(s)
//
// Options:
//-d, --day <DAY>            Puzzle day [default: last unlocked day (during Advent of Code month)]
//-y, --year <YEAR>          Puzzle year [default: year of current or last Advent of Code event]
//-s, --session-file <PATH>  Path to session cookie file [default: ~/.adventofcode.session]
//-w, --width <WIDTH>        Width at which to wrap output [default: terminal width]
//-o, --overwrite            Overwrite files if they already exist
//-I, --input-only           Download puzzle input only
//-P, --puzzle-only          Download puzzle description only
//-i, --input-file <PATH>    Path where to save puzzle input [default: input]
//-p, --puzzle-file <PATH>   Path where to save puzzle description [default: puzzle.md]
//-q, --quiet                Restrict log messages to errors only
//--debug                Enable debug logging
//-h, --help                 Print help information
//-V, --version              Print version information

int main(int argc, char** argv)
{
    InputParser ip{argc, argv};

    if(ip.cmdOptionExists("read"))
    {
        const auto year = ip.getCmdOption("-y");
        const auto day = ip.getCmdOption("-d");

        if(!year.empty() && !day.empty())
        {
            Read(year, day);
        }
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