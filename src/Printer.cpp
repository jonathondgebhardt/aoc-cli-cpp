#include "Printer.hpp"

#include <iostream>
#include <regex>

Printer::Printer(const std::string& content, size_t width) : mContent(content), mWidth(width)
{
}

void Printer::operator()() const
{
    if(mWidth == 0)
    {
        std::cout << mContent;
    }
    else
    {
        size_t begin = 0;
        while(begin < mContent.size())
        {
            auto line = mContent.substr(begin, mWidth);

            // New line characters needs to be considered: if a new line is in the line, change
            // `begin` to just after the new line and continue.
            if(const auto newLine = line.find('\n'); newLine != std::string::npos && newLine != 0)
            {
                line = mContent.substr(begin, newLine + 1);
                std::cout << line;
            }
            else
            {
                std::cout << line << "\n";
            }

            // TODO: If a line doesn't end in whitespace, I want to find the last instance of
            // whitespace in the substring and end the line there. The following code ain't it:
            // infinite loop.
            //        else if(!std::regex_match(line, std::regex{"\\s$"}))
            //        {
            //            line = line.substr(begin, line.find_last_of("\\s") - begin);
            //        }

            // Do look ahead? Text like:
            // ...
            // food, but they need a lot of magical energy to deliver presents on Christmas. Fo
            // r that,
            // ...
            // looks bad.

            begin += line.size();
        }
    }
}
