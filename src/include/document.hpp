#pragma once

#include <string>
#include <vector>

namespace lab5::index
{

struct Document
{
    size_t id = 0;
    std::string name;
    std::vector<std::string> words; // технически, текст, просто лень было писать что-то длинное где-то
};

} // namespace lab5::index