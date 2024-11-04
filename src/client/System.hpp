#pragma once

#include <ctime>
#include <string>

static std::string GetHomePath()
{
#ifdef WIN32
    //  TODO: Test this
    // FIXME: Resharper complains this is deprecated.
    if(const auto homeDrive = std::getenv("HOMEDRIVE"))
    {
        // FIXME: Resharper complains this is deprecated.
        if(const auto homePath = std::getenv("HOMEPATH"))
        {
            // Surely there's a more elegant way to do this.
            return std::string(homeDrive) + "/" + std::string(homePath);
        }
    }

    return {};

#else
    return std::getenv("HOME");
#endif
}

// If year is not provided, assume it's this year or the previous year's AoC if it's not yet
// December.
static std::string GetCurrentYear()
{
    // https://stackoverflow.com/a/58153628
    const std::time_t t = std::time(nullptr);

    // FIXME: Resharper complains this is deprecated.
    const std::tm* const pTInfo = std::localtime(&t);

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
static std::string GetCurrentDay()
{
    // https://stackoverflow.com/a/58153628
    const std::time_t t = std::time(nullptr);
    const std::tm* const pTInfo = std::localtime(&t);
    return {std::to_string(pTInfo->tm_mday)};
}