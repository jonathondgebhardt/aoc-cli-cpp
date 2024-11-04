#pragma once

#include <cstdint>
#include <string>

#include "Printer.hpp"

// Basically all functionality from main.cpp should be here besides argument parsing.
// Additional members should be added to facilitate testing, such as base url.

class AocClient
{
public:
    //! \brief Print the calendar showing puzzle status.
    //! The calendar output width will not be restricted.
    void calendar();
    void download();
    void read();

    //! \brief Submit the answer and print the response.
    //! Year, day, part, and answer must be set.
    void submit();

    //! \brief Print the private leaderboard.
    //! Year and private leaderboard id must be set. Private leaderboard output width will not be
    //! restricted.
    void privateLeaderboard();

    //! \brief Set the printer to use.
    // The printer is not used for all operations because changing the format changes the meaning
    // (.e.g, puzzle input).
    void setPrinter(Printer printer) { mPrinter = std::move(printer); }

    void setBaseUrl(std::string baseUrl) { mBaseUrl = std::move(baseUrl); }
    void setSessionFile(std::string sessionFile) { mSessionFile = std::move(sessionFile); }
    void setHttpsRequestBaseUrl(std::string baseUrl) { mHttpsRequestBaseUrl = std::move(baseUrl); }
    void setPrivateLeaderboardId(std::string leaderboardId)
    {
        mPrivateLeaderboardId = std::move(leaderboardId);
    }
    void setAnswer(std::string answer) { mAnswer = std::move(answer); }
    void setPrinterWidth(std::uint16_t printerWidth) { mPrinterWidth = printerWidth; }
    void setYear(std::string year) { mYear = std::move(year); }
    void setDay(std::string day) { mDay = std::move(day); }
    void setPart(std::string part) { mPart = std::move(part); }

private:
    Printer mPrinter;
    std::string mBaseUrl;
    std::string mSessionFile;
    std::string mHttpsRequestBaseUrl;
    std::string mPrivateLeaderboardId;
    std::string mAnswer;
    std::uint16_t mPrinterWidth{};
    std::string mYear;
    std::string mDay;
    std::string mPart; // TODO: I only need a single bit to represent this.
};