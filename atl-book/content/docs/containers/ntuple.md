---
title: "NTuple"
weight: 1
---

> C++ does not have a built-in tuple-like container that can store a fixed-size tuple of values of the same type. This behavior can be achieved via manual typing `std::tuple<T, T, T, ...>` where `T` is the type of the values or via `std::array<T, N>` where `N` is the size of the tuple. I insist that `std::array<T, N>` is not semantically the same, so I wrote a custom `abl::ntuple` container that works the same way as `abl::tuple`.

> [!CAUTION]
> Imports `<concepts>`, `<type_traits>`, `<utility>` and `<cstdlib>`.

> [!TIP]
> To import use `#include "atl/containers/ntuple.hpp"`.

{{< cards >}}
  {{< card link="https://github.com/Scurrra/atl/blob/master/include/atl/containers/ntuple.hpp" title="Code" icon="github" >}}
  {{< card link="https://godbolt.org/z/3v7WT3vE6" title="Godbolt example" icon="globe" >}}
{{< /cards >}}
