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
    bool add(Document doc);
    bool remove(const std::string& name);
    std::vector<std::string> search(const std::string& word) const;
    size_t count(const std::string& word, const std::string& doc_name) const;

  private:
    size_t next_id_ = 1;

    std::unordered_map<size_t, Document> docs_;
    std::unordered_map<std::string, std::unordered_map<size_t, size_t>> index_;
    std::unordered_map<std::string, size_t> name_to_id_; // доп мапа
    // нужна чтобы имя файла помнило какое айди у него
};
} // namespace lab5::index