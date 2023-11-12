#pragma once

#include "HtmlContent.hpp"
#include <sstream>

//! Extracts all plain text from HtmlContent and attempts to format it with operator().
class HtmlFormatter
{
public:
    //! Extracts plain text from the HtmlContent. HtmlContent should be valid HTML.
    explicit HtmlFormatter(const HtmlContent& content);
    ~HtmlFormatter() noexcept = default;

    //! Yields formatted plain text.
    std::string operator()() const;

private:
    std::stringstream mStream;
};
