#pragma once

#include <vector>

#include "HtmlContent.hpp"

//! Extracts specific content from HtmlContent and formats it. Use operator() to get formatted
//! content.
class HtmlFormatter
{
public:
    explicit HtmlFormatter(const HtmlContent& content);

    std::string operator()();

private:
    HtmlContent mContent;
};
