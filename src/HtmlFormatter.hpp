#pragma once

#include <sstream>
#include <vector>

#include "HtmlContent.hpp"

//! \brief Extracts all plain text from HtmlContent and attempts to format it with operator().
class HtmlFormatter
{
public:
    //! \brief Extracts plain text from the HtmlContent. HtmlContent should be valid HTML.
    explicit HtmlFormatter(const HtmlContent& content);

    //! \brief Yields formatted plain text.
    std::string operator()() { return mStream.str(); }

private:
    std::stringstream mStream;
};
