#include "HtmlFormatter.hpp"

#include <iostream>
#include <regex>

namespace
{
    enum ContentType
    {
        Heading = 0, // h*
        Paragraph,   // p
        List         // li
    };

    using Content = std::pair<ContentType, std::string>;
}

HtmlFormatter::HtmlFormatter(const HtmlContent& content) : mContent(content)
{
    // associate content type with content
    // ex:
    // Paragraph,
    // heading (<h*>), --- Day 1: Calorie Counting ---
    // paragraph (<p>), Santa's reindeer typically eat regular reindeer food
    // this content is removed but the order is maintained.
    //
    // Here's the blessed regex to match any <.*>: <\w*\s*\w*[^\/]>*?
    // This doesn't match everything, but I think it'll suit our purposes.
    //
    // Any content within the content we want should lose it's tags. Ex:
    // <p>i'm some text with <em>a special thing</em>!!!</p>
    //
    // Or I could grab all the text between > and < ...

//    const std::regex re{">.*?<"};
//    std::smatch match;
//    const std::string& htmlContent = content();
//    if(std::regex_match(htmlContent, match, re))
//    {
//        std::cout << match[1] << "\n";
//    }
//
//    if(std::regex_search(htmlContent, match, re))
//    {
//        for(size_t i = 0; i < match.size(); ++i)
//        {
//            std::cout << i << ": " << match[i] << "\n";
//        }
//    }

    const std::string fnames[] = {"foo.txt", "bar.txt", "baz.dat", "zoidberg"};
    const std::regex txt_regex("[a-z]+\\.txt");

    for (const auto& fname : fnames)
        std::cout << fname << ": " << std::regex_match(fname, txt_regex) << '\n';

    // Extraction of a sub-match
    const std::regex base_regex("([a-z]+)\\.txt");
    std::smatch base_match;

    for (const auto& fname : fnames)
        if (std::regex_match(fname, base_match, base_regex))
            // The first sub_match is the whole string; the next
            // sub_match is the first parenthesized expression.
            if (base_match.size() == 2)
            {
                std::ssub_match base_sub_match = base_match[1];
                std::string base = base_sub_match.str();
                std::cout << fname << " has a base of " << base << '\n';
            }
}

std::string HtmlFormatter::operator()() const
{
    return {};
}
