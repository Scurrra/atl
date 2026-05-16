---
title: "Tuple"
weight: 2
---

> C++ `std::tuple` manually implemented.

> [!CAUTION]
> Imports `<concepts>`, `<type_traits>`, `<utility>` and `<cstdlib>`.

> [!TIP]
> To import use `#include "atl/containers/tuple.hpp"`.

### Implementation details

The `abl::tuple` container is implemented as a wrapper around a proxy struct that inherits from a pack of leaf structs, each of which holds a single value of the tuple. This makes the container occupy consecutive memory locations, allowing for efficient indexing and iteration via simple casts.

## Constructors

`abl::tuple` provides the following constructors:
- `abl::tuple()`: default constructor, initializes a tuple with default-constructed values.
- `abl::tuple(Args&&...)`: initializes the tuple with the given values.
- `abl::tuple(const abl::tuple&)`: copy constructor, copies the tuple.
- `abl::tuple(abl::tuple&&)`: move constructor, transfers ownership of the tuple.

## Assignment operators

`abl::tuple` provides the following assignment operators:
- `abl::tuple& operator=(const abl::tuple&)`: copy assignment operator, copies the tuple.
- `abl::tuple& operator=(abl::tuple&&)`: move assignment operator, transfers ownership of the tuple.

## Modifiers

`abl::tuple` provides the following modifiers:
- `void swap(abl::tuple&)`: swaps the contents of the tuple with another tuple.

## Accessors

`abl::tuple` provides the following accessors:
- `T& get<I>() &`: returns a reference to the value at the given index `I`.
- `const T& get<I>() const &`: returns a const reference to the value at the given index `I`.
- `T&& get<I>() &&`: returns a rvalue reference to the value at the given index `I`.
- `const T&& get<I>() const &&`: returns a const rvalue reference to the value at the given index `I`.

## Observers

`abl::tuple` provides the following observers:
- `static std::size_t size` returns the number of elements in the tuple.

## Comparison operators

`abl::tuple` provides the following comparison operators:
- `bool operator==(const abl::tuple&)`: returns `true` if the tuples are equal.
- `bool operator<=>(abl::tuple const&)`: a three-way comparison operator that returns `0` if the tuples are equal, `1` if the first tuple is greater, and `-1` if the second tuple is greater.

## Non-member functions

`abl::tuple` provides the following non-member functions:
- `void swap(abl::tuple&, abl::tuple&)`: swaps the contents of two tuples.
- `T& get<I>(abl::tuple&)`: returns a reference to the value at the given index `I`.
- `const T& get<I>(const abl::tuple&)`: returns a const reference to the value at the given index `I`.
- `T&& get<I>(abl::tuple&&)`: returns a rvalue reference to the value at the given index `I`.
- `const T&& get<I>(const abl::tuple&&)`: returns a const rvalue reference to the value at the given index `I`.

> [!NOTE]
> Thus, `abl::tuple` provides accessors in both standard functional style and as methods. Additionally, `abl::tuple` implements the tuple-like interface, i.e. it allows unpacking. Check out the examples below.

{{< cards >}}
  {{< card link="https://github.com/Scurrra/atl/blob/master/include/atl/containers/tuple.hpp" title="Code" icon="github" >}}
  {{< card link="https://godbolt.org/z/ffeGW8xf1" title="Godbolt example" icon="globe" >}}
{{< /cards >}}
