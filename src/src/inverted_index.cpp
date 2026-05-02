#include "inverted_index.hpp"
#include "text_utils.hpp"

namespace lab5::index
{
bool InvertedIndex::add(const Document& doc)
{
    return add(Document(doc));
}

bool InvertedIndex::add(Document&& doc)
{
    if (doc.name.empty())
        return false;
    if (doc.words.empty())
        return false;
    if (name_to_id_.find(doc.name) != name_to_id_.end())
        return false; // проверка на уникальность имени
    doc.id = next_id_++;
    name_to_id_.emplace(doc.name, doc.id);

    for (const std::string& word : doc.words)
        index_[word][doc.id]++;

    docs_.emplace(doc.id, std::move(doc));
    return true;
}
bool InvertedIndex::remove(const std::string& name)
{
    auto name_it = name_to_id_.find(name); // ищем по имени айди
    if (name_it == name_to_id_.end())      // по доп мапе
        return false;
    size_t id = name_it->second;
    auto doc_it = docs_.find(id); // найденному id ищем док
    if (doc_it == docs_.end())
        return false;

    for (const auto& word : doc_it->second.words) // удаляем слова
    {
        auto& posting = index_[word];
        posting.erase(id);
        if (posting.empty()) // проверка на вхождение удаленного слова в другие документы
            index_.erase(word);
    }

    docs_.erase(doc_it);
    name_to_id_.erase(name_it);
    return true;
}
std::vector<std::string> InvertedIndex::search(const std::string& word) const
{
    auto it = index_.find(normalize(word));
    if (it == index_.end())
        return {};

    std::vector<std::string> res;
    res.reserve(it->second.size());
    for (const auto& [doc_id, _] : it->second)
        res.push_back(docs_.at(doc_id).name);

    return res;
}

size_t InvertedIndex::count(const std::string& word, const std::string& doc_name) const
{
    auto name_it = name_to_id_.find(doc_name);
    if (name_it == name_to_id_.end())
        return 0;

    auto word_it = index_.find(normalize(word));
    if (word_it == index_.end())
        return 0;

    auto doc_it = word_it->second.find(name_it->second);
    if (doc_it == word_it->second.end())
        return 0;

    return doc_it->second;
}

} // namespace lab5::index