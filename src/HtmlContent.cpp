#include "HtmlContent.hpp"

#include <sstream>

std::string HtmlContent::extract()
{
    // TODO: HTML is not case sensitive, should make all tags the same case. Use libxml instead?
    // TODO: Use regex instead? I've gone down this road with iterative regex matches and had a lot
    // of trouble.
    if(!mBegin.empty() && !mEnd.empty())
    {
        std::stringstream ss;

        auto beginIndex = mContent.find(mBegin);
        while(beginIndex != std::string::npos)
        {
            beginIndex += mBegin.size();

            auto endIndex = mContent.find(mEnd, beginIndex);
            if(endIndex == std::string::npos)
            {
                break;
            }

            const auto length = endIndex - beginIndex;
            ss << mContent.substr(beginIndex, length);

            beginIndex = mContent.find(mBegin, endIndex);
        }

        mExtractedContent = ss.str();
    }
    else
    {
        mExtractedContent = mContent;
    }

    return mExtractedContent;
}
