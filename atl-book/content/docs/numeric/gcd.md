---
title: "GCD, LCM, and Extended GCD"
weight: 1
---

> Greatest Common Divisor and Least Common Multiple.

> [!CAUTION]
> Imports `<bit>`, `<concepts>`, `<limits>`, `<type_traits>`, `<stdexcept>`, and `<utility>`. For now, also `<tuple>` and `<vector>`.

> [!TIP]
> To import use `#include "atl/numeric/gcd.hpp"`.

## `abl::gcd` and `abl::lcm`

> Compute GCD and LCM of two or more numbers using binary GCD algorithm. The resulting type is the common type of the function's argument's types.

> [!NOTE]
> Functions are `constexpr` and conditionally `noexcept` (throw on failed type conversion).

### Usage

Both `abl::gcd` and `abl::lcm` take at least two arguments.

```cpp
abl::gcd(a, b) // computes gcd(a, b)
abl::gcd(a, b, c) // computes gcd(gcd(a, b), c)
abl::gcd({a, b, c}) // computes gcd(gcd(a, b), c) iteratively
// and so on
```

> [!WARNING]
> LCM of two negative numbers (as of a negative and positive) is a negative number, to be really "the least".

```cpp
abl::lcm(a, b) // computes lcm(a, b)
abl::lcm(a, b, c) // computes lcm(gcd(a, b), c)
abl::lcm({a, b, c}) // computes lcm(gcd(a, b), c) iteratively
// and so on
```

{{< cards >}}
  {{< card link="https://scurrra.github.io/atl/book/math/algebra/gcd_n_lcm/" title="Book article" icon="library" >}}
  {{< card link="https://godbolt.org/z/T1z59WPKK" title="Godbolt example" icon="globe" >}}
{{< /cards >}}

## `abl::gcdx`

> Computes the greatest common divisor and the Bézout coefficients of a list of signed integers.

> [!NOTE]
> Functions are `constexpr` and conditionally `noexcept` (throw on failed type conversion).

### Usage

`abl::gcdx` takes two or more integer arguments. When called with two arguments, returns Bézout coefficients as `std::tuple`, otherwise as `std::vector`.

```cpp
auto [d1, coeffs1] = abl::gcdx(a, b);
auto [d2, coeffs2] = abl::gcdx(a, b, c);
auto [d3, coeffs3] = abl::gcdx({a, b, c});
```

{{< cards >}}
  {{< card link="https://scurrra.github.io/atl/book/math/algebra/gcdx/" title="Book article" icon="library" >}}
  {{< card link="https://godbolt.org/z/G89bx1ErM" title="Godbolt example" icon="globe" >}}
{{< /cards >}}

{{< cards >}}
  {{< card link="https://github.com/Scurrra/atl/blob/master/include/atl/numeric/gcd.hpp" title="Code" icon="github" >}}
  {{< card link="https://godbolt.org/z/5r3x3rP7b" title="Godbolt full example" icon="globe" >}}
{{< /cards >}}
