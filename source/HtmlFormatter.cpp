#include "aoc-cli/HtmlFormatter.hpp"

#include <libxml/HTMLparser.h>

namespace
{
auto get_string_from_xml_char(const xmlChar* xc) -> std::string
{
    return reinterpret_cast<const char*>(xc);
}

auto is_heading_node(xmlNodePtr node) -> bool
{
    return (node != nullptr) && (node->parent != nullptr)
        && (node->parent->name != nullptr)
        && get_string_from_xml_char(node->parent->name) == "h2";
}

// Close, but not quite:
// clang-format off
    //  - The first Elf is carrying food with 1000 - , 2000 - , and 3000 -  Calories, a total of 6000 -  Calories.
// clang-format on

auto is_list_node(xmlNodePtr node) -> bool
{
    if (node == nullptr) {
        return false;
    }

    auto* const parent = node->parent;
    if (parent == nullptr) {
        return false;
    }

    auto* const grand_parent = parent->parent;
    if (grand_parent == nullptr) {
        return false;
    }

    if ((parent->name == nullptr) || (grand_parent->name == nullptr)) {
        return false;
    }

    return get_string_from_xml_char(parent->name) == "li"
        && get_string_from_xml_char(grand_parent->name) == "ul";
}

// clang-tidy says don't use recursion: https://stackoverflow.com/a/63939994
// TODO: Investigate an iterative solution
void extract_text_nodes(xmlNodePtr node, std::stringstream& ss)
{
    if (node == nullptr) {
        return;
    }

    if (is_heading_node(node)) {
        ss << "\n";
    }

    if (node->type == XML_TEXT_NODE) {
        ss << node->content;

        if (node->children != nullptr) {
            ss << "\n\n";
        }

        if (is_heading_node(node)) {
            ss << "\n\n";
        }
    }

    extract_text_nodes(node->children, ss);
    extract_text_nodes(node->next, ss);
}
}  // namespace

void html_formatter::extract_content(const std::string& html)
{
    auto* const doc = htmlReadMemory(html.c_str(),
                                     static_cast<int>(html.size()),
                                     "",
                                     nullptr,
                                     XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
    if (doc != nullptr) {
        extract_text_nodes(reinterpret_cast<xmlNodePtr>(doc), m_stream);

        xmlFreeDoc(doc);
    }
}
