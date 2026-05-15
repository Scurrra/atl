#ifndef ATL_CONCEPTS_HPP
#define ATL_CONCEPTS_HPP

#include <concepts>

#include "type_traits.hpp"

namespace atl {

/// @brief Constrains a type pack to contain only one distinct type.
template <typename T, typename... Ts>
concept all_same = (std::is_same_v<T, Ts> && ...);

/// @brief Constrains a type pack to have no duplicates.
template <typename... Ts>
concept all_unique = are_unique<Ts...>::value;

/// @brief Constrains `T` to be present in `Ts...`.
template <typename T, typename... Ts>
concept contains = (std::is_same_v<T, Ts> || ...);

}  // namespace abl

#endif
