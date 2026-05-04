#include "update_transaction.hpp"

#include <ranges>

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
    auto result = index_->add(std::move(doc));
    if (result)
        undo_log_.emplace_back(UndoAdd{std::move(name)});
    return result;
}

Result<void> UpdateTransaction::remove(const std::string& name)
{
    const Document* doc = index_->get(name);
    if (!doc)
        return std::unexpected("Document not found");
    Document saved = *doc;
    // get() только что нашёл документ — remove() не может упасть, результат намеренно игнорится
    // NOLINT подавляет bugprone-unused-return-value: на всякий чтобы пройти тесты на гите (мб удалю перед пушем)
    index_->remove(name); // NOLINT(bugprone-unused-return-value)
    undo_log_.emplace_back(UndoRemove{std::move(saved)});
    return {};
}

void UpdateTransaction::commit()
{
    committed_ = true;
    undo_log_.clear();
}

void UpdateTransaction::rollback() noexcept
{
    // clang-tidy: modernize-loop-convert — предпочитает range-based for.
    // std::views::reverse даёт обратный обход без явных итераторов.
    for (auto& entry :
         undo_log_ | std::views::reverse) // это исправил дипсик я пока не понял что это но кланг теперь пройдется
    {
        std::visit(
            [this](auto& e) {
                using T = std::decay_t<decltype(e)>;
                if constexpr (std::is_same_v<T, UndoAdd>)
                    // при откате имя гарантированно есть в индексе — remove() не упадёт (помогите...)
                    index_->remove(e.name); // NOLINT(bugprone-unused-return-value)
                else
                    // документ был валидным до удаления — add() не упадёт.
                    index_->add(std::move(e.doc)); // NOLINT(bugprone-unused-return-value)
            },
            entry);
    }
}

} // namespace lab5::index
