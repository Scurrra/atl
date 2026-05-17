---
title: "Option"
weight: 1
---

> Option (Rust's `Option`, C++'s `std::optional`, Haskell's `Maybe`) is a wrapper type that represents either a value or nothing. It's the simplest sum type, basically.

We are going to construct a struct that can hold a value or nothing and easily switch between the two states. This means that we should decouple the [lifetime](https://en.cppreference.com/cpp/language/lifetime) of the value from the `option` struct itself.

> [!NOTE]
> I hate C++ for its broken naming and love for its flexibility. Anyway, if use a contained type itself, the field most certainly will be constructed and destructed automatically. Do not try to understand first time.

There are three ways to do so:
1. Using a pointer to contained type. In this case, the memory will be allocated... somewhere else. So it's not a good option.
2. Using a raw inline storage `alignas(T) char storage[sizeof(T)]`. This requires pointer laundering and `reinterpret_cast`, which is not constexpr safe.
3. Using a union with a single field of type `T`. This is the most straightforward and safe option.

All three require holding a state that indicates whether the option contains a value or nothing. The state is usually a boolean flag or an enum value (in our example). We will discuss the later two options in more detail.

## Raw storage

As I said, we need to hold a state that indicates whether the option contains a value or nothing. We will use a `enum class`, that represents two alternatives: `some` and `none`:

```cpp
enum class state : unsigned char { none = 0, some = 1 };
```

So, the meaningful fields of the `option` type are:

```cpp
alignas(T) unsigned char storage[sizeof(T)];
state state = state::none;
```

> [!NOTE]
> I'm not going to explain raw storage in detail. This pattern is used in things like stateful allocators, where the stored type might not be known. Anyway, it's not the topic.

But here are some useful functions and technics you have to use with it:
- `std::construct_at` used to construct the value in-place, basically calls a relevant constructor
- `std::destroy_at` used to destroy the value in-place, basically calls the destructor
- `reinterpret_cast` used to reinterpret the storage as a `T*` pointer
- `std::launder` used to ["launder"](https://en.cppreference.com/cpp/utility/launder#notes) the pointer

So, the three basic operations are:

- Construction

```cpp
std::construct_at(reinterpret_cast<T*>(storage), );
```

- Destruction

```cpp
std::destroy_at(reinterpret_cast<T*>(storage));
```

- Access

```cpp
T value = *std::launder(reinterpret_cast<T*>(storage));
const T const_value = *std::launder(reinterpret_cast<const T*>(storage));
```

## Union storage

Union type can hold many alternative types, but only one at a time. And the size of the union is the size of the largest alternative type.

No `reinterpret_cast` means `constexpr`, `constexpr` is good. So the storage field is:

```cpp
union {
    T value;
} storage;
```

Construction and destruction of the value is performed the same way, but the pointer is obtained via `&this->storage.value`. The value is accessed via `this->storage.value`. So, the three basic operations are:

- Construction

```cpp
std::construct_at(&this->storage.value, );
```

- Destruction

```cpp
std::destroy_at(&this->storage.value);
```

- Access

```cpp
T value = this->storage.value;
```

This simple change allows us to use the union storage in a constexpr context.

## Interface

Knowing basic operations we can implement the interface of `option`. This is mostly a boilerplate code, as it is composed of the described operations and checks of the current state. More on it in the [docs](https://scurrra.github.io/atl/docs/types/option/).

{{< cards >}}
{{< card link="https://godbolt.org/z/af1he5Edh" title="Godbolt raw storage example" icon="globe" >}}
{{< card link="https://godbolt.org/z/Tr9rveE8r" title="Godbolt union storage example" icon="globe" >}}
{{< /cards >}}
