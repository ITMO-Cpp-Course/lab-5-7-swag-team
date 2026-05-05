#pragma once
#include <expected>
#include <string>

namespace lab5::index
{
template <typename T> using Result = std::expected<T, std::string>;
}