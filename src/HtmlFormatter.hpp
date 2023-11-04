#pragma once

#include <vector>

#include "HtmlContent.hpp"

//! Extracts specific content from HtmlContent and formats it. Use operator() to get formatted
//! content.
class HtmlFormatter
{
public:
    explicit HtmlFormatter(const HtmlContent& content);

    std::string operator()();

private:
    //! Extracts content between begin and end tags.
    class Extractor
    {
    public:
        // TODO: Add a constructor for symmetrical tags?

        Extractor(const std::string& begin, const std::string& end) : mBegin(begin), mEnd(end)
        {
        }

        size_t extract(const std::string& content);

        std::string operator()() const;

        size_t mBeginPos = std::string::npos;

    private:

        // TODO: Add a single fire option? (i.e., extractor is only used once)

        std::string mContent;
        std::string mBegin;
        std::string mEnd;
        size_t mLength = std::string::npos;
    };

    std::vector<Extractor> mExtractors;

    HtmlContent mContent;
};
