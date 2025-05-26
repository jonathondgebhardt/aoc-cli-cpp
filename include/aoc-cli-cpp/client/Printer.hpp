#pragma once

#include <cstdint>
#include <optional>
#include <string>

//! \brief Prints some text to the screen.
//! If a width is set, hopefully inserts new lines intelligently so long lines look nice.
class Printer
{
public:
    //! \param width The width to enforce.
    explicit Printer(const std::optional<std::uint16_t> width) : Printer(std::string(), width) {}

    //! \param content The content to print.
    //! \param width The width to enforce.
    explicit Printer(std::string content, const std::optional<std::uint16_t> width = std::nullopt)
        : mContent(std::move(content)), mWidth(width)
    {
    }

    Printer() = default;
    Printer(const Printer&) = default;
    Printer(Printer&&) = default;
    ~Printer() noexcept = default;
    Printer& operator=(const Printer&) = default;
    Printer& operator=(Printer&&) noexcept = default;

    void setContent(std::string content) { mContent = std::move(content); }

    //! \brief Print the text to std::cout.
    void operator()() const;

private:
    std::string mContent;
    std::optional<std::uint16_t> mWidth;
};
