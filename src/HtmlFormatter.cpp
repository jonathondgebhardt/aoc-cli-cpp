#include "HtmlFormatter.hpp"

#include <iostream>
#include <sstream>

HtmlFormatter::HtmlFormatter(const HtmlContent& content) : mContent(content)
{
    mExtractors.emplace_back("<h2>", "</h2>");
    mExtractors.emplace_back("<p>", "</p>");
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
            //            ss << (*first)() << "\n\n";
            content = content.substr(first->mBeginPos);
            it = first->mBeginPos;
        }
    } while(it != std::string::npos); // Continue until end of content

    return ss.str();
}

size_t HtmlFormatter::Extractor::extract(const std::string& content)
{
    mContent = content;
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
