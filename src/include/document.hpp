#pragma once

#include <string>
#include <vector>

namespace lab5::index
{

struct Document
{
    size_t id;
    std::string name;
    std::vector<std::string> words; // технически, текст, просто лень было писать text_ где-то
};

} // namespace lab5::index
