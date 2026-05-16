---
title: "Type Traits"
weight: 1
---

> Some extra type traits utilities.

> [!CAUTION]
> Imports `<type_traits>`.

> [!TIP]
> To import use `#include "atl/utils/type_traits.hpp"`.

## `abl::static_max` and `abl::static_min`

Not a "type trait", but a useful utility for getting the maximum or minimum of multiple values at compile time.

```cpp
#include "atl/utils/type_traits.hpp"

static_assert(abl::static_max<1, 2, 3>::value == 3);
static_assert(abl::static_min<1, 2, 3>::value == 1);
```

Or with `abl::static_max_v` and `abl::static_min_v`

```cpp
static_assert(abl::static_max_v<1, 2, 3> == 3);
static_assert(abl::static_min_v<1, 2, 3> == 1);
```

Empty `abl::static_max` holds `1` and `abl::static_min` holds `0`.

## `abl::are_same` and `abl::are_unique`

`abl::are_same` and `abl::are_unique` are type trait utilities that check if all types are the same or unique, respectively.

```cpp
#include "atl/utils/type_traits.hpp"

static_assert(abl::are_same<int, int, int>::value);
static_assert(!abl::are_same<int, int, float>::value);
static_assert(abl::are_unique<int, float, double>::value);
static_assert(!abl::are_unique<int, int, double>::value);
```

Or with `abl::are_same_v` and `abl::are_unique_v`

```cpp
static_assert(abl::are_same_v<int, int, int>);
static_assert(!abl::are_same_v<int, int, float>);
static_assert(abl::are_unique_v<int, float, double>);
static_assert(!abl::are_unique_v<int, int, double>);
```

## `abl::contains_type`

`abl::contains_type<T, Ts...>` is a type trait utility that checks if a type `T` is contained within a variadic pack of types `Ts...`.

```cpp
#include "atl/utils/type_traits.hpp"

static_assert(abl::contains_type<int, int, float, double>::value);
static_assert(!abl::contains_type<int, float, double>::value);
```

Or with `abl::contains_type_v`

```cpp
static_assert(abl::contains_type_v<int, int, float, double>);
static_assert(!abl::contains_type_v<int, float, double>);
```

{{< cards >}}
  {{< card link="https://github.com/Scurrra/atl/blob/master/include/atl/utils/type_traits.hpp" title="Code" icon="github" >}}
{{< /cards >}}
