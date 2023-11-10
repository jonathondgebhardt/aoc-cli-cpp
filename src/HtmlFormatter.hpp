#pragma once

#include "HtmlContent.hpp"
#include <sstream>

//! Extracts specific content from HtmlContent and formats it.
class HtmlFormatter
{
public:
    explicit HtmlFormatter(const HtmlContent& content);
    ~HtmlFormatter() noexcept = default;

    std::string operator()() const;

private:
    std::stringstream mStream;
};
