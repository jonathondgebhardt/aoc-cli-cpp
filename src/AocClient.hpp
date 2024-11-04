#pragma once

#include <cstdint>
#include <string>

#include "Printer.hpp"

// Basically all functionality from main.cpp should be here besides argument parsing.
// Additional members should be added to facilitate testing, such as base url.

class AocClient
{
public:
    void calendar();
    void download();
    void read();
    void submit();
    void privateLeaderboard();

    void setPrinter(Printer printer) { mPrinter = std::move(printer); }
    void setSessionFile(std::string sessionFile) { mSessionFile = std::move(sessionFile); }
    void setHttpsRequestBaseUrl(std::string baseUrl) { mHttpsRequestBaseUrl = std::move(baseUrl); }
    void setPrivateLeaderboardId(std::string leaderboardId)
    {
        mPrivateLeaderboardId = std::move(leaderboardId);
    }
    void setPrinterWidth(std::uint16_t printerWidth) { mPrinterWidth = printerWidth; }
    void setYear(std::uint16_t year) { mYear = year; }
    void setDay(std::uint8_t day) { mDay = day; }
    void setPart(std::uint8_t part) { mPart = part; }

private:
    Printer mPrinter;
    std::string mSessionFile;
    std::string mHttpsRequestBaseUrl;
    std::string mPrivateLeaderboardId;
    std::uint16_t mPrinterWidth{};
    std::uint16_t mYear{};
    std::uint8_t mDay{};
    std::uint8_t mPart{}; // TODO: I only need a single bit to represent this.
};