#pragma once

#include <string>

class Printer
{
public:
    Printer(const std::string& content, size_t width);
    ~Printer() noexcept = default;

    void operator()() const;

private:
    std::string mContent;
    size_t mWidth;
};
