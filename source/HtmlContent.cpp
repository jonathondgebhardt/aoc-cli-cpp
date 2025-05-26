#include <sstream>
#include <string>

#include "aoc-cli/HtmlContent.hpp"

auto html_content::extract() -> std::string
{
    // TODO: HTML is not case sensitive, should make all tags the same case. Use
    // libxml instead?
    // TODO: Use regex instead? I've gone down this road with iterative regex
    // matches and had a lot of trouble.
    if (!m_begin.empty() && !m_end.empty()) {
        std::stringstream stream;

        auto begin_index = m_content.find(m_begin);
        while (begin_index != std::string::npos) {
            begin_index += m_begin.size();

            auto end_index = m_content.find(m_end, begin_index);
            if (end_index == std::string::npos) {
                break;
            }

            const auto length = end_index - begin_index;
            stream << m_content.substr(begin_index, length);

            begin_index = m_content.find(m_begin, end_index);
        }

        m_extracted_content = stream.str();
    } else {
        m_extracted_content = m_content;
    }

    return m_extracted_content;
}
