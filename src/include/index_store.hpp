#pragma once

#include "inverted_index.hpp"
#include "result.hpp"
namespace lab5::index
{
class UpdateTransaction;

class IndexStore
{
  public:
    Result<void> add(Document doc);
    Result<void> remove(const std::string& name);
    Result<std::vector<std::string>> search(const std::string& word) const;
    Result<size_t> count(const std::string& word, const std::string& doc_name) const;
    UpdateTransaction start_transaction();

  private:
    InvertedIndex index_;
};
} // namespace lab5::index