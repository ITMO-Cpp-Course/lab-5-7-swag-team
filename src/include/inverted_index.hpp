#pragma once

#include "document.hpp"
#include "result.hpp"

#include <string>
#include <unordered_map>
#include <vector>

namespace lab5::index
{

class InvertedIndex
{
  public:
    Result<void> add(Document doc);
    Result<void> remove(const std::string& name);
    std::vector<std::string> search(const std::string& word) const;
    Result<size_t> count(const std::string& word, const std::string& doc_name) const;
    const Document* get(const std::string& name) const; // нужен UpdateTransaction, чтобы делать копию и откатывать изменения

  private:
    using id_t = size_t;
    using word_t = std::string;
    using name_t = std::string;
    size_t next_id_ = 1;

    std::unordered_map<id_t, Document> docs_;
    std::unordered_map<word_t, std::unordered_map<id_t, size_t>> index_;
    std::unordered_map<name_t, id_t> name_to_id_; // доп мапа
    // нужна, чтобы имя файла помнило какое айди у него
};
} // namespace lab5::index