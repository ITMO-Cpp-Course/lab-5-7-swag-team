#pragma once

namespace lab5::index
{

enum class ErrorCode
{
    EmptyName,
    EmptyText,
    DuplicateName,
    NotFound,
    InternalError
};

struct IndexError
{
    ErrorCode code;
};

} // namespace lab5::index
