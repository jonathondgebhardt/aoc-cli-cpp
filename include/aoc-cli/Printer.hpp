#pragma once

#include <cstdint>
#include <optional>
#include <string>

#include "aoc-cli/aoc-cli_export.hpp"

//! \brief Prints some text to the screen.
//! If a width is set, hopefully inserts new lines intelligently so long lines
//! look nice.
class AOC_CLI_EXPORT printer
{
public:
    //! \param width The width to enforce.
    explicit printer(const std::optional<std::uint16_t> width)
        : printer(std::string(), width)
    {
    }

    //! \param content The content to print.
    //! \param width The width to enforce.
    explicit printer(std::string content,
                     const std::optional<std::uint16_t> width = std::nullopt)
        : m_content(std::move(content))
        , m_width(width)
    {
    }

    printer() = default;
    printer(const printer&) = default;
    printer(printer&&) = default;
    ~printer() noexcept = default;
    auto operator=(const printer&) -> printer& = default;
    auto operator=(printer&&) noexcept -> printer& = default;

    void set_content(std::string content) { m_content = std::move(content); }

    //! \brief Print the text to std::cout.
    void operator()() const;

private:
    AOC_CLI_CPP_SUPPRESS_C4251
    std::string m_content;
    std::optional<std::uint16_t> m_width;
};
