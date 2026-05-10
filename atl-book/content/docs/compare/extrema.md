---
title: "Extrema"
weight: 2
---

> Min/max functions that can compare values of different types.

> [!CAUTION]
> Imports `<concepts>`, `<type_traits>` and `<utility>`.

> [!TIP]
> To import use `#include "atl/compare/extrema.hpp"`.

## `abl::min` and `abl::max`

If the types of the arguments are the same, the function returns constant reference to the smaller/larger value. Otherwise, the function returns the smaller/larger value by value of the common type.

```cpp
abl::min(a, b);
abl::max(a, b);
```

## `abl::minmax`

Returns a pair of the smaller and larger values. If the types of the arguments are different, both elements in the pair are converted to the common type.

```cpp
abl::minmax(a, b);
```

## With a custom comparator

You can also use a custom comparator function to compare the values.

> [!NOTE]
> `a` is considered "better" in terms of the function if the comparator returns `true` when comparing `a` and `b`, i.e. the result of calling `abl::min` and `abl::max` with the same comparator is the same.

```cpp
abl::min(a, b, [](auto a, auto b) { return a < b; });
abl::max(a, b, [](auto a, auto b) { return a > b; });
abl::minmax(a, b, [](auto a, auto b) { return a < b; });
```

{{< cards >}}
  {{< card link="https://github.com/Scurrra/atl/blob/master/include/atl/compare/extrema.hpp" title="Code" icon="github" >}}
  {{< card link="https://godbolt.org/z/YfdMhMqfr" title="Godbolt example" icon="globe" >}}
{{< /cards >}}
