---
title: "GCD and LCM"
weight: 1
---

> Given two integers, `a` and `b`, find their greatest common divisor and least common multiple.

I think it's the first complex concept everyone was introduced to at school: GCD helps simply the fraction, LCM helps find the minimal denominator when computing sum of fractions. Mathematically they are defined as: $$\gcd(a, b) = \max \{ d > 0 : (d | a) and (d | b) \},$$ $$lcm(a, b) = (a b) / \gcd(a, b)$$

> [!NOTE]
> This stands for negative integers, too: greatest divisor is always positive, so it's more convenient to just compute absolute values before performing the algorithm.

GCD and LCM are associative functions, what means that $$\gcd(a, b, c) = \gcd(\gcd(a, b), c),$$ $$lcm(a, b, c) = lcm(lcm(a, b), c),$$ 

## Algorithm

> **_Original_**: Subtract the smaller number from the larger until one of them becomes zero. The non-zero one is the GCD of these numbers.

If $d$ divides both $a$ and $b$ it also divides $a - b$ ($a > b$). Well, the original algorithm may be too slow: $b$ will be subtracted from $a$ $\lfloor a / b \rfloor$ times. So the operation is equivalent to: $a - \lfloor a / b \rfloor b = a \mod b $.

> **_Optimized_**: $$\gcd(a, b) = \begin{cases} a, & \text{if}\; b = 0 \\ \gcd(b, a \mod b), & \text{otherwise} \end{cases}$$

## Implementation

### Recursive

```cpp
int gcd_rec(int a, int b) {
    // if (b == 0) return a;
    // return gcd_rec(b, a % b);
    // or simply
    return b == 0 ? a : gcd_rec(b, a % b);
}
```

Recursive algorithms are generally considered bad, so the iterative approach is preferred.

### Iterative

```cpp
int gcd_iter(int a, int b) {
    while (b != 0) {
        a %= b;
        std::swap(a, b);
    }
    return a;
}
```

### Binary GCD

The slow part of the previous implementation is the modulo operation. While arithmetic operations are considered to have _O(1)_ complexity, some of them are actually slower than the others.

Computers use binary representation, so it would be nice to substitute some operations with cheap subtractions (back to the origins!) and shifts. Turns out there are some useful properties of GCD:

1. $\gcd(a, 0) = a$ and $\gcd(0, b) = b$
2. $\gcd(2a, 2b) = 2\gcd(a, b)$
3. $\gcd(2a, b) = \gcd(a, b)$
4. $\gcd(a, b) = \gcd(b, a - b)$

Let's apply these properties:

1. use the 1st property to check corner cases;
2. with the 2nd find the shift for the answer;
3. shift `a` (divide it by the maximum possible power of 2);
4. in a loop shift `b` and swap `(a, b)` with `(b, a - b)`;
5. shift `a` back.

> [!NOTE]
> With C++20 the function `std::countr_zero` from the `<bit>` header can be used to determine the number of consecutive zero bits starting from the least significant bit in an unsigned integer --- maximum power of 2 the number can be divided with.
>
> Previously, compilers provided a built-in function `__builtin_ctz`.

```cpp
int gcd_binary(unsigned int a, unsigned int b) {
    if (!a || !b) return a | b;
    unsigned shift = std::countr_zero(a | b);
    a >>= std::countr_zero(a);
    while (b != 0) {
        b >>= std::countr_zero(b);
        if (a > b) std::swap(a, b);
        b -= a;
    }
    return a << shift;
}
```

## Templated implementation

> [!NOTE]
> Since C++17 we have `std::gcd` and `std::lcm` in the `<numeric>` header, which already can handle arguments of different types.

First of all, the signature: `std::common_type_t<T, U> gcd(const T& a, const U& b)` --- it's obvious that the result should be of the common type of the arguments. But what are the constraints?

1. `std::common_type<T, U>` should exist;
2. `T` and `U` should be (nothrow) convertible to their common type: `std::is_nothrow_convertible_v<T, std::common_type_t<T, U>> && std::is_nothrow_convertible_v<U, std::common_type_t<T, U>>`;
3. the common type should be integral: `td::integral<std::common_type_t<T, U>>`.

Now let's think what the common type is. We have three cases:

1. both `T` and `U` are unsigned -> common type is the largest of them two;
2. one `T` and `U` is signed -> common type is a signed type at least twice as big as the unsigned;
3. both `T` and `U` are signed -> common type is the largest of them two;

As the algorithm should be computed on unsigned integers, the version with signed return type requires three more conversions and possibly negations. So the two overloads of our `gcd` can be distinguished via `std::is_signed_v<std::common_type_t<T, U>>` and `std::is_unsigned_v<std::common_type_t<T, U>>`.

> [!NOTE]
> Inside signed version we have two types: common signed type `common_t` and its unsigned version of the same size `common_ut`.
>
> When converting signed to unsigned we should not forget about the case when the value equals to `std::numeric_limits<common_t>::min()`. In this corner case the value is simply converted, while other negative values should be negotiated. This conversion is guaranteed by two's compliment representation of signed integer types.

> [!WARNING]
> The implementation is biased: LCM of two negative integers is a negative.

```cpp
// std::countr_zero
#include <bit>
// std::integral, std::is_signed, std::is_unsigned
#include <concepts>
// std::numeric_limits
#include <limits>
// std::invalid_argument
#include <stdexcept>  
// std::common_type, std::make_unsigned, 
// std::is_convertible (std::is_nothrow_convertible)
#include <type_traits>

namespace abl {

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

template <typename T1, typename T2, typename T3, typename... Ts>
    requires requires(const T1& a, const T2& b) { gcd(a, b); }
constexpr std::common_type_t<T1, T2, T3, Ts...> gcd(const T1& a, const T2& b,
                                                    const T3& c,
                                                    const Ts&... rest) {
    return gcd(gcd(a, b), c, rest...);
}

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

template <typename T1, typename T2, typename T3, typename... Ts>
    requires requires(const T1& a, const T2& b) { lcm(a, b); }
constexpr std::common_type_t<T1, T2, T3, Ts...> lcm(const T1& a, const T2& b,
                                                    const T3& c,
                                                    const Ts&... rest) {
    return lcm(lcm(a, b), c, rest...);
}

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

}  // namespace abl
```

{{< cards >}}
  {{< card link="https://scurra.github.io/atl/docs/numeric/gcd/" title="Docs" icon="globe" >}}
  {{< card link="https://godbolt.org/z/T1z59WPKK" title="Godbolt example" icon="globe" >}}
{{< /cards >}}

## Resources

1. [Euclidean algorithm for computing the greatest common divisor (AfCP)](https://cp-algorithms.com/algebra/euclid-algorithm.html)
2. [Алгоритм Евклида (Algorithmica)](https://ru.algorithmica.org/cs/modular/euclid/)
3. [Binary GCD (Algorithmica / HPC)](https://en.algorithmica.org/hpc/algorithms/gcd/)
