#include "HtmlFormatter.hpp"

#include <iostream>
#include <sstream>

HtmlFormatter::HtmlFormatter(const HtmlContent& content) : mContent(content)
{
    // TODO: There's a rogue "</codE>" in here...
    mExtractors.emplace_back("<h2>", "</h2>");
    mExtractors.emplace_back("<p>", "</p>");
    mExtractors.emplace_back("<pre><code>", "</code></pre>");
    //    mExtractors.emplace_back("<code>", "</code>");
    //    mExtractors.emplace_back("<em>", "</em>");

    // FIXME: Extractors don't use regex
    mExtractors.emplace_back("<a href.*?>", "</a>");

    // TODO: Need to handle <li> and </li> specifically
    mExtractors.emplace_back("<li>", "</li>");
}

std::string HtmlFormatter::operator()()
{
    std::stringstream ss;

    auto content = mContent();

    size_t it;
    do
    {
        it = std::string::npos;

        // Update extractors
        std::for_each(mExtractors.begin(), mExtractors.end(),
                      [&content](Extractor& e) { e.extract(content); });

        // Find extractor with smallest begin position
        const auto first = std::min_element(mExtractors.begin(), mExtractors.end(),
                                            [](Extractor& lhs, Extractor& rhs)
                                            { return lhs.mBeginPos < rhs.mBeginPos; });

        // Update stream
        if(first != mExtractors.end() && first->mBeginPos != std::string::npos)
        {
            std::cout << (*first)() << "\n\n";

            // FIXME: Printer doesn't like
            //            ss << (*first)() << "\n\n";

            content = content.substr(first->mBeginPos);
            it = first->mBeginPos;
        }
    } while(it != std::string::npos); // Continue until end of content

    return ss.str();
}

size_t HtmlFormatter::Extractor::extract(const std::string& content)
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

std::string HtmlFormatter::Extractor::operator()() const
{
    return mContent.substr(mBeginPos, mLength);
}
