#include "document_builder.hpp"
#include "text_utils.hpp"

#include <cctype>

namespace lab5::index
{

Document DocumentBuilder::build(std::string name, const std::string& text)
{
    Document doc;
    doc.id = 0;
    doc.name = std::move(name); // быстро без лишнего копирования
    doc.words = tokenize(text);
    return doc; // C++17 умный
}

std::vector<std::string> DocumentBuilder::tokenize(const std::string& text)
{
    std::vector<std::string> words;
    std::string word;

    for (size_t i = 0; i < text.size(); ++i)
    {
        const char ch = text[i];

        if (std::isalnum(static_cast<unsigned char>(ch)))
            word += normalize(ch);

        else if (ch == '-' && i > 0 && i + 1 < text.size() && std::isalpha(static_cast<unsigned char>(text[i - 1])) &&
                 std::isalpha(static_cast<unsigned char>(text[i + 1]))) // мб вынесу if в отдельный bool позже
        {
            word += '-';
        }

        else if (!word.empty())
        {
            words.push_back(std::move(word));
            word.clear();
        }
    }

    if (!word.empty())
        words.push_back(std::move(word));

    return words;
}

} // namespace lab5::index