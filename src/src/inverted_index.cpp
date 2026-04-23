#include <inverted_index.hpp>

namespace lab5::index
{
void InvertedIndex::add(Document doc)
{
    doc.id = next_id++;

    for (const std::string& word : doc.words)
        index[word][doc.id]++;

    docs.emplace(doc.id, std::move(doc));
}
void InvertedIndex::delete_(size_t id)
{
    auto it = docs.find(id);
    if (it == docs.end())
        return;

    for (const std::string& word : it->second.words)
    {
        auto& posting = index[word];
        posting.erase(id);
        if (posting.empty())
            index.erase(word);
    }

    docs.erase(it);
}
std::vector<size_t> InvertedIndex::search(const std::string& word) const
{
    auto it = index.find(word);
    if (it == index.end())
        return {};

    std::vector<size_t> res;
    res.reserve(it->second.size());               // резервирует память
    for (const auto& [doc_id, freq] : it->second) // freq не используется можно заменить на _
        res.push_back(doc_id);

    return res;
}
size_t InvertedIndex::search_and_count(const std::string& word, size_t doc_id) const
{
    auto word_it = index.find(word);
    if (word_it == index.end())
        return 0;

    auto doc_it = word_it->second.find(doc_id);
    if (doc_it == word_it->second.end())
        return 0;

    return doc_it->second;
}
} // namespace lab5::index