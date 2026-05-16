#ifndef ATL_NUMERIC_minmax_HPP
#define ATL_NUMERIC_minmax_HPP

#include <concepts>  // std::predicate
#include <type_traits>  // std::common_type, std::is_convertible, std::is_nothrow_convertible
#include <utility>  // std::pair

namespace abl {

/// @brief Returns the minimum of two values.
/// @param a The first value.
/// @param b The second value.
/// @return The minimum of `a` and `b` as a constant reference.
template <typename T>
constexpr const T& min(const T& a, const T& b) {
    return a < b ? a : b;
}

/// @brief Returns the minimum of two values using a custom comparison function.
/// @param a The first value.
/// @param b The second value.
/// @param comp The comparison function.
/// @return The minimum of `a` and `b` as a constant reference.
template <typename Compare, typename T>
    requires std::predicate<const Compare&, const T&, const T&>
constexpr const T& min(const T& a, const T& b, Compare comp) {
    return comp(a, b) ? a : b;
}

/// @brief Returns the minimum of two values.
/// @param a The first value.
/// @param b The second value.
/// @return The minimum of `a` and `b` as a value of their common type.
template <typename T, typename U>
    requires requires { typename std::common_type_t<T, U>; } &&
             (std::is_convertible_v<T, std::common_type_t<T, U>> ||
              std::is_convertible_v<U, std::common_type_t<T, U>>)
std::common_type_t<T, U> min(const T& a, const U& b) noexcept(
    std::is_nothrow_convertible_v<T, std::common_type_t<T, U>> ||
    std::is_nothrow_convertible_v<U, std::common_type_t<T, U>>) {
    using common_t = std::common_type_t<T, U>;
    auto ca = static_cast<common_t>(a), cb = static_cast<common_t>(b);
    return ca < cb ? ca : cb;
}

/// @brief Returns the minimum of two values using a custom comparison function.
/// @param a The first value.
/// @param b The second value.
/// @param comp The comparison function.
/// @return The minimum of `a` and `b` as a value of their common type.
template <typename Compare, typename T, typename U>
    requires requires { typename std::common_type_t<T, U>; } &&
             (std::is_convertible_v<T, std::common_type_t<T, U>> ||
              std::is_convertible_v<U, std::common_type_t<T, U>>) &&
             std::predicate<const Compare&, const std::common_type_t<T, U>&,
                            const std::common_type_t<T, U>&>
std::common_type_t<T, U> min(const T& a, const U& b, Compare comp) noexcept(
    std::is_nothrow_convertible_v<T, std::common_type_t<T, U>> ||
    std::is_nothrow_convertible_v<U, std::common_type_t<T, U>>) {
    using common_t = std::common_type_t<T, U>;
    auto ca = static_cast<common_t>(a), cb = static_cast<common_t>(b);
    return comp(ca, cb) ? ca : cb;
}

/// @brief Returns the maximum of two values.
/// @param a The first value.
/// @param b The second value.
/// @return The maximum of `a` and `b` as a constant reference.
template <typename T>
constexpr const T& max(const T& a, const T& b) {
    return a > b ? a : b;
}

/// @brief Returns the maximum of two values using a custom comparison function.
/// @param a The first value.
/// @param b The second value.
/// @param comp The comparison function.
/// @return The maximum of `a` and `b` as a constant reference.
template <typename Compare, typename T>
    requires std::predicate<const Compare&, const T&, const T&>
constexpr const T& max(const T& a, const T& b, Compare comp) {
    return comp(a, b) ? a : b;
}

/// @brief Returns the maximum of two values.
/// @param a The first value.
/// @param b The second value.
/// @return The maximum of `a` and `b` as a value of their common type.
template <typename T, typename U>
    requires requires { typename std::common_type_t<T, U>; } &&
             (std::is_convertible_v<T, std::common_type_t<T, U>> ||
              std::is_convertible_v<U, std::common_type_t<T, U>>)
std::common_type_t<T, U> max(const T& a, const U& b) noexcept(
    std::is_nothrow_convertible_v<T, std::common_type_t<T, U>> ||
    std::is_nothrow_convertible_v<U, std::common_type_t<T, U>>) {
    using common_t = std::common_type_t<T, U>;
    auto ca = static_cast<common_t>(a), cb = static_cast<common_t>(b);
    return ca > cb ? ca : cb;
}

/// @brief Returns the maximum of two values using a custom comparison function.
/// @param a The first value.
/// @param b The second value.
/// @param comp The comparison function.
/// @return The maximum of `a` and `b` as a value of their common type.
template <typename Compare, typename T, typename U>
    requires requires { typename std::common_type_t<T, U>; } &&
             (std::is_convertible_v<T, std::common_type_t<T, U>> ||
              std::is_convertible_v<U, std::common_type_t<T, U>>) &&
             std::predicate<const Compare&, const std::common_type_t<T, U>&,
                            const std::common_type_t<T, U>&>
std::common_type_t<T, U> max(const T& a, const U& b, Compare comp) noexcept(
    std::is_nothrow_convertible_v<T, std::common_type_t<T, U>> ||
    std::is_nothrow_convertible_v<U, std::common_type_t<T, U>>) {
    using common_t = std::common_type_t<T, U>;
    auto ca = static_cast<common_t>(a), cb = static_cast<common_t>(b);
    return comp(ca, cb) ? ca : cb;
}

/// @brief Returns the minimum and maximum of two values.
/// @param a The first value.
/// @param b The second value.
/// @return A pair containing the minimum and maximum of `a` and `b`, where both
/// values are of their common type.
template <typename T>
constexpr std::pair<T, T> minmax(const T& a, const T& b) {
    if (a < b) return {a, b};
    return {b, a};
}

/// @brief Returns the minimum and maximum of two values using a custom
/// comparison function.
/// @param a The first value.
/// @param b The second value.
/// @param comp The comparison function.
/// @return A pair containing the minimum and maximum of `a` and `b`, where both
/// values are of their common type.
template <typename Compare, typename T>
    requires std::predicate<const Compare&, const T&, const T&>
constexpr std::pair<T, T> minmax(const T& a, const T& b, Compare comp) {
    if (comp(a, b)) return {a, b};
    return {b, a};
}

/// @brief Returns the minimum and maximum of two values.
/// @param a The first value.
/// @param b The second value.
/// @return A pair containing the minimum and maximum of `a` and `b`, where both
/// values are of their common type.
template <typename T, typename U>
    requires requires { typename std::common_type_t<T, U>; } &&
             (std::is_convertible_v<T, std::common_type_t<T, U>> ||
              std::is_convertible_v<U, std::common_type_t<T, U>>)
std::pair<std::common_type_t<T, U>, std::common_type_t<T, U>>
minmax(const T& a, const U& b) noexcept(
    std::is_nothrow_convertible_v<T, std::common_type_t<T, U>> ||
    std::is_nothrow_convertible_v<U, std::common_type_t<T, U>>) {
    using common_t = std::common_type_t<T, U>;
    auto ca = static_cast<common_t>(a), cb = static_cast<common_t>(b);
    if (ca < cb) return {ca, cb};
    return {cb, ca};
}

/// @brief Returns the minimum and maximum of two values using a custom
/// comparison function.
/// @param a The first value.
/// @param b The second value.
/// @param comp The comparison function.
/// @return A pair containing the minimum and maximum of `a` and `b`, where both
/// values are of their common type.
template <typename Compare, typename T, typename U>
    requires requires { typename std::common_type_t<T, U>; } &&
             (std::is_convertible_v<T, std::common_type_t<T, U>> ||
              std::is_convertible_v<U, std::common_type_t<T, U>>) &&
             std::predicate<const Compare&, const std::common_type_t<T, U>&,
                            const std::common_type_t<T, U>&>
std::pair<std::common_type_t<T, U>, std::common_type_t<T, U>>
minmax(const T& a, const U& b, Compare comp) noexcept(
    std::is_nothrow_convertible_v<T, std::common_type_t<T, U>> ||
    std::is_nothrow_convertible_v<U, std::common_type_t<T, U>>) {
    using common_t = std::common_type_t<T, U>;
    auto ca = static_cast<common_t>(a), cb = static_cast<common_t>(b);
    if (comp(ca, cb)) return {ca, cb};
    return {cb, ca};
}

}  // namespace abl

#endif
