#pragma once

#include <ctime>
#include <iostream>
#include <string>

static inline std::string GetHomePath()
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

static inline std::string GetCurrentYear()
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

    return std::to_string(currentYear);
}

// FIXME: This should be getting the last unlocked day. Not sure if I wanna make an HTTPS request
// just for that though.
static inline std::string GetCurrentDay()
{
    // https://stackoverflow.com/a/58153628
    std::time_t t = std::time(nullptr);
    std::tm* const pTInfo = std::localtime(&t);
    return {std::to_string(pTInfo->tm_mday)};
}