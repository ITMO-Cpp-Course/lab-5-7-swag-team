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
    static std::vector<std::string> tokenize(const std::string& text);
};

} // namespace lab5::index