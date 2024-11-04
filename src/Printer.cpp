#include "Printer.hpp"

#include <iostream>

void Printer::operator()() const
{
    if(!mWidth.has_value())
    {
        std::cout << mContent;
    }
    else
    {
        const auto width = mWidth.value();
        decltype(mContent.size()) begin = 0;
        while(begin < mContent.size())
        {
            auto line = mContent.substr(begin, width);

            if(const auto newLine = line.find_last_of('\n'); newLine != std::string::npos)
            {
                // New line characters needs to be considered: if a new line is in the line, change
                // `begin` to just after the new line and continue.
                line = mContent.substr(begin, newLine + 1);
                std::cout << line;
                begin += line.size();
            }
            else if(!std::isspace(line[width - 1]))
            {
                // If a line doesn't end in whitespace, that means we're cutting off a word. Go
                // backwards.
                const auto endIndex = line.find_last_of(' ');
                line = mContent.substr(begin, endIndex);
                std::cout << line << "\n";

                // If begin is not adjusted, some lines will contain a single space at the beginning
                // of the line.
                begin += line.size() + 1;
            }
            else
            {
                std::cout << line << "\n";
                begin += line.size();
            }
        }
    }
}
