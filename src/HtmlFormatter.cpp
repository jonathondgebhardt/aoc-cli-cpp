#include "HtmlFormatter.hpp"

#include <iostream>
#include <libxml/HTMLparser.h>

namespace
{
    std::string GetStringFromXmlChar(const xmlChar* xc)
    {
        return reinterpret_cast<const char*>(xc);
    }

    bool IsHeadingNode(xmlNodePtr node)
    {
        return node && node->parent && node->parent->name &&
               GetStringFromXmlChar(node->parent->name) == "h2";
    }

    // Close, but not quite:
    // clang-format off
    //  - The first Elf is carrying food with 1000 - , 2000 - , and 3000 -  Calories, a total of 6000 -  Calories.
    // clang-format on
    bool IsListNode(xmlNodePtr node)
    {
        if(!node)
        {
            return false;
        }

        const auto parent = node->parent;
        if(!parent)
        {
            return false;
        }

        const auto grandParent = parent->parent;
        if(!grandParent)
        {
            return false;
        }

        if(!parent->name || !grandParent->name)
        {
            return false;
        }

        return GetStringFromXmlChar(parent->name) == "li" &&
               GetStringFromXmlChar(grandParent->name) == "ul";
    }

    void ExtractTextNodes(xmlNodePtr node, std::stringstream& ss)
    {
        if(!node)
        {
            return;
        }

        if(IsHeadingNode(node))
        {
            ss << "\n";
        }

        if(node->type == XML_TEXT_NODE)
        {
            ss << node->content;

            if(node->children != nullptr)
            {
                ss << "\n\n";
            }

            if(IsHeadingNode(node))
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
