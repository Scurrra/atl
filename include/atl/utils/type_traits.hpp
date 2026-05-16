#ifndef ATL_TYPE_TRAITS_HPP
#define ATL_TYPE_TRAITS_HPP

#include <cstddef>      //std::size_t
#include <type_traits>  // std::is_same

namespace abl {

/// @brief Compile-time maximum of `std::size_t` values.
template <std::size_t... Ns>
struct static_max {
    static constexpr std::size_t value = 1;
};
template <std::size_t N>
struct static_max<N> {
    static constexpr std::size_t value = N;
};
template <std::size_t N1, std::size_t N2, std::size_t... Ns>
struct static_max<N1, N2, Ns...> {
    static constexpr std::size_t value =
        static_max<(N1 > N2 ? N1 : N2), Ns...>::value;
};

/// @brief Helper variable for `static_max`.
template <std::size_t... Ns>
inline constexpr std::size_t static_max_v = static_max<Ns...>::value;

/// @brief Compile-time minimum of `std::size_t` values.
template <std::size_t... Ns>
struct static_min {
    static constexpr std::size_t value = 0;
};
template <std::size_t N>
struct static_min<N> {
    static constexpr std::size_t value = N;
};
template <std::size_t N1, std::size_t N2, std::size_t... Ns>
struct static_min<N1, N2, Ns...> {
    static constexpr std::size_t value =
        static_min<(N1 < N2 ? N1 : N2), Ns...>::value;
};

/// @brief Helper variable for `static_min`.
template <std::size_t... Ns>
inline constexpr std::size_t static_min_v = static_min<Ns...>::value;

/// @brief Checks if all types are the same.
template <typename... Ts>
struct are_same;
template <typename T1, typename T2>
struct are_same<T1, T2> {
    static constexpr bool value = std::is_same<T1, T2>::value;
};
template <typename T1, typename T2, typename... Ts>
struct are_same<T1, T2, Ts...> {
    static constexpr bool value =
        std::is_same<T1, T2>::value && are_same<T2, Ts...>::value;
};

/// @brief Helper variable for `are_same`.
template <typename... Ts>
inline constexpr bool are_same_v = are_same<Ts...>::value;

/// @brief Checks if all types are unique.
template <typename... Ts>
struct are_unique {
    static constexpr bool value = false;
};
template <typename T1, typename T2>
struct are_unique<T1, T2> {
    static constexpr bool value = !std::is_same<T1, T2>::value;
};
template <typename T1, typename T2, typename... Ts>
struct are_unique<T1, T2, Ts...> {
    static constexpr bool value =
        !std::is_same<T1, T2>::value && are_unique<T2, Ts...>::value;
};

/// @brief Helper variable for `are_unique`.
template <typename... Ts>
inline constexpr bool are_unique_v = are_unique<Ts...>::value;

/// @brief Checks if a type is contained in a pack of types.
template <typename T, typename... Ts>
struct contains_type {
    static constexpr bool value = (std::is_same_v<T, Ts> || ...);
};

/// @brief Helper variable for `contains_type`.
template <typename T, typename... Ts>
inline constexpr bool contains_type_v = contains_type<T, Ts...>::value;

}  // namespace abl

#endif  // ATL_TYPE_TRAITS_HPP
