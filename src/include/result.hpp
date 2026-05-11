#pragma once

#include "error.hpp"

#include <expected>

namespace lab5::index
{

template <typename T> using Result = std::expected<T, IndexError>;

} // namespace lab5::index
