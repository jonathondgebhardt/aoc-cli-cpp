#include "HtmlFormatter.hpp"

#include <iostream>
#include <libxml/HTMLparser.h>

namespace
{
    std::string GetStringFromXmlChar(const xmlChar* xc)
    {
        return reinterpret_cast<const char*>(xc);
    }

    void ExtractTextNodes(xmlNodePtr node, std::stringstream& ss)
    {
        if(!node)
        {
            return;
        }

        const auto isHeading =
            node->parent && node->parent->name && GetStringFromXmlChar(node->parent->name) == "h2";
        if(isHeading)
        {
            ss << "\n";
        }

        if(node->type == XML_TEXT_NODE)
        {
            // Close, but not quite:
            // clang-format off
            //  - The first Elf is carrying food with 1000 - , 2000 - , and 3000 -  Calories, a total of 6000 -  Calories.
            // clang-format on
            // Maybe I need to check the parent's parent?!?!?
            if(node->parent && node->parent->name &&
               GetStringFromXmlChar(node->parent->name) == "li")
            {
                ss << " - ";
            }

            ss << node->content;

            if(node->children != nullptr)
            {
                ss << "\n\n";
            }

            if(isHeading)
            {
                ss << "\n\n";
            }
        }

        ExtractTextNodes(node->children, ss);
        ExtractTextNodes(node->next, ss);
    }
}

HtmlFormatter::HtmlFormatter(const HtmlContent& content)
{
    const auto& contentStr = content();
    const auto doc = htmlReadMemory(contentStr.c_str(), contentStr.size(), "", nullptr,
                                    XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
    if(doc)
    {
        ExtractTextNodes(reinterpret_cast<xmlNodePtr>(doc), mStream);

        xmlFreeDoc(doc);
    }
}

std::string HtmlFormatter::operator()() const
{
    return mStream.str();
}
