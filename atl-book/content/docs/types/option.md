---
title: "Option"
weight: 2
---

> Union-based implementation of an option type.

> [!CAUTION]
> Imports `<exception>` and `<memory>`.

> [!TIP]
> To import use `#include "atl/types/option.hpp"`.

## Constructors

`abl::option` provides the following constructors:
- `abl::option()`: Default constructor, initializes with an empty state.
- `abl::option(const T& value)`: Constructs with a value.
- `abl::option(T&& value)`: Constructs with a value, moving it.
- `abl::option(const abl::option& value)`: Copy constructor.
- `abl::option(abl::option&& value)`: Move constructor.


## Assignment operators

`abl::option` provides the following assignment operators:
- `abl::option& operator=(const T& value)`: Assigns a value.
- `abl::option& operator=(T&& value)`: Assigns a value, moving it.
- `abl::option& operator=(const abl::option& value)`: Copy assignment.
- `abl::option& operator=(abl::option&& value)`: Move assignment.

## Modifiers

`abl::option` provides the following modifiers:
- `void swap(abl::option& other)`: Swaps the contents with another `abl::option`.
- `T& abl::emplace(Args&&...)`: Constructs the value in-place.
- `void reset()`: Resets the option to an empty state.

## Accessors

`abl::option` provides the following accessors:
- `T& operator*()`: Returns a reference to the value.
- `const T& operator*()`: Returns a const reference to the value.
- `T&& operator*()`: Returns a rvalue reference to the value.
- `const T&& operator*()`: Returns a const rvalue reference to the value.
- `T* operator->()`: Returns a pointer to the value.
- `const T* operator->()`: Returns a const pointer to the value.
- `T& value()`: Returns a reference to the value.
- `const T& value()`: Returns a const reference to the value.
- `T&& value()`: Returns a rvalue reference to the value.
- `const T&& value()`: Returns a const rvalue reference to the value.
- `T value_or(U&& default_value) const&`: Returns the value if present, otherwise returns the default value.
- `T value_or(U&& default_value) &&`: Returns the value if present, otherwise returns the default value.

> [!NOTE]
> On attempt to access the value of an empty `abl::option` via `value()` methods, a `abl::bad_option_access` exception is thrown.

## Observers

`abl::option` provides the following observers:
- `bool has_value() const`: Returns `true` if the option has a value, `false` otherwise.
- `explicit operator bool() const`: Returns `true` if the option has a value, `false` otherwise.

{{< cards >}}
  {{< card link="https://github.com/Scurrra/atl/blob/master/include/atl/types/option.hpp" title="Code" icon="github" >}}
  {{< card link="https://godbolt.org/z/Tr9rveE8r" title="Godbolt example" icon="globe" >}}
{{< /cards >}}
