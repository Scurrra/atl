---
title: "Tuple"
weight: 3
---

> Tuple is a product type. `std::tuple` is pretty nice, but what if I want to have an `N`-dimensional tuple (product) of the same type (set)?

Anyway, how to implement it? My first thought was to use a raw storage, shift on it by `alignof(T)` for each `T` and be happy. But there is a much nicer way to implement it. And it is actually an inheritance from a pack of indexed wrappers. Sounds complicated, so here is the code:

```cpp
/// @brief The data of the `abl::tuple`.
/// @tparam I The index of the element.
/// @tparam T The type of the element.
template <std::size_t I, typename T>
struct leaf {
    [[no_unique_address]] T value;

    template <typename U>
        requires std::constructible_from<T, U>
    constexpr leaf(U&& u) : value(std::forward<U>(u)) {}

    constexpr leaf() = default;
    constexpr leaf(const leaf&) = default;
    constexpr leaf(leaf&&) noexcept(
        std::is_nothrow_move_constructible_v<T>) = default;
    constexpr leaf& operator=(const leaf&) = default;
    constexpr leaf& operator=(leaf&&) noexcept(
        std::is_nothrow_move_assignable_v<T>) = default;
};

/// @brief The inner type of the `abl::tuple`.
/// @tparam Seq The index sequence of the elements.
/// @note Inheritance from `leaf` for each element makes it basically a
/// struct where each field can be accessed via casting to `leaf` and
/// accessing `value`.
template <typename Seq>
struct inner;
template <std::size_t... Is>
struct inner<std::index_sequence<Is...>> : leaf<Is, Ts>... {
    template <typename... Args>
        requires(sizeof...(Args) == sizeof...(Ts)) &&
                (std::constructible_from<Ts, Args> && ...)
    constexpr inner(Args&&... args)
        : leaf<Is, Ts>(std::forward<Args>(args))... {}

    constexpr inner() = default;
    constexpr inner(const inner&) = default;
    constexpr inner(inner&&) noexcept = default;
    constexpr inner& operator=(const inner&) = default;
    constexpr inner& operator=(inner&&) noexcept = default;
};
```

> [!TIP]
> In case when we have the same type, type parameter can be simply dropped as it's always the same type `T`.

So what are those:
- `leaf<I, T>` is a wrapper struct that assigns an index `I` to each `T`, `[[no_unique_address]]` says that if `sizeof(T) == 0` then `sizeof(leaf<I, T>) == 0` and the compiler can optimize it away.
- `inner<Seq>` is a... dispatcher: it is inherited from a pack of leaves, where each following leaf is stored right after the previous.

This trick gives us not only a consecutive memory layout for the tuple elements, but also a type-safe way to access them by index for free:

```cpp
...
static_cast<leaf<I, T>>(this->data).value
...
```

where 

```cpp
// builds the variadic struct
inner<std::make_index_sequence<N>> data;
```

> [!NOTE]
> This time we don't store anything but data, access is always done by index `t.get<I>()` and `abl::get<I>(t)`, so no horrific dispatchers.

The interesting things are happening in `swap` and comparison operators. For example, this is the whole `swap` implementation:

```cpp
constexpr static auto indices = std::make_index_sequence<N>{};
...
constexpr void swap(tuple& other) noexcept(
    (std::is_nothrow_swappable_v<Ts> && ...)) {
    [&, this]<std::size_t... Is>(std::index_sequence<Is...>) {
        using std::swap;
        (swap(this->template get<Is>(), other.template get<Is>()), ...);
    }(indices);
}
```

It simply folds over the indices to swap each element with the corresponding element in `other` (it uses the gorgeous `t.template get<Is>()` syntax!). Same with equality comparison:

```cpp
...
((lhs.template get<Is>() == rhs.template get<Is>()) && ...)
...
```

C++20 introduces a three-way comparison operator `<=>` that allows us to define comparison in a single line. Here's the implementation for `operator<=>`:

```cpp
friend constexpr auto operator<=>(const tuple& lhs,const tuple& rhs) 
    noexcept((
        noexcept(std::declval<const Ts&>() <=> std::declval<const Ts&>()) && ...))
    -> std::common_comparison_category_t<
        decltype(std::declval<const Ts&>() <=>std::declval<const Ts&>())...> {
    return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
        using ordering_type = std::common_comparison_category_t<
            decltype(std::declval<const Ts&>() <=> std::declval<const Ts&>())...>;
        ordering_type res = ordering_type::equal;
        (void)((res == ordering_type::equal &&
                (res = lhs.template get<Is>() <=> rhs.template get<Is>()) !=
                    ordering_type::equal) ||
               ...);
        return std::move(res);
    }(indices);
}
```

The funny thing about the spaceship is that it does not simply return `-1, 0, 1`; it returns an instance of comparison category, and `std::common_comparison_category` shows that there is a bunch of them! Anyway, what is happening here is that the spaceship operator is being used to compare each element of the tuple until they are not equal, and the result is being accumulated into `res`:
- `res == ordering_type::equal` checks that for now all the elements were equal;
- in `res = lhs.template get<Is>() <=> rhs.template get<Is>()` the new pair of elements is compared and the result is assigned to `res`;
- `(...) != ordering_type::equal` checks that the current pair of elements are not equal;
- and `(...) && (...)` checks if the result of comparison has changed;
- finally, `(...) || ...` works as early exit after the first occurence of `false` in the previous doing.

> [!NOTE]
> `(void)` helps to silence unused variable warnings, and `std::move(res)` was used to avoid unnecessary copies (not eliding copy on return [-Wnrvo]).

## Unpacking

Turns out that unpacking can be performed on anything `tuple-like`. To become `tuple-like`, any type must provide `tuple_size` and `tuple_element` specializations in the `std` namespace and has functional `get<I>` accessors. And it's literally the following:

```cpp
// `get<I>(t)` accessors are wrappers around `t.get<I>()`

namespace std {
template <typename... Types>
struct tuple_size<abl::tuple<Types...>>
    : std::integral_constant<std::size_t, sizeof...(Types)> {};

template <std::size_t I, class Head, class... Tail>
struct tuple_element<I, abl::tuple<Head, Tail...>>
    : std::tuple_element<I - 1, abl::tuple<Tail...>> {};

template <class Head, class... Tail>
struct tuple_element<0, abl::tuple<Head, Tail...>> {
    using type = Head;
};

}  // namespace std
```

For the full interface see the docs, and for the code and examples check out the examples at Compiler Explorer below.

{{< cards >}}
{{< card link="https://scurrra.github.io/atl/docs/containers/tuple/" title="Docs for abl::tuple" icon="globe" >}}
{{< card link="https://godbolt.org/z/ffeGW8xf1" title="Godbolt abl::tuple example" icon="globe" >}}

{{< card link="https://scurrra.github.io/atl/docs/containers/ntuple/" title="Docs for abl::ntuple" icon="globe" >}}
{{< card link="https://godbolt.org/z/PYbxbbfEe" title="Godbolt abl::ntuple example" icon="globe" >}}
{{< /cards >}}
