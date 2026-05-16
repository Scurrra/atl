---
title: "Variant"
weight: 3
---

> Type safe sum type (union), where all types of the alternatives are distinct.

> [!CAUTION]
> Imports `<concepts>` and `<type_traits>`.

> [!TIP]
> To import use `#include "atl/types/variant.hpp"`.

### Implementation details

`abl::variant` is implemented using a recursive union type and holds an index of the current alternative as `std::size_t`. Because of a large type for holding the alternative index, the size of the variant is at least 16 bytes compares to 8 bytes of a regular `std::variant`.

## Constructors

`abl::variant` provides the following constructors:
- `abl::variant()`: Default constructor, holds no value.
- `abl::variant(const T& value)`: Constructs with a value of type `T`.
- `abl::variant(T&& value)`: Constructs with a value of type `T` using move semantics.
- `abl::variant(const abl::variant& other)`: Copy constructor.
- `abl::variant(abl::variant&& other)`: Move constructor.
- `abl::variant(const abl::variant<Us...>& other)`: Copy constructor from the variant of type `Us...`, where `Us...` $\subseteq$ `Ts...`.
- `abl::variant(abl::variant<Us...>&& other)`: Move constructor from the variant of type `Us...`, where `Us...` $\subseteq$ `Ts...`.

## Assignment operators

`abl::variant` provides the following assignment operators:
- `abl::variant& operator=(const T& data)`: Assigns a value of type `T` using copy semantics.
- `abl::variant& operator=(T&& data)`: Assigns a value of type `T` using move semantics.
- `abl::variant& operator=(const abl::variant& other)`: Assigns a value from another variant using copy semantics.
- `abl::variant& operator=(abl::variant&& other)`: Assigns a value from another variant using move semantics.
- `abl::variant& operator=(const abl::variant<Us...>& other)`: Assigns a value from the variant of type `Us...`, where `Us...` $\subseteq$ `Ts...` using copy semantics.
- `abl::variant& operator=(abl::variant<Us...>&& other)`: Assigns a value from the variant of type `Us...`, where `Us...` $\subseteq$ `Ts...` using move semantics.

## Modifiers

`abl::variant` provides the following modifiers:
- `void swap(abl::variant& other)`: Swaps the contents of this variant with another variant.
- `T& emplace<T>(Args&&... args)`: Constructs a value of type `T` in-place using the provided arguments.
- `T& emplace<I>(Args&&... args)`: Constructs a value of type `Ts[I]` in-place using the provided arguments.
- `void reset()`: Resets the `variant` to its default state.

## Accessors

`abl::variant` provides the following accessors:

- `T& get<I>() &`: Returns a reference to the value of type `Ts[I]`.
- `const T& get<I>() const &`: Returns a const reference to the value of type `Ts[I]`.
- `T&& get<I>() &&`: Returns a rvalue reference to the value of type `Ts[I]`.
- `const T&& get<I>() const &&`: Returns a const rvalue reference to the value of type `Ts[I]`.
- `T& get<T>() &`: Returns a reference to the value of type `T`.
- `const T& get<T>() const &`: Returns a const reference to the value of type `T`.
- `T&& get<T>() &&`: Returns a rvalue reference to the value of type `T`.
- `const T&& get<T>() const &&`: Returns a const rvalue reference to the value of type `T`.

> [!NOTE]
> These are the methods available for accessing the value of the `variant`, i.e. they can be called as `v.get<T>()` and `v.get<I>()`.

## Observers

`abl::variant` provides the following observers:
- `bool holds_alternative<T>()`: Returns `true` if the variant holds a value of type `T`.
- `std::size_t index()`: Returns the index of the currently held alternative.
- `operator bool()`: Returns `true` if the variant holds a value.

## Non-member functions

`abl::variant` provides the following non-member functions:
- `bool holds_alternative<T>(const variant& v)`: Returns `true` if the variant `v` holds a value of type `T`.
- `void swap(variant& a, variant& b)`: Swaps the contents of `a` and `b`.
- `T& get<T>(variant& v)`: Returns a reference to the value of type `T` in `v`.
- `const T& get<T>(const variant& v)`: Returns a const reference to the value of type `T` in `v`.
- `T&& get<T>(variant&& v)`: Returns a rvalue reference to the value of type `T` in `v`.
- `const T&& get<T>(const variant&& v)`: Returns a const rvalue reference to the value of type `T`.
- `T& get<I>(variant& v)`: Returns a reference to the value of type `Ts[I]` in `v`.
- `const T& get<I>(const variant& v)`: Returns a const reference to the value of type `Ts[I]` in `v`.
- `T&& get<I>(variant&& v)`: Returns a rvalue reference to the value of type `Ts[I]` in `v`.
- `const T&& get<I>(const variant&& v)`: Returns a const rvalue reference to the value of type `Ts[I]`.

> [!NOTE]
> Thus, `abl::variant` provides accessors in both standard functional style and as methods. Check out the examples below.

{{< cards >}}
  {{< card link="https://github.com/Scurrra/atl/blob/master/include/atl/types/variant.hpp" title="Code" icon="github" >}}
  {{< card link="https://godbolt.org/z/9sczMs6n1" title="Godbolt example" icon="globe" >}}
{{< /cards >}}
