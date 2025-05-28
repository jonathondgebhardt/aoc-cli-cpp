#pragma once

#include <ctime>
#include <stdexcept>
#include <string>

#ifdef WIN32
#    include <processenv.h>
#endif

static auto get_home_path() -> std::string
{
#ifdef WIN32
    // Here lies my failed attempts at using a non-deprecated function. I
    // admitted defeat and just used the deprecated function.

    // std::string val;
    // val.reserve(MAX_PATH);

    // const auto userProfile = "USERPROFILE";
    // if(GetEnvironmentVariableA(userProfile, val.data(),
    // static_cast<DWORD>(val.size())) == 0)
    //{
    //     const auto error = GetLastError();
    //     throw std::runtime_error(
    //         std::format("failed to get environment variable '{}': {}",
    //         userProfile, error));
    // }

    // if(val.empty())
    //{
    //     throw std::runtime_error(
    //         std::format("failed to get environment variable '{}'. Win32 API
    //         returned empty string.",
    //                     userProfile));
    // }

    // return val;

    // if(const auto res = _dupenv_s(&val, &len, userProfile); res != 0)
    //{
    //     throw std::runtime_error(
    //         std::format("failed to get environment variable '{}': {}",
    //         userProfile, res));
    // }

    // if(len == 0 || val == nullptr)
    //{
    //     throw std::runtime_error(
    //         std::format("failed to get environment variable '{}'",
    //         userProfile));
    // }

    // std::string value{val, len};
    // free(val);
    // return value;

    return std::getenv("USERPROFILE");
#else
    return std::getenv("HOME");
#endif
}

static auto get_system_time() -> std::tm
{
    // https://stackoverflow.com/a/58153628
    const std::time_t time = std::time(nullptr);
    std::tm p_t_info{};
#ifdef WIN32
    if (localtime_s(&pTInfo, &t) != 0)
#else
    if (localtime_r(&time, &p_t_info) == nullptr)
#endif
    {
        throw std::runtime_error("failed to get system time");
    }

    return p_t_info;
}

// If year is not provided, assume it's this year or the previous year's AoC if
// it's not yet December.
static auto get_current_year() -> std::string
{
    const auto time = get_system_time();
    const auto offset = 1900;
    auto current_year = time.tm_year + offset;

    // AoC starts December 1st. If it's not December yet, use last year.
    constexpr auto november = 11;
    if (time.tm_mon < november) {
        --current_year;
    }

    return std::to_string(current_year);
}

// FIXME: This should be getting the last unlocked day. Not sure if I want to
// make an HTTPS request just for that though.
static auto get_current_day() -> std::string
{
    return {std::to_string(get_system_time().tm_mday)};
}
