#include "Throttler.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
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
    double GetLastGetRequestTime(const std::string& file)
    {
        if(std::filesystem::exists(file))
        {
            struct stat result;
            if(stat(file.c_str(), &result) == 0)
            {
                auto mod_time = result.st_mtime;

                std::time_t t = std::time(nullptr);
                std::localtime(&t);

                return difftime(t, mod_time);
            }
        }

        return {};
    }

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

Throttler::Throttler(HttpsRequest* request, double time, const std::string& file)
    : mRequest(request), mTime(time), mFile(file)
{
}

std::optional<HtmlContent> Throttler::handleRequest()
{
    const auto delta = mTime - GetLastGetRequestTime(mFile);
    if(delta > 0.0)
    {
        std::cout
            << "In an effort to prevent overloading AoC servers, waiting to perform HTTPS request ("
            << delta << " seconds)\n";
        std::this_thread::sleep_for(std::chrono::seconds(static_cast<long>(delta)));
    }

    TouchFile(mFile);

    return (*mRequest)();
}
