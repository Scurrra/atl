#ifndef ATL_NTUPLE_HPP
#define ATL_NTUPLE_HPP

#include <concepts>     // std::constructible_from
#include <cstddef>      // std::size_t
#include <type_traits>  // std::is_same, std::is_nothrow_move_constructible, std::is_nothrow_move_assignable, std::is_nothrow_swappable, std::integral_constant
#include <utility>  // std::forward, std::move, std::index_sequence, std::make_index_sequence, std::swap, std::tuple_element, std::tuple_size

namespace abl {

/// @brief A fixed-size tuple-like container that stores `N` elements of type
/// `T`.
template <std::size_t N, typename T>
struct ntuple {
    /// @brief Default constructor.
    constexpr ntuple() = default;

    /// @brief Constructor that takes `N` arguments of type `T`.
    /// @param args The arguments to store in the `ntuple`.
    template <typename... Args>
        requires(sizeof...(Args) == N) &&
                (std::constructible_from<T, Args> && ...)
    constexpr ntuple(Args&&... args) : data(std::forward<Args>(args)...) {}

    /// @brief Copy constructor.
    /// @param other The `ntuple` to copy from.
    constexpr ntuple(const ntuple&) = default;
    /// @brief Move constructor.
    /// @param other The `ntuple` to move from.
    constexpr ntuple(ntuple&&) noexcept(
        std::is_nothrow_move_constructible_v<T>) = default;

    /// @brief Copy assignment operator.
    /// @param other The `ntuple` to copy from.
    constexpr ntuple& operator=(const ntuple&) = default;
    /// @brief Move assignment operator.
    /// @param other The `ntuple` to move from.
    constexpr ntuple& operator=(ntuple&&) noexcept(
        std::is_nothrow_move_assignable_v<T>) = default;

    /// @brief Returns the size of the `ntuple`.
    constexpr static auto size = N;

    /// @brief Returns a reference to the `I`th element of the `ntuple`.
    /// @param I The index of the element to return.
    template <std::size_t I>
        requires(I < N)
    constexpr T& get() & noexcept {
        return static_cast<leaf<I>&>(this->data).value;
    }

    /// @brief Returns a const reference to the `I`th element of the `ntuple`.
    /// @param I The index of the element to return.
    template <std::size_t I>
        requires(I < N)
    constexpr const T& get() const& noexcept {
        return static_cast<const leaf<I>&>(this->data).value;
    }

    /// @brief Returns a reference to the `I`th element of the `ntuple`.
    /// @param I The index of the element to return.
    template <std::size_t I>
        requires(I < N)
    constexpr T&& get() && noexcept {
        return std::move(static_cast<leaf<I>&>(this->data).value);
    }

    /// @brief Returns a const reference to the `I`th element of the `ntuple`.
    /// @param I The index of the element to return.
    template <std::size_t I>
        requires(I < N)
    constexpr const T&& get() const&& noexcept {
        return std::move(static_cast<const leaf<I>&>(this->data).value);
    }

    /// @brief Swaps the contents of this `ntuple` with another `ntuple`.
    /// @param other The `ntuple` to swap with.
    constexpr void swap(ntuple& other) noexcept(
        (std::is_nothrow_swappable_v<T>)) {
        [&, this]<std::size_t... Is>(std::index_sequence<Is...>) {
            using std::swap;
            (swap(this->template get<Is>(), other.template get<Is>()), ...);
        }(indices);
    }

    /// @brief Compares two `ntuple`s with a three-way comparison operator.
    /// @param lhs The left-hand side `ntuple` to compare.
    /// @param rhs The right-hand side `ntuple` to compare.
    /// @returns An instance of comparison category.
    friend constexpr auto operator<=>(
        const ntuple& lhs,
        const ntuple& rhs) noexcept(noexcept(std::declval<const T&>() <=>
                                             std::declval<const T&>()))
        -> std::common_comparison_category_t<
            decltype(std::declval<const T&>() <=> std::declval<const T&>())> {
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            using ordering_type = std::common_comparison_category_t<
                decltype(std::declval<const T&>() <=>
                         std::declval<const T&>())>;
            ordering_type res = ordering_type::equal;
            (void)((res == ordering_type::equal &&
                    (res = lhs.template get<Is>() <=> rhs.template get<Is>()) !=
                        ordering_type::equal) ||
                   ...);
            return std::move(res);
        }(indices);
    }

    /// @brief Compares two `ntuple`s for equality.
    /// @param lhs The left-hand side `ntuple` to compare.
    /// @param rhs The right-hand side `ntuple` to compare.
    /// @returns `true` if the `ntuple`s are equal, `false` otherwise.
    friend constexpr bool operator==(
        const ntuple& lhs,
        const ntuple& rhs) noexcept(noexcept(std::declval<const T&>() ==
                                             std::declval<const T&>())) {
        return [&]<std::size_t... Is>(std::index_sequence<Is...>) {
            return ((lhs.template get<Is>() == rhs.template get<Is>()) && ...);
        }(indices);
    }

   protected:
    /// @brief The `leaf` struct holds a single element of type `T`.
    /// @tparam I The index of the element in the `ntuple`.
    template <std::size_t I>
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

    /// @brief The `inner` struct holds a sequence of `leaf` elements.
    /// @tparam Seq A `std::index_sequence` type used to generate the `leaf`
    /// elements.
    /// @note Inheritance from `leaf` for each element makes it basically a
    /// struct where each field can be accessed via casting to `leaf` and
    /// accessing `value`.
    template <typename Seq>
    struct inner;
    template <std::size_t... Is>
    struct inner<std::index_sequence<Is...>> : leaf<Is>... {
        template <typename... Args>
            requires(sizeof...(Args) == N) &&
                    (std::constructible_from<T, Args> && ...)
        constexpr inner(Args&&... args)
            : leaf<Is>(std::forward<Args>(args))... {}

        constexpr inner() = default;
        constexpr inner(const inner&) = default;
        constexpr inner(inner&&) noexcept = default;
        constexpr inner& operator=(const inner&) = default;
        constexpr inner& operator=(inner&&) noexcept = default;
    };

   private:
    /// @brief Helper. A `std::index_sequence` used to generate the `leaf`
    /// elements.
    constexpr static auto indices = std::make_index_sequence<N>{};
    /// @brief The `inner` struct holds a sequence of `leaf` elements.
    inner<std::make_index_sequence<N>> data;
};

/// @brief Swaps the contents of two `ntuple` objects.
/// @param lhs The first `ntuple` object to swap.
/// @param rhs The second `ntuple` object to swap.
template <std::size_t N, typename T>
void swap(ntuple<N, T>& lhs,
          ntuple<N, T>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

/// @brief Returns a reference to the `I`th element of the `ntuple`.
/// @param t The `ntuple` object to get the element from.
/// @return A reference to the `I`th element of the `ntuple`.
template <std::size_t I, std::size_t N, typename T>
    requires(I < N)
constexpr auto& get(ntuple<N, T>& t) noexcept {
    return t.template get<I>();
}

/// @brief Returns a const reference to the `I`th element of the `ntuple`.
/// @param t The `ntuple` object to get the element from.
/// @return A const reference to the `I`th element of the `ntuple`.
template <std::size_t I, std::size_t N, typename T>
    requires(I < N)
constexpr const auto& get(const ntuple<N, T>& t) noexcept {
    return t.template get<I>();
}

/// @brief Returns a reference to the `I`th element of the `ntuple`.
/// @param t The `ntuple` object to get the element from.
/// @return A reference to the `I`th element of the `ntuple`.
template <std::size_t I, std::size_t N, typename T>
    requires(I < N)
constexpr auto&& get(ntuple<N, T>&& t) noexcept {
    return t.template get<I>();
}

/// @brief Returns a const reference to the `I`th element of the `ntuple`.
/// @param t The `ntuple` object to get the element from.
/// @return A const reference to the `I`th element of the `ntuple`.
template <std::size_t I, std::size_t N, typename T>
    requires(I < N)
constexpr const auto&& get(const ntuple<N, T>&& t) noexcept {
    return t.template get<I>();
}
}  // namespace abl

namespace std {
template <std::size_t N, typename T>
struct tuple_size<abl::ntuple<N, T>> : std::integral_constant<std::size_t, N> {
};

template <std::size_t I, std::size_t N, typename T>
    requires(I < N)
struct tuple_element<I, abl::ntuple<N, T>> {
    using type = T;
};
}  // namespace std

#endif  // ATL_NTUPLE_HPP
