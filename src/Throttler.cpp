#include "Throttler.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <thread>

#include <sys/stat.h>
#include <sys/types.h>

#ifndef WIN32
#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

namespace
{
    //! Updates the time last modified on the file.
    //! \param file The file.
    void TouchFile(const std::string& file)
    {
        if(std::filesystem::exists(file))
        {
            std::filesystem::remove(file);
        }

        std::filesystem::path p{file};
        if(const auto parent = p.parent_path(); !std::filesystem::exists(parent))
        {
            // TODO: Should Throttler be responsible for this?
            if(!std::filesystem::create_directories(parent))
            {
                throw std::runtime_error(
                    std::format("could not create working directory '{}'", parent.string()));
            }
        }

        std::ofstream ofs;
        ofs.open(file);

        if(!ofs.good())
        {
            throw std::runtime_error(std::format("could not touch book-keeping file '{}'", file));
        }
    }
}

HtmlContent Throttler::handleRequest() const
{
    if(const auto waitTime = getTimeToWait(); waitTime.has_value() && waitTime.value() > 0.0)
    {
        std::println("In an effort to prevent overloading AoC servers, waiting to perform HTTPS "
                     "request ({} seconds)",
                     waitTime.value());
        std::this_thread::sleep_for(std::chrono::seconds(static_cast<long>(waitTime.value())));
    }

    TouchFile(mFile);

    return (*mRequest)();
}

std::optional<double> Throttler::getTimeToWait() const
{
    if(!std::filesystem::exists(mFile))
    {
        return std::nullopt;
    }

    struct stat fileStat;
    if(const auto res = stat(mFile.c_str(), &fileStat); res != 0)
    {
        throw std::runtime_error(std::format("could not stat file '{}': {}", mFile, res));
    }

    auto mod_time = fileStat.st_mtime;

    std::time_t t = std::time(nullptr);
    std::localtime(&t);

    return mWaitTime - difftime(t, mod_time);
}
