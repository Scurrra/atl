---
title: "Concepts"
weight: 2
---

> Some extra concepts.

> [!CAUTION]
> Imports `<concepts>` and `<type_traits>` via `#include "atl/utils/type_traits.hpp"`.

> [!TIP]
> To import use `#include "atl/concepts/concepts.hpp"`.

## `abl::all_same` and `abl::all_unique`

> `abl::all_same` and `abl::all_unique` are concept constraints that check if all types in a type pack are the same or unique, respectively.

```cpp
#include "atl/utils/type_traits.hpp"

template <typename... Ts>
concept all_same = are_same<Ts...>::value;

template <typename... Ts>
concept all_unique = are_unique<Ts...>::value;
```

## `abl::contains`

> `abl::contains` is a concept constraint that checks if a type is contained in a type pack.

```cpp
#include "atl/utils/type_traits.hpp"

template <typename T, typename... Ts>
concept contains = (std::is_same_v<T, Ts> || ...);
```

{{< cards >}}
  {{< card link="https://github.com/Scurrra/atl/blob/master/include/atl/utils/concepts.hpp" title="Code" icon="github" >}}
{{< /cards >}}
