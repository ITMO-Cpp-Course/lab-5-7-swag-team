#pragma once

#include "document.hpp"
#include "inverted_index.hpp"
#include "result.hpp"

#include <string>
#include <variant>
#include <vector>

namespace lab5::index
{

class UpdateTransaction
{
  public:
    explicit UpdateTransaction(InvertedIndex& index);
    ~UpdateTransaction();

    UpdateTransaction(const UpdateTransaction&) = delete;
    UpdateTransaction& operator=(const UpdateTransaction&) = delete;
    UpdateTransaction(UpdateTransaction&&) noexcept;
    UpdateTransaction& operator=(UpdateTransaction&&) = delete;

    Result<void> add(Document doc);
    Result<void> remove(const std::string& name);
    void commit();

  private:
    struct UndoAdd
    {
        std::string name;
    };

    struct UndoRemove
    {
        Document doc;
    };

    using UndoEntry = std::variant<UndoAdd, UndoRemove>;

    InvertedIndex* index_;
    bool committed_ = false;
    std::vector<UndoEntry> undo_log_;

    void rollback() noexcept; // чтобы не выбросить исключение в деструкторе
};

} // namespace lab5::index
