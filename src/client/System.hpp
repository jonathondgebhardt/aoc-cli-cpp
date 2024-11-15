#pragma once

#include <ctime>
#include <string>

#ifdef WIN32
#include <processenv.h>
#endif

static std::string GetHomePath()
{
#ifdef WIN32
    // Here lies my failed attempts at using a non-deprecated function. I admitted defeat and just
    // used the deprecated function.

    // std::string val;
    // val.reserve(MAX_PATH);

    // const auto userProfile = "USERPROFILE";
    // if(GetEnvironmentVariableA(userProfile, val.data(), static_cast<DWORD>(val.size())) == 0)
    //{
    //     const auto error = GetLastError();
    //     throw std::runtime_error(
    //         std::format("failed to get environment variable '{}': {}", userProfile, error));
    // }

    // if(val.empty())
    //{
    //     throw std::runtime_error(
    //         std::format("failed to get environment variable '{}'. Win32 API returned empty
    //         string.",
    //                     userProfile));
    // }

    // return val;

    // if(const auto res = _dupenv_s(&val, &len, userProfile); res != 0)
    //{
    //     throw std::runtime_error(
    //         std::format("failed to get environment variable '{}': {}", userProfile, res));
    // }

    // if(len == 0 || val == nullptr)
    //{
    //     throw std::runtime_error(
    //         std::format("failed to get environment variable '{}'", userProfile));
    // }

    // std::string value{val, len};
    // free(val);
    // return value;

    return std::getenv("USERPROFILE");
#else
    return std::getenv("HOME");
#endif
}

static std::tm GetSystemTime()
{
    // https://stackoverflow.com/a/58153628
    const std::time_t t = std::time(nullptr);
    std::tm pTInfo;
#ifdef WIN32
    if(localtime_s(&pTInfo, &t) != 0)
#else
    if(localtime_r(&t, &pTInfo) == nullptr)
#endif
    {
        throw std::runtime_error("failed to get system time");
    }

    return pTInfo;
}

// If year is not provided, assume it's this year or the previous year's AoC if it's not yet
// December.
static std::string GetCurrentYear()
{
    const auto time = GetSystemTime();
    auto currentYear = 1900 + time.tm_year;

    // AoC starts December 1st. If it's not December yet, use last year.
    if(time.tm_mon < 11)
    {
        --currentYear;
    }

    return std::to_string(currentYear);
}

// FIXME: This should be getting the last unlocked day. Not sure if I wanna make an HTTPS request
// just for that though.
static std::string GetCurrentDay() { return {std::to_string(GetSystemTime().tm_mday)}; }