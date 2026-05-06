#ifndef ATL_NUMERIC_GCD_HPP
#define ATL_NUMERIC_GCD_HPP

#include <bit>        // std::countr_zero
#include <concepts>   // std::integral, std::is_signed, std::is_unsigned
#include <limits>     // std::numeric_limits
#include <stdexcept>  // std::invalid_argument
#include <tuple>
#include <type_traits>  // std::common_type, std::make_unsigned, std::is_convertible, std::is_nothrow_convertible
#include <utility>      // std::swap
#include <vector>

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

/// @brief Computes the greatest common divisor of two or more integers using
/// the binary Euclidean algorithm.
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

/// @brief Computes the greatest common divisor of a list of integers using
/// the binary Euclidean algorithm.
/// @param numbers The list of integers.
/// @return The greatest common divisor of the integers in `numbers`.
/// @note Throws an exception if the result cannot be converted to
/// `std::common_type_t<T...>`.
template <typename T>
    requires std::integral<T>
constexpr T gcd(std::initializer_list<T> numbers) {
    if (numbers.size() < 2)
        throw std::invalid_argument(
            "abl::gcd takes at least two integer argumnets");

    std::vector<T> nums = numbers;
    T result = gcd(nums[0], nums[1]);
    for (std::size_t i = 2; i < nums.size(); result = gcd(result, nums[i++]));
    return result;
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

/// @brief Computes the least common multiple of a list of integers.
/// @param numbers The list of integers.
/// @return The least common multiple of the integers in `numbers`.
/// @note Throws an exception if the result cannot be converted to
/// `std::common_type_t<T...>`.
template <typename T>
    requires std::integral<T>
constexpr T lcm(std::initializer_list<T> numbers) {
    if (numbers.size() < 2)
        throw std::invalid_argument(
            "abl::lcm takes at least two integer argumnets");

    std::vector<T> nums = numbers;
    T result = lcm(nums[0], nums[1]);
    for (std::size_t i = 2; i < nums.size(); result = lcm(result, nums[i++]));
    return result;
}

/// @brief Computes the greatest common divisor and the Bézout coefficients of
/// two integers.
/// @param a The first integer.
/// @param b The second integer.
/// @return A pair containing the greatest common divisor and the Bézout
/// coefficients.
/// @note Throws an exception if the result cannot be converted to
/// `std::common_type_t<T, U>`.
template <typename T, typename U>
    requires requires { typename std::common_type_t<T, U>; } &&
             std::is_convertible_v<T, std::common_type_t<T, U>> &&
             std::is_convertible_v<U, std::common_type_t<T, U>> &&
             std::integral<std::common_type_t<T, U>> &&
             std::is_signed_v<std::common_type_t<T, U>>
constexpr std::pair<
    std::common_type_t<T, U>,
    std::tuple<std::common_type_t<T, U>, std::common_type_t<T, U>>>
gcdx(const T& a, const U& b) noexcept(
    std::is_nothrow_convertible_v<T, std::common_type_t<T, U>> &&
    std::is_nothrow_convertible_v<U, std::common_type_t<T, U>>) {
    using common_t = std::common_type_t<T, U>;
    using common_ut = std::make_unsigned_t<common_t>;

    common_ut r0 = a < 0 ? (a == std::numeric_limits<common_t>::min()
                                ? static_cast<common_ut>(a)
                                : static_cast<common_ut>(-a))
                         : static_cast<common_ut>(a);

    common_ut r1 = b < 0 ? (b == std::numeric_limits<common_t>::min()
                                ? static_cast<common_ut>(b)
                                : static_cast<common_ut>(-b))
                         : static_cast<common_ut>(b);

    if (r0 == 0) return {static_cast<common_t>(r1), {0, (b < 0 ? -1 : 1)}};
    if (r1 == 0) return {static_cast<common_t>(r0), {(a < 0 ? -1 : 1), 0}};

    common_t s0 = 1, s1 = 0;
    while (r1 != 0) {
        common_ut q = r0 / r1;
        std::swap(r1, r0 -= q * r1);
        std::swap(s1, s0 -= static_cast<common_t>(q) * s1);
    }

    return {static_cast<common_t>(r0),
            {s0 * (a < 0 ? -1 : 1),
             (static_cast<common_t>(r0) - static_cast<common_t>(a) * s0) /
                 static_cast<common_t>(b) * (b < 0 ? -1 : 1)}};
}

/// @brief Computes the greatest common divisor and the Bézout coefficients of
/// two unsigned integers.
/// @param a The first unsigned integer.
/// @param b The second unsigned integer.
/// @return A pair containing the greatest common divisor and the Bézout
/// coefficients.
/// @note Throws an exception if the result cannot be converted to
/// `std::common_type_t<T, U>`.
template <typename T, typename U>
    requires requires { typename std::common_type_t<T, U>; } &&
             std::is_convertible_v<T, std::common_type_t<T, U>> &&
             std::is_convertible_v<U, std::common_type_t<T, U>> &&
             std::integral<std::common_type_t<T, U>> &&
             std::is_unsigned_v<std::common_type_t<T, U>>
constexpr std::pair<std::common_type_t<T, U>,
                    std::tuple<std::make_signed_t<std::common_type_t<T, U>>,
                               std::make_signed_t<std::common_type_t<T, U>>>>
gcdx(const T& a, const U& b) noexcept(
    std::is_nothrow_convertible_v<T, std::common_type_t<T, U>> &&
    std::is_nothrow_convertible_v<U, std::common_type_t<T, U>>) {
    using common_ut = std::common_type_t<T, U>;
    using common_t = std::make_signed_t<common_ut>;

    auto r0 = static_cast<common_ut>(a), r1 = static_cast<common_ut>(b);
    if (r0 == 0) return {r1, {0, 1}};
    if (r1 == 0) return {r0, {1, 0}};

    common_t s0 = 1, s1 = 0;
    common_t t0 = 0, t1 = 1;
    while (r1 != 0) {
        common_ut q = r0 / r1;
        std::swap(r1, r0 -= q * r1);
        std::swap(s1, s0 -= static_cast<common_t>(q) * s1);
        std::swap(t1, t0 -= static_cast<common_t>(q) * t1);
    }

    return {r0, {s0, t0}};
}

/// @brief Computes the greatest common divisor and the Bézout coefficients of a list of signed integers.
/// @param numbers The list of integers.
/// @return A pair containing the greatest common divisor and the Bézout coefficients as `std::vector<T>`.
/// @note Throws an exception if the result cannot be converted to `std::common_type_t<T, U>`.
template <typename T>
    requires std::integral<T> && std::is_signed_v<T>
constexpr std::pair<T, std::vector<T>> gcdx(std::initializer_list<T> numbers) {
    if (numbers.size() < 2)
        throw std::invalid_argument(
            "abl::gcdx takes at least two integer argumnets");

    std::vector<T> coeffs_vec = numbers;

    T d;
    std::tuple<T, T> coeffs;
    std::tie(d, coeffs) = gcdx(coeffs_vec[0], coeffs_vec[1]);

    coeffs_vec[0] = std::get<0>(coeffs);
    coeffs_vec[1] = std::get<1>(coeffs);
    for (std::size_t i = 2; i < numbers.size(); i++) {
        std::tie(d, coeffs) = gcdx(d, coeffs_vec[i]);
        for (std::size_t j = 0; j < i; coeffs_vec[j++] *= std::get<0>(coeffs));
        coeffs_vec[i] = std::get<1>(coeffs);
    }

    return {d, coeffs_vec};
}

/// @brief Computes the greatest common divisor and the Bézout coefficients of a list of signed integers.
/// @param numbers The list of integers.
/// @return A pair containing the greatest common divisor and the Bézout coefficients as `std::vector<T>`.
/// @note Throws an exception if the result cannot be converted to `std::common_type_t<T, U>`.
template <typename... Ts>
    requires(sizeof...(Ts) > 2) &&
            (std::is_convertible_v<Ts, std::common_type_t<Ts...>> && ...) &&
            (std::signed_integral<Ts> && ...)
constexpr std::pair<std::common_type_t<Ts...>,
                    std::vector<std::common_type_t<Ts...>>>
gcdx(Ts&&... numbers) noexcept(
    (std::is_nothrow_convertible_v<Ts, std::common_type_t<Ts...>> && ...)) {
    return gcdx({std::forward<Ts>(numbers)...});
}

}  // namespace abl

#endif  // ATL_NUMERIC_GCD_HPP
