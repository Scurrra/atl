---
title: "GCD, LCM, and Extended GCD"
weight: 1
draft: true
---

> Greatest Common Divisor and Least Common Multiple.

> [!CAUTION]
> Imports `<bit>`, `<concepts>`, `<limits>`, and `<type_traits>`.

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
// and so on
```

> [!WARNING]
> LCM of two negative numbers (as of a negative and positive) is a negative number, to be really "the least".

```cpp
abl::lcm(a, b) // computes lcm(a, b)
abl::lcm(a, b, c) // computes lcm(gcd(a, b), c)
// and so on
```

{{< cards >}}
  {{< card link="https://scurra.github.io/atl/book/math/algebra/gcd_n_lcm/" title="Book article" icon="library" >}}
  {{< card link="https://godbolt.org/z/E1TPKPq41" title="Godbolt example" icon="globe" >}}
{{< /cards >}}



{{< cards >}}
  {{< card link="https://github.com/Scurrra/atl/blob/master/include/atl/numeric/gcd.hpp" title="Code" icon="github" >}}
  {{< card link="https://godbolt.org/z/E1TPKPq41" title="Godbolt example" icon="globe" >}}
{{< /cards >}}
