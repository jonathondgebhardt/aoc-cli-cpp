#pragma once

#include "HtmlContent.hpp"

//! Extracts specific content from HtmlContent and formats it.
class HtmlFormatter
{
public:
    explicit HtmlFormatter(const HtmlContent& content);
    ~HtmlFormatter() noexcept = default;

    std::string operator()() const;

private:
    HtmlContent mContent;
};
