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
    //! Gets the difference between current system time and the time the file was last modified.
    //! \param file The file to check time last modified.
    //! \return The elapsed time since the last GET request.
    double GetLastGetRequestTime(const std::string& file)
    {
        if(!std::filesystem::exists(file))
        {
            throw std::runtime_error(std::format("can't find book-keeping file '{}'", file));
        }

        struct stat fileStat;
        if(const auto res = stat(file.c_str(), &fileStat); res != 0)
        {
            throw std::runtime_error(std::format("could not stat file '{}': {}", file, res));
        }

        auto mod_time = fileStat.st_mtime;

        std::time_t t = std::time(nullptr);
        std::localtime(&t);

        return difftime(t, mod_time);
    }

    //! Updates the time last modified on the file.
    //! \param file The file.
    void TouchFile(const std::string& file)
    {
        if(std::filesystem::exists(file))
        {
            std::filesystem::remove(file);
        }

        std::ofstream ofs;
        ofs.open(file);
    }
}

HtmlContent Throttler::handleRequest()
{
    if(const auto delta = mTime - GetLastGetRequestTime(mFile); delta > 0.0)
    {
        std::println("In an effort to prevent overloading AoC servers, waiting to perform HTTPS "
                     "request ({} seconds)",
                     delta);
        std::this_thread::sleep_for(std::chrono::seconds(static_cast<long>(delta)));
    }

    TouchFile(mFile);

    return (*mRequest)();
}
