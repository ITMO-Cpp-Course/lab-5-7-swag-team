#pragma once

#include "document.hpp"

#include <string>
#include <vector>

namespace lab5::index
{

class DocumentBuilder
{
  public:
    Document build(std::string name, const std::string& text);

  private:
    std::vector<std::string> tokenize(const std::string& text);
    // std::string normalize(const std::string& word); пока пусть будет
    char normalize(char ch);
};

} // namespace lab5::index
