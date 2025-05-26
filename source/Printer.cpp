#include <cctype>
#include <cstddef>
#include <iostream>

#include "aoc-cli/Printer.hpp"

void printer::operator()() const
{
    if (!m_width.has_value() || m_width.value() == 0) {
        std::cout << m_content;
    } else {
        const auto width = m_width.value();
        decltype(m_content.size()) begin = 0;
        while (begin < m_content.size()) {
            auto line = m_content.substr(begin, width);

            if (const auto new_line = line.find_last_of('\n');
                new_line != std::string::npos)
            {
                // New line characters needs to be considered: if a new line is
                // in the line, change `begin` to just after the new line and
                // continue.
                line = m_content.substr(begin, new_line + 1);
                std::cout << line;
                begin += line.size();
            } else if (const std::size_t idx = width - 1; !line.empty()
                       && idx > 0 && (std::isspace(line[idx]) == 0))
            {
                // If a line doesn't end in whitespace, that means we're cutting
                // off a word. Go backwards.
                const auto end_index = line.find_last_of(' ');
                line = m_content.substr(begin, end_index);
                std::cout << line << "\n";

                // If begin is not adjusted, some lines will contain a single
                // space at the beginning of the line.
                begin += line.size() + 1;
            } else {
                std::cout << line << "\n";
                begin += line.size();
            }
        }
    }
}
