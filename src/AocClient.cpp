#include "AocClient.hpp"

#include <array>
#include <format>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>

#include "AocRequestManager.hpp"
#include "HtmlFormatter.hpp"

void AocClient::calendar()
{
    HttpsRequest request;
    request.setUrl(std::format("{}/{}", mBaseUrl, mYear));
    request.setContentType("text/html");
    request.setBeginAndEndTags(R"(<pre class="calendar">)", "</pre>");

    const auto content = AocRequestManager::Instance().doRequest(&request);
    const auto html = content();

    std::array<std::string, 25> dayStatus;
    std::regex days{"class=\"calendar-day(\\d{1,2})(\\s?.*?)\">"};
    for(auto i = std::sregex_iterator(html.begin(), html.end(), days), end = std::sregex_iterator();
        i != end; ++i)
    {
        const auto& match = *i;
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
        std::cout << std::setw(2) << day << ": " << stars << "\n";
        ++day;
    }

    // The calendar is only 6 columns wide due to the way I format it. Don't impose a width because
    // that would make it confusing.
    Printer p{ss.str()};
    p();
}

void AocClient::download(const DownloadConfig& config)
{
    const auto readDownloads = mReadDownloads;

    if(config.mDownloadInput)
    {
        mReadDownloads = config.mReadInput;
        downloadPuzzleInput();
    }

    if(config.mDownloadPuzzle)
    {
        mReadDownloads = config.mReadPuzzle;
        downloadPuzzleDescription();
    }

    if(config.mDownloadSampleInput)
    {
        mReadDownloads = config.mReadSampleInput;
        downloadPuzzleSampleInput();
    }

    mReadDownloads = readDownloads;
}

void AocClient::downloadPuzzleInput()
{
    HttpsRequest request;
    const auto page = std::format("{}/day/{}/input", mYear, mDay);
    request.setUrl(std::format("{}/{}", mBaseUrl, page));
    request.setContentType("text/plain");

    // TODO: This should be encapsulated by the request manager.
    // const auto input =
    //     std::format("{}/{}/{}/{}.txt", GetHomePath(), DOWNLOAD_PREFIX, INPUT_PREFIX, DAY);

    // TODO: Check for the following string
    // Puzzle inputs differ by user.  Please log in to get your puzzle input.

    if(const auto content = AocRequestManager::Instance().readOrDownload(page, &request);
       mReadDownloads)
    {
        // Don't enforce a width on input because that changes the meaning of the input.
        Printer p{content};
        p();
    }
}

void AocClient::downloadPuzzleDescription()
{
    HttpsRequest request;
    const auto page = std::format("{}/day/{}", mYear, mDay);
    request.setUrl(std::format("{}/{}", mBaseUrl, page));
    request.setContentType("text/html");
    request.setBeginAndEndTags(R"(<article class="day-desc">)", "</article>");

    // const auto puzzle =
    //     std::format("{}/{}/{}/{}.txt", GetHomePath(), DOWNLOAD_PREFIX, PUZZLE_PREFIX, DAY);

    // TODO: Implement part support

    if(const auto content = AocRequestManager::Instance().readOrDownload(page, &request);
       mReadDownloads)
    {
        mPrinter.setContent(content);
        mPrinter();
    }
}

void AocClient::downloadPuzzleSampleInput()
{
    // FIXME: Make caching more intelligent.
    // The sample input is contained within the puzzle description. If the description is already
    // downloaded, I should be able to skip an HTTPS request. Should I keep the raw HTML somewhere
    // on the filesystem? Should I care??
    HttpsRequest request;
    const auto page = std::format("{}/day/{}", mYear, mDay);
    request.setUrl(std::format("{}/{}", mBaseUrl, page));
    request.setContentType("text/html");
    request.setBeginAndEndTags("<pre><code>", "</code></pre>");

    // TODO: Implement part support

    // const auto puzzle =
    //     std::format("{}/{}/{}/{}_sample.txt", GetHomePath(), DOWNLOAD_PREFIX, INPUT_PREFIX, DAY);
    if(const auto content = AocRequestManager::Instance().readOrDownload(page, &request);
       mReadDownloads)
    {
        // Don't enforce a width on input because that changes the meaning of the input.
        Printer p{content};
        p();
    }
}

void AocClient::read() {}

void AocClient::submit()
{
    HttpsRequest request;
    request.setUrl(std::format("{}/{}/day/{}/answer", mBaseUrl, mYear, mDay));
    request.setBeginAndEndTags("<main>", R"(</main>)");
    request.setPostContent(std::format("level={}&answer={}", mPart, mAnswer));

    const auto content = AocRequestManager::Instance().doRequest(&request);
    mPrinter.setContent(HtmlFormatter::Format(content));
    mPrinter();
}

void AocClient::privateLeaderboard()
{
    HttpsRequest request;
    request.setUrl(
        std::format("{}/{}/leaderboard/private/view/{}", mBaseUrl, mYear, mPrivateLeaderboardId));
    request.setContentType("text/html");
    request.setBeginAndEndTags(R"(<form method="post">)", "</form>");

    const auto content = AocRequestManager::Instance().doRequest(&request);
    auto html = HtmlFormatter::Format(content);

    // Not an ideal solution, but it works for now.

    // Remove header that gets formatted incorrectly.
    const std::string daysHeader = "12345678910111213141516171819202122232425";
    html.erase(html.find(daysHeader), daysHeader.size());

    {
        // Remove day status that gets formatted incorrectly.
        const std::string stars = "*************************  ";
        auto idx = html.find(stars);
        while(idx != std::string::npos)
        {
            html.erase(idx, stars.size());
            idx = html.find(stars);
        }
    }

    {
        // Remove preceding whitespace.
        const std::string whitespace = "\n      \n";
        if(const auto idx = html.find(whitespace); idx != std::string::npos)
        {
            html.erase(idx, whitespace.size());
        }
    }

    // Due to the above formatting, the leaderboard is only 6 columns wide, so avoid formatting.
    Printer p{html};
    p();
}