---
title: "NTuple"
weight: 1
---

> C++ does not have a built-in tuple-like container that can store a fixed-size tuple of values of the same type. This behavior can be achieved via manual typing `std::tuple<T, T, T, ...>` where `T` is the type of the values or via `std::array<T, N>` where `N` is the size of the tuple. I insist that `std::array<T, N>` is not semantically the same, so I wrote a custom `abl::ntuple` container that works the same way as `abl::tuple`.

> [!CAUTION]
> Imports `<concepts>`, `<type_traits>`, `<utility>` and `<cstdlib>`.

> [!TIP]
> To import use `#include "atl/containers/ntuple.hpp"`.

### Implementation details

The `abl::ntuple` container is implemented as a wrapper around a proxy struct that inherits from a pack of leaf structs, each of which holds a single value of the tuple. This makes the container occupy consecutive memory locations, allowing for efficient indexing and iteration via simple casts.

## Constructors

`abl::ntuple` provides the following constructors:
- `abl::ntuple()`: default constructor, initializes a tuple with default-constructed values.
- `abl::ntuple(Args&&...)`: initializes the tuple with the given values.
- `abl::ntuple(abl::ntuple const&)`: copy constructor, copies the tuple.
- `abl::ntuple(abl::ntuple&&)`: move constructor, transfers ownership of the tuple.

## Assignment operators

`abl::ntuple` provides the following assignment operators:
- `abl::ntuple& operator=(abl::ntuple const&)`: copy assignment operator, copies the tuple.
- `abl::ntuple& operator=(abl::ntuple&&)`: move assignment operator, transfers ownership of the tuple.

## Modifiers

`abl::ntuple` provides the following modifiers:
- `void swap(abl::ntuple&)`: swaps the contents of the tuple with another tuple.

## Accessors

`abl::ntuple` provides the following accessors:
- `T& get<I>() &`: returns a reference to the value at the given index `I`.
- `T const& get<I>() const &`: returns a const reference to the value at the given index `I`.
- `T&& get<I>() &&`: returns a rvalue reference to the value at the given index `I`.
- `T const&& get<I>() const &&`: returns a const rvalue reference to the value at the given index `I`.

## Observers

`abl::ntuple` provides the following observers:
- `static std::size_t size` returns the number of elements in the tuple.

## Comparison operators

`abl::ntuple` provides the following comparison operators:
- `bool operator==(abl::ntuple const&)`: returns `true` if the tuples are equal, `false` otherwise.
- `bool operator<=>(abl::ntuple const&)`: a three-way comparison operator that returns `0` if the tuples are equal, `1` if the first tuple is greater, and `-1` if the second tuple is greater.

## Non-member functions

`abl::ntuple` provides the following non-member functions:
- `void swap(abl::ntuple&, abl::ntuple&)`: swaps the contents of two tuples.
- `T& get<I>(abl::ntuple&)`: returns a reference to the value at the given index `I`.
- `T const& get<I>(abl::ntuple const&)`: returns a const reference to the value at the given index `I`.
- `T&& get<I>(abl::ntuple&&)`: returns a rvalue reference to the value at the given index `I`.
- `T const&& get<I>(abl::ntuple const&&)`: returns a const rvalue reference to the value at the given index `I`.

> [!NOTE]
> Thus, `abl::ntuple` provides accessors in both standard functional style and as methods. Additionally, `abl::ntuple` implements the tuple-like interface, i.e. it allows unpacking. Check out the examples below.

{{< cards >}}
  {{< card link="https://github.com/Scurrra/atl/blob/master/include/atl/containers/ntuple.hpp" title="Code" icon="github" >}}
  {{< card link="https://godbolt.org/z/PYbxbbfEe" title="Godbolt example" icon="globe" >}}
{{< /cards >}}
