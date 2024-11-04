#pragma once

#include <sstream>

#include "HtmlContent.hpp"

//! \brief Extracts all plain text from HtmlContent and attempts to format it with operator().
class HtmlFormatter
{
public:
    //! \brief Extracts plain text from the HtmlContent. HtmlContent should be valid HTML.
    explicit HtmlFormatter(const HtmlContent& content);

    //! \brief Yields formatted plain text.
    std::string operator()() const { return mStream.str(); }

    static std::string Format(const HtmlContent& content)
    {
        const HtmlFormatter formatter{content};
        return formatter();
    }

private:
    std::stringstream mStream;
};
