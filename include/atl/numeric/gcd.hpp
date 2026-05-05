#ifndef ATL_NUMERIC_GCD_HPP
#define ATL_NUMERIC_GCD_HPP

#include <bit>          // std::countr_zero
#include <concepts>     // std::integral, std::is_signed, std::is_unsigned
#include <limits>       // std::numeric_limits
#include <type_traits>  // std::common_type, std::make_unsigned, std::is_convertible, std::is_nothrow_convertible

namespace abl {

/// @brief Computes the greatest common divisor of two integers using the binary
/// Euclidean algorithm.
/// @param a The first integer.
/// @param b The second integer.
/// @return The greatest common divisor of `a` and `b`.
/// @note Throws an exception if the result cannot be converted to
/// `std::common_type_t<T, U>`.
template <typename T, typename U>
    requires requires { typename std::common_type_t<T, U>; } &&
             std::is_convertible_v<T, std::common_type_t<T, U>> &&
             std::is_convertible_v<U, std::common_type_t<T, U>> &&
             std::integral<std::common_type_t<T, U>> &&
             std::is_signed_v<std::common_type_t<T, U>>
constexpr std::common_type_t<T, U> gcd(const T& a, const U& b) noexcept(
    std::is_nothrow_convertible_v<T, std::common_type_t<T, U>> &&
    std::is_nothrow_convertible_v<U, std::common_type_t<T, U>>) {
    using common_t = std::common_type_t<T, U>;
    using common_ut = std::make_unsigned_t<common_t>;

    common_ut acu = a < 0 ? (a == std::numeric_limits<common_t>::min()
                                 ? static_cast<common_ut>(a)
                                 : static_cast<common_ut>(-a))
                          : static_cast<common_ut>(a);

    common_ut bcu = b < 0 ? (b == std::numeric_limits<common_t>::min()
                                 ? static_cast<common_ut>(b)
                                 : static_cast<common_ut>(-b))
                          : static_cast<common_ut>(b);

    if (!acu || !bcu) return static_cast<common_t>(acu | bcu);

    auto shift = std::countr_zero(acu | bcu);
    acu >>= std::countr_zero(acu);
    while (bcu != 0) {
        bcu >>= std::countr_zero(bcu);
        if (acu > bcu) std::swap(acu, bcu);
        bcu -= acu;
    }
    acu <<= shift;

    return static_cast<common_t>(acu);
}

/// @brief Computes the greatest common divisor of two integers using the binary
/// Euclidean algorithm.
/// @param a The first integer.
/// @param b The second integer.
/// @return The greatest common divisor of `a` and `b`.
/// @note Throws an exception if the result cannot be converted to
/// `std::common_type_t<T, U>`.
template <typename T, typename U>
    requires requires { typename std::common_type_t<T, U>; } &&
             std::is_convertible_v<T, std::common_type_t<T, U>> &&
             std::is_convertible_v<U, std::common_type_t<T, U>> &&
             std::integral<std::common_type_t<T, U>> &&
             std::is_unsigned_v<std::common_type_t<T, U>>
constexpr std::common_type_t<T, U> gcd(const T& a, const U& b) noexcept(
    std::is_nothrow_convertible_v<T, std::common_type_t<T, U>> &&
    std::is_nothrow_convertible_v<U, std::common_type_t<T, U>>) {
    using common_ut = std::common_type_t<T, U>;

    auto acu = static_cast<common_ut>(a), bcu = static_cast<common_ut>(b);
    if (!acu || !bcu) return acu | bcu;

    auto shift = std::countr_zero(acu | bcu);
    acu >>= std::countr_zero(acu);
    while (bcu != 0) {
        bcu >>= std::countr_zero(bcu);
        if (acu > bcu) std::swap(acu, bcu);
        bcu -= acu;
    }
    acu <<= shift;

    return acu;
}

/// @brief Computes the greatest common divisor of two or more integers using the binary
/// Euclidean algorithm.
/// @param a The first integer.
/// @param b The second integer.
/// @param c The third integer.
/// @param rest The remaining integers.
/// @return The greatest common divisor of `a`, `b`, `c`, and `rest...`.
/// @note Throws an exception if the result cannot be converted to
/// `std::common_type_t<T1, T2, T3, Ts...>`.
template <typename T1, typename T2, typename T3, typename... Ts>
    requires requires(const T1& a, const T2& b) { gcd(a, b); }
constexpr std::common_type_t<T1, T2, T3, Ts...> gcd(const T1& a, const T2& b,
                                                    const T3& c,
                                                    const Ts&... rest) {
    return gcd(gcd(a, b), c, rest...);
}

/// @brief Computes the least common multiple of two integers.
/// @param a The first integer.
/// @param b The second integer.
/// @return The least common multiple of `a` and `b`.
/// @note Throws an exception if the result cannot be converted to
/// `std::common_type_t<T, U>`.
/// @note LCM of two negative integers is negative.
template <typename T, typename U>
    requires requires { typename std::common_type_t<T, U>; } &&
             std::is_convertible_v<T, std::common_type_t<T, U>> &&
             std::is_convertible_v<U, std::common_type_t<T, U>> &&
             requires(const T& a, const T& b) { gcd(a, b); }
constexpr std::common_type_t<T, U> lcm(const T& a, const U& b) noexcept(
    std::is_nothrow_convertible_v<T, std::common_type_t<T, U>> &&
    std::is_nothrow_convertible_v<U, std::common_type_t<T, U>>) {
    if (a == 0 || b == 0) return 0;
    auto d = gcd(a, b);
    if constexpr (std::is_signed_v<decltype(d)>) {
        decltype(d) sign = (a < 0) && (b < 0) ? -1 : 1;
        return sign * (static_cast<decltype(d)>(a) / d) *
               static_cast<decltype(d)>(b);
    }
    return (static_cast<decltype(d)>(a) / d) * static_cast<decltype(d)>(b);
}

/// @brief Computes the least common multiple of two or more integers.
/// @param a The first integer.
/// @param b The second integer.
/// @param c The third integer.
/// @param rest The remaining integers.
/// @return The least common multiple of `a`, `b`, `c`, and `rest...`.
/// @note Throws an exception if the result cannot be converted to
/// `std::common_type_t<T1, T2, T3, Ts...>`.
/// @note LCM of two negative integers is negative.
template <typename T1, typename T2, typename T3, typename... Ts>
    requires requires(const T1& a, const T2& b) { lcm(a, b); }
constexpr std::common_type_t<T1, T2, T3, Ts...> lcm(const T1& a, const T2& b,
                                                    const T3& c,
                                                    const Ts&... rest) {
    return lcm(lcm(a, b), c, rest...);
}

}  // namespace abl

#endif  // ATL_NUMERIC_GCD_HPP
