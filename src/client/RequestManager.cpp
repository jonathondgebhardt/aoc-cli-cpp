#include "RequestManager.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <print>
#include <sys/stat.h>
#include <thread>

#ifndef WIN32
#include <unistd.h>
#endif

#include <curl/curl.h>

#include "HtmlFormatter.hpp"
#include "System.hpp"

namespace
{
    std::string GetSessionCookie(const std::string& sessionFile)
    {
        std::ifstream ifs{sessionFile};

        if(!ifs.is_open())
        {
            throw std::runtime_error(std::format("could not open session file: {}", sessionFile));
        }

        std::string line;
        std::getline(ifs, line);
        return "session=" + line;
    }

    std::string ReadFileIntoString(const std::string& file)
    {
        if(std::filesystem::exists(file))
        {
            std::ifstream t(file);
            return {(std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>()};
        }

        return {};
    }

    bool CreateIfDoesNotExist(const std::filesystem::path& path)
    {
        if(!std::filesystem::exists(path))
        {
            try
            {
                std::filesystem::create_directories(path);
                return true;
            }
            catch(...)
            {
                std::cerr << "Could not create directory '" << path << "'\n";
                // std::println(std::cerr, "Could not create directory '{}'", path);
            }
        }

        return false;
    }

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
            // TODO: Should this be responsible for this?
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

void RequestManager::setSessionFile(const std::string& sessionFile)
{
    mSessionCookie = GetSessionCookie(sessionFile);
}

std::string RequestManager::readOrDownload(HttpsRequest& request)
{
    const auto home = GetHomePath();
    if(home.empty())
    {
        throw std::runtime_error("could not get home path");
    }

    const auto& file = std::format("{}/{}/{}.html", home, mDownloadPrefix, request.getPage());
    if(std::filesystem::exists(file))
    {
        std::println("File '{}' found on system", file);

        const HtmlContent html{ReadFileIntoString(file), request.getBeginTag(),
                               request.getEndTag()};
        return HtmlFormatter{html}();
    }

    std::println("File '{}' not found on system, downloading...", file);
    const auto content = doRequest(request);

    CreateIfDoesNotExist(std::filesystem::path{file}.parent_path());

    std::println("Downloaded '{}'", file);
    std::ofstream ofs{file};
    ofs << content.plain() << "\n";

    return HtmlFormatter{content}();
}

HtmlContent RequestManager::doRequest(HttpsRequest& request) const
{
    // TODO: This probably shouldn't be set by this.
    request.setCookie(mSessionCookie);

    if(const auto waitTime = getTimeToWait(); waitTime.has_value() && waitTime.value() > 0.0)
    {
        std::println("In an effort to prevent overloading AoC servers, waiting to perform HTTPS "
                     "request ({} seconds)",
                     waitTime.value());
        std::this_thread::sleep_for(std::chrono::seconds(static_cast<long>(waitTime.value())));
    }

    TouchFile(mBookkeepingFile);

    return request();
}

std::optional<double> RequestManager::getTimeToWait() const
{
#ifdef WIN32
#define stat _stat
#endif

    if(!std::filesystem::exists(mBookkeepingFile))
    {
        return std::nullopt;
    }

    struct stat fileStat;
    if(const auto res = stat(mBookkeepingFile.c_str(), &fileStat); res != 0)
    {
        throw std::runtime_error(
            std::format("could not stat file '{}': {}", mBookkeepingFile, res));
    }

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

    return mWaitTime - difftime(t, fileStat.st_mtime);
}
