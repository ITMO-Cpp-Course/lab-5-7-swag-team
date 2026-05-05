#include "update_transaction.hpp"

namespace lab5::index
{

UpdateTransaction::UpdateTransaction(InvertedIndex& index) : index_(&index) {}

UpdateTransaction::UpdateTransaction(UpdateTransaction&& other) noexcept
    : index_(other.index_), committed_(other.committed_), undo_log_(std::move(other.undo_log_))
{
    other.index_ = nullptr;
}

UpdateTransaction::~UpdateTransaction()
{
    if (index_ && !committed_)
        rollback();
}

Result<void> UpdateTransaction::add(Document doc)
{
    std::string name = doc.name;
    undo_log_.reserve(undo_log_.size() + 1); // бросает до изменения индекса
    auto result = index_->add(std::move(doc));
    if (result)
        undo_log_.emplace_back(UndoAdd{std::move(name)}); // избежади ошибок памяти
    return result;
}

Result<void> UpdateTransaction::remove(const std::string& name)
{
    const Document* doc = index_->get(name);
    if (!doc)
        return std::unexpected("Document not found");
    Document saved = *doc;
    undo_log_.reserve(undo_log_.size() + 1); // бросает до изменения индекса
    index_->remove(name);
    undo_log_.emplace_back(UndoRemove{std::move(saved)}); // избежади ошибок памяти
    return {};
}

void UpdateTransaction::commit()
{
    committed_ = true;
    undo_log_.clear();
}

void UpdateTransaction::rollback() noexcept
{
    for (auto it = undo_log_.rbegin(); it != undo_log_.rend(); ++it)
    {
        std::visit(
            [this](auto& entry) {
                using T = std::decay_t<decltype(entry)>;  // убираем все адреса для умной проверки
                if constexpr (std::is_same_v<T, UndoAdd>) // умная проверка структуры от дипсика чтобы не писать кучу
                                                          // ифов на ундо адд и ремув
                    index_->remove(entry.name);
                else
                    index_->add(std::move(entry.doc));
            },
            *it);
    }
}

} // namespace lab5::index
