#pragma once

#include <string>

//! Prints some text to the screen. Hopefully inserts new lines intelligently so long lines look
//! nice.
class Printer
{
public:
    //! \param content The content to print.
    //! \param width The width to enforce. Zero means no width enforcement.
    Printer(const std::string& content, size_t width = 0);
    ~Printer() noexcept = default;

    //! Print the text to std::cout.
    void operator()() const;

private:
    std::string mContent;
    size_t mWidth = 0;
};
