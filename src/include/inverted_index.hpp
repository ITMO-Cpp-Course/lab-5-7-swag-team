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
    std::vector<size_t> search(const std::string& word) const;
    size_t count(const std::string& word, size_t doc_id) const; // принимать название?? добавлю еще функцию
    // size_t count_by_name(const std::string& word, const std::string& name) const;
    // пригодится, дальше посмотрим, что оставим, можем обе

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