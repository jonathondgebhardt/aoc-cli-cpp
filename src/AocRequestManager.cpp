#include "AocRequestManager.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <print>

#include <curl/curl.h>

#include "HtmlFormatter.hpp"
#include "System.hpp"
#include "Throttler.hpp"

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
}

AocRequestManager& AocRequestManager::Instance()
{
    static AocRequestManager manager;
    return manager;
}

AocRequestManager::AocRequestManager() { curl_global_init(CURL_GLOBAL_DEFAULT); }

AocRequestManager::~AocRequestManager() { curl_global_cleanup(); }

void AocRequestManager::setSessionFile(const std::string& sessionFile)
{
    mSessionCookie = GetSessionCookie(sessionFile);
}

std::string AocRequestManager::readOrDownload(const std::string& file, HttpsRequest* request)
{
    if(std::filesystem::exists(file))
    {
        std::println("File '{}' found on system", file);
        return ReadFileIntoString(file);
    }

    std::println("File '{}' not found on system, downloading...", file);

    const auto content = doRequest(request);

    HtmlFormatter plain{content};

    if(const auto home = GetHomePath(); !home.empty())
    {
        CreateIfDoesNotExist(std::filesystem::path{file}.parent_path());

        std::println("Downloaded '{}'", file);
        std::ofstream ofs{file};
        ofs << plain() << "\n";
    }

    return plain();
}

HtmlContent AocRequestManager::doRequest(HttpsRequest* request)
{
    request->setCookie(mSessionCookie);

    Throttler t{request, mWaitTime,
                std::format("{}/{}/.lastgetrequest", GetHomePath(), mDownloadPrefix)};
    return t.handleRequest();
}