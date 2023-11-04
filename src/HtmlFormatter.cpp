#include "HtmlFormatter.hpp"

HtmlFormatter::HtmlFormatter(const HtmlContent& content) : mContent(content)
{
}

std::string HtmlFormatter::operator()() const
{
    return std::string();
}
