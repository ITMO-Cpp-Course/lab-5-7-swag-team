#include "text_utils.hpp"

#include <cctype>

namespace lab5::index
{

char normalize(char ch)
{
    return static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
}

std::string normalize(const std::string& str)
{
    std::string result;
    result.reserve(str.size());
    for (char c : str)
        result += normalize(c);
    return result;
}
} // namespace lab5::index