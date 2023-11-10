#include "HtmlFormatter.hpp"

#include <iostream>
#include <libxml/HTMLparser.h>

namespace
{
    std::string GetStringFromXmlChar(const xmlChar* xc)
    {
        return reinterpret_cast<const char*>(xc);
    }

    void PrintTextNodes(xmlNodePtr node)
    {
        if(!node)
        {
            return;
        }

        if(node->name && GetStringFromXmlChar(node->name) == "h2")
        {
            std::cout << "\n";
        }

        auto addNewLine = node->children != nullptr;

        if(node->parent && node->parent->name)
        {
            std::string name = GetStringFromXmlChar(node->parent->name);
            addNewLine |= name == "h2";
        }

        if(node->type == XML_TEXT_NODE)
        {
            std::cout << node->content;

            if(addNewLine)
            {
                std::cout << "\n";
            }
        }

        PrintTextNodes(node->children);
        PrintTextNodes(node->next);
    }

}

HtmlFormatter::HtmlFormatter(const HtmlContent& content) : mContent(content)
{
    const auto& htmlContent = content();
    const auto doc = htmlReadMemory(htmlContent.c_str(), htmlContent.size(), "", nullptr,
                                    XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
    if(doc)
    {
        // root
        //  "html" (XML_DTD_NODE), _"html" (XML_ELEMENT_NODE)_
        //      "body" (XML_ELEMENT_NODE)
        //          "article" (XML_ELEMENT_NODE)
        //              "h2" (XML_ELEMENT_NODE)
        //                  "text" (XML_TEXT_NODE) -> "--- Day 1: Calorie Counting---"

        PrintTextNodes(reinterpret_cast<xmlNodePtr>(doc));

        xmlFreeDoc(doc);
    }
}

std::string HtmlFormatter::operator()() const
{
    return {};
}
