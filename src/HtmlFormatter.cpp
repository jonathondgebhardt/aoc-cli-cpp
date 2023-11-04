#include "HtmlFormatter.hpp"

#include <iostream>
#include <sstream>

namespace
{
    //! Extracts content between begin and end tags.
    class Extractor
    {
    public:
        // TODO: Add a constructor for symmetrical tags?

        Extractor(std::string begin, std::string end, std::string prefix = std::string(),
                  std::string suffix = "\n")
            : mPrefix(std::move(prefix)), mSuffix(std::move(suffix)), mBegin(std::move(begin)),
              mEnd(std::move(end))
        {
        }

        size_t extract(const std::string& content)
        {
            // Reset to avoid false positives
            mBeginPos = std::string::npos;
            mLength = std::string::npos;

            mContent = content;

            // TODO: Use regex :(
            mBeginPos = content.find(mBegin);

            if(mBeginPos != std::string::npos)
            {
                mBeginPos += mBegin.size();
                const auto endPos = content.find(mEnd, mBeginPos);
                if(endPos != std::string::npos)
                {
                    mLength = endPos - mBeginPos;
                }
            }

            return mBeginPos;
        }

        [[nodiscard]] std::string operator()() const
        {
            return mPrefix + mContent.substr(mBeginPos, mLength) + mSuffix;
        }

        [[nodiscard]] size_t begin() const
        {
            return mBeginPos;
        }

        void setPrefix(const std::string& prefix)
        {
            mPrefix = prefix;
        }

        void setSuffix(const std::string& suffix)
        {
            mSuffix = suffix;
        }

    private:
        // TODO: Add a single fire option? (i.e., extractor is only used once)

        std::string mPrefix;
        std::string mSuffix = "\n";

        std::string mContent;
        std::string mBegin;
        std::string mEnd;

        size_t mBeginPos = std::string::npos;
        size_t mLength = std::string::npos;
    };

    std::vector<Extractor> GetExtractors()
    {
        std::vector<Extractor> extractors;

        // TODO: There's a rogue "</codE>" in here...
        extractors.emplace_back("<h2>", "</h2>");
        extractors.emplace_back("<p>", "</p>", std::string(), "\n\n");
        extractors.emplace_back("<pre><code>", "</code></pre>");
        //    extractors.emplace_back("<code>", "</code>");
        //    extractors.emplace_back("<em>", "</em>");

        // FIXME: Extractors don't use regex
        extractors.emplace_back("<a href.*?>", "</a>");

        // TODO: Need to handle <li> and </li> specifically
        extractors.emplace_back("<li>", "</li>", "- ");

        return extractors;
    }
}

HtmlFormatter::HtmlFormatter(const HtmlContent& content) : mContent(content)
{
}

std::string HtmlFormatter::operator()()
{
    std::stringstream ss;

    auto content = mContent();
    auto extractors = GetExtractors();

    size_t it;
    do
    {
        it = std::string::npos;

        // Update extractors
        std::for_each(extractors.begin(), extractors.end(),
                      [&content](Extractor& e) { e.extract(content); });

        // Find extractor with smallest begin position
        const auto first = std::min_element(extractors.begin(), extractors.end(),
                                            [](Extractor& lhs, Extractor& rhs)
                                            { return lhs.begin() < rhs.begin(); });

        // Update stream
        if(first != extractors.end() && first->begin() != std::string::npos)
        {
            std::cout << (*first)();

            // FIXME: Printer doesn't like
            //            ss << (*first)();

            content = content.substr(first->begin());
            it = first->begin();
        }
    } while(it != std::string::npos); // Continue until end of content

    return ss.str();
}
