#pragma once

#include "index_store.hpp"
#include "update_transaction.hpp"

namespace lab5::index
{
Result<> IndexStore::add(Document doc)
{
    return index_.add(std::move(doc));
}

Result<> IndexStore::remove(const std::string& name)
{
    return index_.remove(name);
}

Result<std::vector<std::string>> IndexStore::search(const std::string& word) const
{
    return index_.search(word);
}

Result<size_t> IndexStore::count(const std::string& word, const std::string& doc_name) const
{
    return index_.count(word, doc_name);
}

UpdateTransaction IndexStore::start_transaction()
{
    return UpdateTransaction(index_);
}
} // namespace lab5::index