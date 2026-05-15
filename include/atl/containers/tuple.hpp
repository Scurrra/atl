#ifndef ATL_CONTAINERS_TUPLE_HPP
#define ATL_CONTAINERS_TUPLE_HPP

#include <concepts>     // std::constructible_from
#include <cstddef>      // std::size_t
#include <type_traits>  // std::is_same, std::is_nothrow_move_constructible, std::is_nothrow_move_assignable, std::is_nothrow_swappable, std::integral_constant
#include <utility>  // std::forward, std::move, std::index_sequence, std::make_index_sequence, std::swap, std::tuple_element, std::tuple_size

namespace abl {

/// @brief A tuple of types `Ts...`.
template <typename... Ts>
struct tuple {
   private:
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

    /// @brief Returns the type at index `I` in the `abl::tuple`.
    template <std::size_t I, typename... Types>
    using type_at = typename type_at_impl<I, Types...>::type;

    /// @brief Helper. The number of elements in the `abl::tuple`.
    constexpr static auto N = sizeof...(Ts);
    /// @brief Helper. The index sequence of the elements.
    constexpr static auto indices = std::make_index_sequence<N>{};
    /// @brief The inner storage of the `abl::tuple`.
    inner<std::make_index_sequence<N>> data;

   public:
    /// @brief Default constructor.
    constexpr tuple() = default;

    /// @brief Constructs a `abl::tuple` from `args...`.
    template <typename... Args>
        requires(sizeof...(Args) == sizeof...(Ts)) &&
                (std::constructible_from<Ts, Args> && ...)
    constexpr tuple(Args&&... args) : data(std::forward<Args>(args)...) {}

    /// @brief Copy constructor.
    /// @param other The `abl::tuple` to copy from.
    constexpr tuple(const tuple&) = default;

    /// @brief Move constructor.
    /// @param other The `abl::tuple` to move from.
    constexpr tuple(tuple&&) noexcept(
        (std::is_nothrow_move_constructible_v<Ts> && ...)) = default;

    /// @brief Copy assignment operator.
    /// @param other The `abl::tuple` to copy from.
    constexpr tuple& operator=(const tuple&) = default;

    /// @brief Move assignment operator.
    /// @param other The `abl::tuple` to move from.
    constexpr tuple& operator=(tuple&&) noexcept(
        (std::is_nothrow_move_assignable_v<Ts> && ...)) = default;

    /// @brief Returns the size of the `abl::tuple`.
    constexpr static auto size = sizeof...(Ts);

    /// @brief Returns a reference to the `I`th element of the `abl::tuple`.
    /// @tparam I The index of the element to get.
    /// @return A reference to the `I`th element of the `abl::tuple`.
    template <std::size_t I>
        requires(I < sizeof...(Ts))
    constexpr type_at<I, Ts...>& get() & noexcept {
        return static_cast<leaf<I, type_at<I, Ts...>>&>(this->data).value;
    }

    /// @brief Returns a const reference to the `I`th element of the
    /// `abl::tuple`.
    /// @tparam I The index of the element to get.
    /// @return A const reference to the `I`th element of the `abl::tuple`.
    template <std::size_t I>
        requires(I < sizeof...(Ts))
    constexpr const type_at<I, Ts...>& get() const& noexcept {
        return static_cast<const leaf<I, type_at<I, Ts...>>&>(this->data).value;
    }

    /// @brief Returns a reference to the `I`th element of the `abl::tuple`.
    /// @tparam I The index of the element to get.
    /// @return A rvalue reference to the `I`th element of the `abl::tuple`.
    template <std::size_t I>
        requires(I < sizeof...(Ts))
    constexpr type_at<I, Ts...>&& get() && noexcept {
        return std::move(
            static_cast<leaf<I, type_at<I, Ts...>>&>(this->data).value);
    }

    /// @brief Returns a const reference to the `I`th element of the
    /// `abl::tuple`.
    /// @tparam I The index of the element to get.
    /// @return A const rvalue reference to the `I`th element of the
    /// `abl::tuple`.
    template <std::size_t I>
        requires(I < sizeof...(Ts))
    constexpr const type_at<I, Ts...>&& get() const&& noexcept {
        return std::move(
            static_cast<const leaf<I, type_at<I, Ts...>>&>(this->data).value);
    }

    /// @brief Swaps the contents of this `abl::tuple` with another
    /// `abl::tuple`.
    /// @param other The `abl::tuple` to swap with.
    constexpr void swap(tuple& other) noexcept(
        (std::is_nothrow_swappable_v<Ts> && ...)) {
        [&, this]<std::size_t... Is>(std::index_sequence<Is...>) {
            using std::swap;
            (swap(this->template get<Is>(), other.template get<Is>()), ...);
        }(indices);
    }

    /// @brief Compares two `abl::tuple`s with a three-way comparison operator.
    /// @param lhs The left-hand side `abl::tuple` to compare.
    /// @param rhs The right-hand side `abl::tuple` to compare.
    /// @returns An instance of comparison category.
    friend constexpr auto operator<=>(
        const tuple& lhs,
        const tuple& rhs) noexcept((noexcept(std::declval<const Ts&>() <=>
                                             std::declval<const Ts&>()) &&
                                    ...))
        -> std::common_comparison_category_t<
            decltype(std::declval<const Ts&>() <=>
                     std::declval<const Ts&>())...> {
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            using ordering_type = std::common_comparison_category_t<
                decltype(std::declval<const Ts&>() <=>
                         std::declval<const Ts&>())...>;
            ordering_type res = ordering_type::equal;
            (void)((res == ordering_type::equal &&
                    (res = lhs.template get<Is>() <=> rhs.template get<Is>()) !=
                        ordering_type::equal) ||
                   ...);
            return std::move(res);
        }(indices);
    }

    /// @brief Compares two `abl::tuple`s for equality.
    /// @param lhs The left-hand side `abl::tuple` to compare.
    /// @param rhs The right-hand side `abl::tuple` to compare.
    /// @returns `true` if the `abl::tuple`s are equal, `false` otherwise.
    friend constexpr bool operator==(
        const tuple& lhs,
        const tuple& rhs) noexcept((noexcept(std::declval<const Ts&>() ==
                                             std::declval<const Ts&>()) &&
                                    ...)) {
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            return ((lhs.template get<Is>() == rhs.template get<Is>()) && ...);
        }(indices);
    }
};

/// @brief Swaps two `abl::tuple`s.
template <typename... Ts>
void swap(tuple<Ts...>& lhs,
          tuple<Ts...>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

/// @brief Returns a reference to the element at index `I` in the `abl::tuple`.
/// @tparam I The index of the element to return.
/// @param t The `abl::tuple` to return the element from.
/// @return A reference to the `I`th element of the `abl::tuple`.
template <std::size_t I, typename... Ts>
    requires(I < sizeof...(Ts))
constexpr auto& get(tuple<Ts...>& t) noexcept {
    return t.template get<I>();
}

/// @brief Returns a const reference to the element at index `I` in the
/// `abl::tuple`.
/// @tparam I The index of the element to return.
/// @param t The `abl::tuple` to return the element from.
/// @return A const reference to the `I`th element of the `abl::tuple`.
template <std::size_t I, typename... Ts>
    requires(I < sizeof...(Ts))
constexpr const auto& get(const tuple<Ts...>& t) noexcept {
    return t.template get<I>();
}

/// @brief Returns a reference to the element at index `I` in the `abl::tuple`.
/// @tparam I The index of the element to return.
/// @param t The `abl::tuple` to return the element from.
/// @return A rvalue reference to the `I`th element of the `abl::tuple`.
template <std::size_t I, typename... Ts>
    requires(I < sizeof...(Ts))
constexpr auto&& get(tuple<Ts...>&& t) noexcept {
    return t.template get<I>();
}

/// @brief Returns a const reference to the element at index `I` in the
/// `abl::tuple`.
/// @tparam I The index of the element to return.
/// @param t The `abl::tuple` to return the element from.
/// @return A const rvalue reference to the `I`th element of the `abl::tuple`.
template <std::size_t I, typename... Ts>
    requires(I < sizeof...(Ts))
constexpr const auto&& get(const tuple<Ts...>&& t) noexcept {
    return t.template get<I>();
}

}  // namespace abl

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

#endif
