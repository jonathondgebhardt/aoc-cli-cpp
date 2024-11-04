#pragma once

#include <cstdint>
#include <string>

//! \brief Prints some text to the screen. Hopefully inserts new lines intelligently so long lines
//! look nice.
class Printer
{
public:
    //! \param content The content to print.
    //! \param width The width to enforce. Zero means no width enforcement.
    explicit Printer(std::string content, const std::uint16_t width = 0)
        : mContent(std::move(content)), mWidth(width)
    {
    }

    Printer() = default;
    Printer(const Printer&) = default;
    Printer(Printer&&) = default;
    ~Printer() noexcept = default;
    Printer& operator=(const Printer&) = default;
    Printer& operator=(Printer&&) noexcept = default;

    //! \brief Print the text to std::cout.
    void operator()() const;

private:
    std::string mContent;
    std::uint16_t mWidth{0};
};
