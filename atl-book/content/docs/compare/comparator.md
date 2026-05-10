---
title: "Comparator"
weight: 1
---

> In C++ tradition, the comparison condition is a predicate (a function that takes two arguments and returns `true` or `false` as a result of their comparison; satisfies the `std::predicate` concept), while in Python you may have seen that `sort` uses a key function to compare elements. I decided to mix these two approaches and create a `abl::comparator` struct that can be used to compare elements by both key and predicate at the same time. Anyway, this still satisfies the `std::predicate` concept as it is called with two arguments.

> [!CAUTION]
> Imports `<concepts>`, `<type_traits>`, `<functional>` and `<utility>`.

> [!TIP]
> To import use `#include "atl/compare/comparator.hpp"`.

## `abl::comparator`

The `abl::comparator` struct is simply an abstraction around a key function and a predicate. It can be constructed with one of them or both. The only aim is style.

### Constructing

`abl::comparator` can be constructed with one of the following:

- a default constructor, `std::identity` as a key and a `std::less` predicate: `abl::comparator<int>()`;
- a constructor with both a key function and a predicate, a full template should be specified:

```cpp
auto square = [](const auto& a) { return a * a; };
auto leq = [](const auto& a, const auto& b) { return a <= b; };
auto comp = abl::comparator<int, decltype(square), decltype(leq)>(square, l);
```
- a factory member function `abl::comparator<T>::make` with one or both of the key function and predicate specified:

```cpp
auto comp1 = abl::comparator<int>::make(square, leq);
auto comp2 = abl::comparator<int>::make(square);
auto comp3 = abl::comparator<int>::make(leq);
```

> I personally find this syntax more readable than C++'s `std::make_pair` and Co.

{{< cards >}}
{{< card link="https://github.com/Scurrra/atl/blob/master/include/atl/compare/comparator.hpp" title="Code" icon="github" >}}
{{< card link="https://godbolt.org/z/G93d9scWo" title="Godbolt example" icon="globe" >}}
{{< /cards >}}
