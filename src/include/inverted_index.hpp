#pragma once

#include "document.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace lab5::index
{

class InvertedIndex
{
  public:
    void add(Document doc);
    void delete_(size_t id);
    std::vector<size_t> search(const std::string& word) const;
    size_t search_and_count(const std::string& word, size_t doc_id) const;

  private:
    size_t next_id = 1;

    std::unordered_map<size_t, Document> docs;
    std::unordered_map<std::string, std::unordered_map<size_t, size_t>> index;
};
} // namespace lab5::index