---
title: "Extended GCD"
weight: 2
---

> Given two integers, $a$ and $b$, find their greatest common divisor and, and such coefficients $x$ and $y$, that $$a x + b y = \gcd(a, b)$$

By [Bézout's identity](https://en.wikipedia.org/wiki/B%C3%A9zout%27s_identity) such a representation exists.

> The algorithm easily expands to any number of variables $a_1, a_2, a_3, \dots, a_n, n \geq 2$. First, we find the GCD $d_1$ and coefficients $x_1$ and $x_2$ of $a_1$ and $a_2$, then we compute the GCD $d_2$ and coefficients $x_2'$ and $x_3$ of $d_1$ and $a_3$ and update $x_1 = x_1 * x_2'$ and $x_2 = x_2 * x_2'$, and so on always updating $x_1, \dots, x_{i-1}$ after computing $x_i$ for $a_i$.

## Algorithm

Let's recall a property of GCD: $\gcd(a, 0) = a$. This gives us that $a * 1 + 0 * 0 = \gcd(a, 0)$. So, starting with coefficients $(x, y) = (1, 0)$ we can go backwards up the recursive calls.

So, on forward pass from $$a x + b y = \gcd(a, b),$$ assuming $a > b$, we have $$b x' + (a \;\%\; b) y' = b x' + (a - \lfloor a / b \rfloor) y' = \gcd(b, a \% b)$$

This gives us $$a y' + b (x' - y' \lfloor a / b \rfloor) = \gcd(a, b)$$

And the original coefficients are $$\begin{cases} x = y' \\ y = x' - y' \lfloor a / b \rfloor \end{cases}$$

## Implementation

Well, recursive algorithm requires recursion. The standard approach in this case is passing $a$ and $b$ the way you like, and local creation of $x$ and $y$ and passing them by reference.

```cpp
// code from AfCP
int gcdx(int a, int b, int& x, int& y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }
    int x1, y1;
    int d = gcd(b, a % b, x1, y1);
    x = y1;
    y = x1 - y1 * (a / b);
    return d;
}
```

Personally, I prefer the approach of using a recursive lambda function inside the function itself. To a lambda recursively we need either specify its type with `std::function`:

```cpp
// `std::function` version
std::pair<int, std::pair<int, int>> gcdx(int a, int b) {
    if (b == 0) {
        return {a, {1, 0}};
    }

    std::function<int(int, int, int&, int&)> gcdx_recursive = [&gcdx_recursive](int a, int b, int& x, int& y) -> int {
        if (b == 0) {
            x = 1;
            y = 0;
            return a;
        }

        int x1, y1;
        int d = gcdx_recursive(b, a % b, x1, y1);
        x = y1;
        y = x1 - y1 * (a / b);
        return d;
    };

    int x, y;
    int d = gcdx_recursive(a, b, x, y);
    return {d, {x, y}};
}
```

or pass this lambda in itself as a parameter:

```cpp
std::pair<int, std::pair<int, int>> gcdx(int a, int b) {
    if (b == 0) {
        return {a, {1, 0}};
    }

    auto gcdx_recursive = [](auto self, int a, int b, int& x, int& y) -> int {
        if (b == 0) {
            x = 1;
            y = 0;
            return a;
        }

        int x1, y1;
        int d = self(self, b, a % b, x1, y1);
        x = y1;
        y = x1 - y1 * (a / b);
        return d;
    };

    int x, y;
    int d = gcdx_recursive(gcdx_recursive, a, b, x, y);
    return {d, {x, y}};
}
```

## Iterative algorithm

Well, let's remember the property $$\gcd(a, b) = \gcd(b, a \% b)$$. Let's denote $a \% b = r_2$ (the index will play its role later): $$\gcd(a, b) = \gcd(b, r_2) = \gcd(r_2, b \% r_2),$$ with $b \% r_2 = r_3$, $$\gcd(r_2, r_3) = \gcd(r_3, r_2 \% r_3),$$ with $r_2 \% r_3 = r_4$, and so on.

The chain stops when $r_n = r_{n-2} \% r_{n-1} = 0$, and the last non-zero remainder, $r_{n-1}$, is the GCD. Start of the algorithm is a bit..., it does not start with $r_i$. From the substitutions we see, that $r_1 = b$ and $r_0 = a$.

$$r_i = r_{i-2} \% r_{i-1}$$ can be rewritten as $$r_i = r_{i - 2} - q_i r_{i - 1},$$ where $q_i = \lfloor r_{i-2} / r_{i-1} \rfloor$ for $i > 1$. Each such equation is [Euclidean division](https://en.wikipedia.org/wiki/Euclidean_division), or division with remainder.

Now we need to do something with the coefficients $x$ and $y$. We know, that $x$ and $y$ satisfy the equation $$ax + by = \gcd(a, b) = r_{n-1}$$ So let's define $r_i = a s_i + b t_i$, where $s_i$ and $t_i$ are the coefficients of $x$ and $y$ in the $i$-th step of the algorithm. With this representation $x = s_{n-1}$ and $y = t_{n-1}$.

Back to the division chain: $$r_2 = r_0 - q_2 r_1 = a s_0 + b t_0 - q_2 (a s_1 + b t_1) =$$ $$= a (s_0 - q_2 s_1) + b (t_0 - q_2 t_1) \Rightarrow$$ $$\Rightarrow \begin{cases} s_2 = s_0 - q_2 s_1, \\ t_2 = t_0 - q_2 t_1\end{cases}$$ $$r_3 = r_1 - q_3 r_2 = a s_1 + b t_1 - q_3 (a s_2 + b t_2) =$$ $$= a (s_1 - q_3 s_2) + b (t_1 - q_3 t_2) \Rightarrow$$ $$\Rightarrow \begin{cases} s_3 = s_1 - q_3 s_2, \\ t_3 = t_1 - q_3 t_2\end{cases}$$ $$\dots$$

Now we see the relationship between $s_i$ and $t_i$: $$\begin{cases} s_i = s_{i-2} - q_i s_{i-1}, \\ t_i = t_{i-2} - q_i t_{i-1}\end{cases}, \;\text{where } q_i = \frac{r_{i-2}}{r_{i-1}}$$

Where $s_i$ and $t_i$ sequences start with $$\begin{cases} r_0 = a s_0 + b t_0 = a, \\ r_1 = b s_1 + b t_1 = b\end{cases} \Rightarrow$$ $$\Rightarrow \begin{cases} s_0 = 1, t_0 = 0, \\ s_1 = 0, t_1 = 1\end{cases}.$$

The algorithm is to just compute sequences $r_i$, $s_i$ and $t_i$ using the recurrence relations and initial conditions above, until $r_i = 0$. But wait, $$a s_{n-1} + b t_{n-1} = r_{n-1} = \gcd(a, b) = a x + b y,$$ where the sequence $r_i$ is needed to compute $s_i$ and $t_i$, which are independent. So one of the sequences $s_i$ or $t_i$ can be dropped and computed from the other when the algorithm ends: $$y = \frac{\gcd(a, b) - a x}{b} = \frac{r_{n-1} - a s_{n-1}}{b} $$

```cpp
std::pair<int, std::pair<int, int>> gcdx_it(int a, int b) {
    auto r0 = a, r1 = b;
    auto s0 = 1, s1 = 0;
    
    while (r1 != 0) {
        auto q = r0 / r1;
        std::swap(r1, r0 -= q * r1);
        std::swap(s1, s0 -= q * s1);
    }

    return {r0, {s0, (r0 - a * s0) / b}};
}
```

## Templated implementation

What are the types? The analysis is pretty similar to those of the GCD function, but in extended version we also need to analyze the types of the coefficients $x$ and $y$. 

Like the GCD function, the extended Euclidean algorithm is defined for unsigned integers, but it can be generalized to any integral type of arguments. Basically, we have two positive integers $a$ and $b$, their GCD is definitely smaller than $a$ and $b$ (or equal to $a$ or $b$), so one of the coefficients $x$ or $y$ is always negative. In signed case, the signs of $a$ and $b$ are simply transferred to the coefficients $x$ and $y$. So, the type of the GCD is the common type of $a$ and $b$, and the type of the coefficients $x$ and $y$ is its signed version.

> [!NOTE]
> The version of the function that takes only unsigned integers is needed due to the fact that $y$ cannot be computed afterward properly, so it should be computed through the sequence.

> [!WARNING]
> `abl::gcdx` of two arguments returns the coefficients as a `std::tuple`, while `abl::gcdx` of three arguments or of `std::initializer_list` returns the coefficients as a `std::vector`. It will be changed in the future.

```cpp
// std::integral, std::is_signed, std::is_unsigned
#include <concepts>
// std::numeric_limits
#include <limits>
// std::common_type, std::make_signed, std::make_unsigned,
// std::is_convertible (std::is_nothrow_convertible)
#include <type_traits>
// std::invalid_argument
#include <stdexcept>
// std::swap
#include <utility>

#include <tuple>
#include <vector>

namespace abl {

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
```

{{< cards >}}
{{< card link="https://scurra.github.io/atl/docs/numeric/gcd/" title="Docs" icon="globe" >}}
{{< card link="https://godbolt.org/z/G89bx1ErM" title="Godbolt example" icon="globe" >}}
{{< /cards >}}

## Resources

1. [Extended Euclidean Algorithm (AfCP)](https://cp-algorithms.com/algebra/extended-euclid-algorithm.html)
2. [Расширенный алгоритм Евклида
   (Algorithmica)](https://ru.algorithmica.org/cs/modular/extended-euclid/)
