---
title: "Variant"
weight: 2
---

> Variant (Rust's enum, Python's Union, C's union, C++'s `std::variant`) is a more general sum type which can hold multiple types. Unfortunately, C++'s `std::variant` can hold the multiple alternatives of the same type, what is weird.

Previously we saw that unions are preferred over raw storage, so let's stick with that. But wait, all types of union's alternatives must be known while union declaration, that is... quite questionable. Turns out that unions can be declared recursively with templates:

```cpp
// base struct
template <typename... Types>
union storage;
// general alternative
template <typename T, typename... Rest>
union storage<T, Rest...> {
    // the value
    T value;
    // the rest
    storage<Rest...> rest;
    storage() noexcept {}
};
// terminal alternative
template <typename T>
union storage<T> {
    // only the value
    T value;
    storage() noexcept {}
};
```

Unions guarantee that all the alternatives occupy the same memory and the largest alternative determines the size of the union. The terminal alternative is of size `sizeof(T)`, the other are of size `max(sizeof(T), sizeof(storage<Rest...>))`. This gives us the same exact behavior, but declaration is variadic now.

The traversal of the recursive struct is also recursive:

```cpp
template <std::size_t I, typename T>
struct storage_dispatcher {
    // variadic union accessor
    template <typename Storage>
    static constexpr T& access(Storage& s) {
        if constexpr (I == 0) {
            return *std::launder(&s.value);
        } else {
            return storage_dispatcher<I - 1, T>::access(s.rest);
        }
    }
    // and the same for const
};
```

Wait, what is `I`? We should somehow know value of what type we are currently storing. As all the types are distinct, we can enumerate them. Additionally, we should be able to get the index `I` in pack `Ts...` knowing only the type `T` and vice versa:

```cpp
/// @brief Helper struct to dispatch index access by type.
template <std::size_t I, typename... Types>
struct index_of_impl;
template <typename T, typename Head>
struct index_of_impl<sizeof...(Types) - 1, T, Head> {
    // `T` is guaranteed to be in `Types...`
    static constexpr std::size_t value = sizeof...(Types) - 1;
};
template <std::size_t I, typename T, typename Head, typename... Tail>
struct index_of_impl<I, T, Head, Tail...> {
    static constexpr std::size_t value =
        std::is_same_v<T, Head> ? I
                                : index_of_impl<I + 1, T, Tail...>::value;
};

/// @brief Struct to dispatch type access by index.
template <typename T, typename... Types>
static constexpr std::size_t index_of =
    index_of_impl<0, T, Types...>::value;

/// @brief Helper struct to dispatch type access by index.
template <std::size_t I, typename... Types>
struct type_at_impl;
template <typename Head, typename... Tail>
struct type_at_impl<0, Head, Tail...> {
    using type = Head;
};
template <std::size_t I, typename Head, typename... Tail>
struct type_at_impl<I, Head, Tail...> {
    using type = typename type_at_impl<I - 1, Tail...>::type;
};

/// @brief Struct to dispatch type access by index.
template <std::size_t I, typename... Types>
using type_at = typename type_at_impl<I, Types...>::type;
```

So, the three basic operations are:

- Construction

```cpp
std::construct_at(&storage_dispatcher<I, T>::access(this->data), );
```

- Destruction

```cpp
std::destroy_at(&storage_dispatcher<I, T>::access(this->data));
```

- Access

```cpp
T value = storage_dispatcher<I, T>::access(this->data);
```

> [!NOTE]
> One of `I` or `T` should be known at compile time, and the other can be found as `I = index_of<T, Types...>` and `T = type_at<I, Types...>`.

The problem comes in when don't know either at compile time. The index of the current alternative is stored in the variant as field (`std::size_t indx` in our case). We can use this to dispatch the correct accessor at runtime. Here is the example for destroying the current alternative:

```cpp
[this]<std::size_t... It>(std::index_sequence<It...>) {
    ((this->indx == It
        ? std::destroy_at(
              &storage_dispatcher<It, type_at<It, Ts...>>::access(
                    this->data))
        : void()),
    ...);
}(std::make_index_sequence<sizeof...(Ts)>{});
```

Horrifying, isn't it? Well, it's an instantly called lambda expression, which takes a `std::index_sequence` of `sizeof...(Ts)` elements and uses it to dispatch the correct accessor at runtime. Each element is captured as an element of the pack of indices `It`. Then, in the fold expression, each element is compared with `this->indx` and the correct accessor is called if the indices match. In this case, we call `std::destroy_at` on the pointer to the current alternative. In a ternary operator, both lhs and rhs should be of the same type, so the rhs is a placeholder `void()` that does nothing.

This was the basic dispatch mechanism. Turns out that this is needed only for the copy/move constructors and assignment operators. In other methods we either have one of the parameters or simply don't access the variant's data at all (see in the docs).

`abl::variant` has something `std::variant` doesn't have. Should we have the ability to construct a new variant from the other variant that is guaranteed to hold the compatible type? I think yes. These cases are when the type pack `Us...` of the new variant is a permutation of `Ts...` or simply `Us...` is a subset of `Ts...`.

Pupupu... so, we have an empty variant `this` (empty in the constructor or emptied during assignment) and a variant `other` that contains the data. We have to dispatch the `other.indx` into `Iu`, get the type `U = type_at<Iu, Us...>`, get the index `It = index_of<U, Ts...>`, and then finally construct the new variant.

The dispatcher should calls a helper lambda with signature `[this]<std::size_t It, std::size_t Iu>(const variant<Us...>& other)` that simply wraps the construction of the new variant. So, the call will be `helper<index_of<type_at<Iu, Us...>, Ts...>, Iu>(other)`? No! C++ can not compare `Iu` with `variant<Us...>`, i.e. fails to parse. Lambda is something with `operator()`, and it can be called as:

```cpp
...
helper.template operator()<index_of<type_at<Iu, Us...>, Ts...>, Iu>(other)
...
```

Gorgeous! And it works! What else do we need to do? In C++ inside a template struct `variant` with `Ts...` types, we can simply use `variant` to describe this exact specialization. But we cannot have access to the private members of `variant<Us...>`, as it's another type. Until it is our friend!

```cpp
template <typename... Us>
    requires all_unique<Us...>
friend struct variant;
```

Declaring this somewhere inside `variant` allows us to access the private members of `variant<Us...>`. Oh, `all_unique` is a custom concept that checks if all types in `Us...` are unique to not be like weird `std::variant`.

What else does `abl::variant` have `std::variant` does not? `.get<I>()` and `.get<T>()` accessors as member functions (additionally to non-member, of course). And yes, sometimes you will have to write `v.template get<...>()`, depends on the context.

We discussed all the major challenges of implementing `abl::variant`, the interface is almost the same as `std::variant` and can be found in the docs.

{{< cards >}}
{{< card link="https://scurrra.github.io/atl/docs/types/variant/" title="Docs" icon="globe" >}}
{{< card link="https://godbolt.org/z/9sczMs6n1" title="Godbolt example" icon="globe" >}}
{{< /cards >}}
