#pragma once

#include <vector>

#include "HtmlContent.hpp"
#include <sstream>

//! Extracts all plain text from HtmlContent and attempts to format it with operator().
class HtmlFormatter
{
public:
    //! Extracts plain text from the HtmlContent. HtmlContent should be valid HTML.
    explicit HtmlFormatter(const HtmlContent& content);

    //! Yields formatted plain text.
    std::string operator()();

private:
    std::stringstream mStream;
};
