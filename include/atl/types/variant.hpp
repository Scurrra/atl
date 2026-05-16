#ifndef ATL_VARIANT_HPP
#define ATL_VARIANT_HPP

#include <cstddef>    //std::size_t
#include <memory>     // std::construct_at, std::destroy_at
#include <new>        // std::launder
#include <stdexcept>  // std::exception
#include <utility>    // std::index_sequence

#include "../utils/concepts.hpp"  // reimports <concepts> and <type_traits>

namespace abl {

/// @brief Exception thrown when attempting to get a wrong alternative value of
/// `abl::variant`.
class bad_variant_access : public std::exception {
   public:
    explicit bad_variant_access() noexcept = default;

    const char* what() const noexcept override {
        return "Attempt to get a wrong alternative value of `abl::variant`.";
    }
};

/// @brief A variant type that holds one of the given *unique* types.
template <typename... Ts>
    requires all_unique<Ts...>
struct variant {
   private:
    /// @brief Friend declaration for the `variant` template with a different
    /// type pack.
    /// @note This is used to give access to the private members of another
    /// `variant` instance.
    template <typename... Us>
        requires all_unique<Us...>
    friend struct variant;

    /// @brief The storage recursive union for the variant.
    template <typename... Types>
    union storage;
    template <typename T, typename... Rest>
    union storage<T, Rest...> {
        T value;
        storage<Rest...> rest;
        storage() noexcept {}
    };
    template <typename T>
    union storage<T> {
        T value;
        storage() noexcept {}
    };

    /// @brief Helper struct to dispatch storage access by index.
    template <std::size_t I, typename T>
    struct storage_dispatcher {
        template <typename Storage>
        static constexpr T& access(Storage& s) {
            if constexpr (I == 0) {
                return *std::launder(&s.value);
            } else {
                return storage_dispatcher<I - 1, T>::access(s.rest);
            }
        }
        template <typename Storage>
        static constexpr const T& access(const Storage& s) {
            if constexpr (I == 0) {
                return *std::launder(&s.value);
            } else {
                return storage_dispatcher<I - 1, T>::access(s.rest);
            }
        }
    };

    /// @brief Helper struct to dispatch index access by type.
    template <std::size_t I, typename... Types>
    struct index_of_impl;
    template <typename T, typename Head>
    struct index_of_impl<sizeof...(Ts) - 1, T, Head> {
        // `T` is guaranteed to be in `Ts...`
        static constexpr std::size_t value = sizeof...(Ts) - 1;
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

    /// @brief Current index of the active alternative.
    std::size_t indx = sizeof...(Ts);
    /// @brief Storage for the active alternative.
    storage<Ts...> data;

    /// @brief Destroys the active alternative.
    void destroy() {
        if (this->indx != sizeof...(Ts)) {
            [this]<std::size_t... It>(std::index_sequence<It...>) {
                ((this->indx == It
                      ? std::destroy_at(
                            &storage_dispatcher<It, type_at<It, Ts...>>::access(
                                this->data))
                      : void()),
                 ...);
            }(std::make_index_sequence<sizeof...(Ts)>{});
            this->indx = sizeof...(Ts);
        }
    }

   public:
    /// @brief Default constructor.
    variant() = default;

    /// @brief Destructor.
    ~variant() noexcept { this->destroy(); }

    /// @brief Constructs a `variant` with the given value.
    template <typename T>
        requires contains<T, Ts...>
    variant(const T& data) noexcept(std::is_nothrow_copy_constructible_v<T>) {
        constexpr auto I = index_of<T, Ts...>;
        std::construct_at(&storage_dispatcher<I, T>::access(this->data), data);
        this->indx = I;
    }
    template <typename T>
        requires contains<T, Ts...>
    variant(T&& data) noexcept(std::is_nothrow_move_constructible_v<T>) {
        constexpr auto I = index_of<T, Ts...>;
        std::construct_at(&storage_dispatcher<I, T>::access(this->data),
                          std::move(data));
        this->indx = I;
    }

    /// @brief Constructs a `variant` with contained type `T` with the given
    /// arguments.
    /// @param args The arguments to construct `T` with.
    /// @return A reference to the constructed `T`.
    template <typename T, typename... Args>
        requires contains<T, Ts...> && std::constructible_from<T, Args...>
    T& emplace(Args&&... args) noexcept(
        (std::is_nothrow_destructible_v<Ts> && ...) &&
        std::is_nothrow_constructible_v<T, Args...>) {
        this->destroy();
        constexpr auto I = index_of<T, Ts...>;
        std::construct_at(&storage_dispatcher<I, T>::access(this->data),
                          std::forward<Args>(args)...);
        this->indx = I;
        return storage_dispatcher<I, T>::access(this->data);
    }

    /// @brief Emplaces a `variant` with contained type of index `I` with the
    /// given arguments.
    /// @param args The arguments to construct the contained type with.
    /// @return A reference to the constructed contained type.
    template <std::size_t I, typename... Args>
        requires(I < sizeof...(Ts)) &&
                std::constructible_from<type_at<I, Ts...>, Args...>
    type_at<I, Ts...>& emplace(Args&&... args) noexcept(
        (std::is_nothrow_destructible_v<Ts> && ...) &&
        std::is_nothrow_constructible_v<type_at<I, Ts...>, Args...>) {
        this->destroy();
        using T = type_at<I, Ts...>;
        std::construct_at(&storage_dispatcher<I, T>::access(this->data),
                          std::forward<Args>(args)...);
        this->indx = I;
        return storage_dispatcher<I, T>::access(this->data);
    }

    /// @brief Copy constructor.
    /// @param other The `variant` to copy from.
    variant(const variant& other) noexcept(
        (std::is_nothrow_copy_constructible_v<Ts> && ...)) {
        if (other.indx != sizeof...(Ts)) {
            [this, &other]<std::size_t... I>(std::index_sequence<I...>) {
                ((other.indx == I
                      ? (std::construct_at(
                             &storage_dispatcher<I, type_at<I, Ts...>>::access(
                                 this->data),
                             storage_dispatcher<I, type_at<I, Ts...>>::access(
                                 other.data)),
                         this->indx = I, void())
                      : void()),
                 ...);
            }(std::make_index_sequence<sizeof...(Ts)>{});
        }
    }

    /// @brief Move constructor.
    /// @param other The `variant` to move from.
    variant(variant&& other) noexcept(
        (std::is_nothrow_move_constructible_v<Ts> && ...) &&
        (std::is_nothrow_destructible_v<Ts> && ...)) {
        if (other.indx != sizeof...(Ts)) {
            [this, &other]<std::size_t... I>(std::index_sequence<I...>) {
                ((other.indx == I
                      ? (std::construct_at(
                             &storage_dispatcher<I, type_at<I, Ts...>>::access(
                                 this->data),
                             std::move(
                                 storage_dispatcher<I, type_at<I, Ts...>>::
                                     access(other.data))),
                         this->indx = I, other.destroy(), void())
                      : void()),
                 ...);
            }(std::make_index_sequence<sizeof...(Ts)>{});
        }
    }

    /// @brief Copy constructor from a `variant` with different types.
    /// @param other The `variant` to copy from.
    /// @note The types `Us...` must be a subset of `Ts...`.
    template <typename... Us>
        requires(contains<Us, Ts...> && ...)
    variant(const variant<Us...>& other) noexcept(
        (std::is_nothrow_constructible_v<Us, Us> && ...)) {
        auto copy_construct = [this]<std::size_t It, std::size_t Iu>(
                                  const variant<Us...>& other) {
            using T = type_at<It, Ts...>;
            using U = type_at<Iu, Us...>;
            std::construct_at(&storage_dispatcher<It, T>::access(this->data),
                              storage_dispatcher<Iu, U>::access(other.data));
            this->indx = It;  // index_of<T, Ts...>;
        };

        auto dispatch = [&copy_construct](std::size_t ind,
                                          const variant<Us...>& other) {
            [&ind, &other,
             &copy_construct]<std::size_t... Iu>(std::index_sequence<Iu...>) {
                ((ind == Iu
                      ? copy_construct.template
                        operator()<index_of<type_at<Iu, Us...>, Ts...>, Iu>(
                            other)
                      : void()),
                 ...);
            }(std::make_index_sequence<sizeof...(Us)>{});
        };

        if (other.indx != sizeof...(Us)) {
            dispatch(other.indx, other);
        }
    }

    /// @brief Move constructor from a `variant` with different types.
    /// @param other The `variant` to move from.
    /// @note The types `Us...` must be a subset of `Ts...`.
    template <typename... Us>
        requires(contains<Us, Ts...> && ...)
    variant(variant<Us...>&& other) noexcept(
        (std::is_nothrow_constructible_v<Us, Us> && ...) &&
        (std::is_nothrow_destructible_v<Us> && ...)) {
        auto move_construct =
            [this]<std::size_t It, std::size_t Iu>(variant<Us...>&& other) {
                using T = type_at<It, Ts...>;
                using U = type_at<Iu, Us...>;
                std::construct_at(
                    &storage_dispatcher<It, T>::access(this->data),
                    std::move(storage_dispatcher<Iu, U>::access(other.data)));
                this->indx = It;  // index_of<T, Ts...>;
                other.destroy();
            };

        auto dispatch = [&move_construct](std::size_t ind,
                                          variant<Us...>&& other) {
            [&ind, &other,
             &move_construct]<std::size_t... Iu>(std::index_sequence<Iu...>) {
                ((ind == Iu
                      ? move_construct.template
                        operator()<index_of<type_at<Iu, Us...>, Ts...>, Iu>(
                            std::move(other))
                      : void()),
                 ...);
            }(std::make_index_sequence<sizeof...(Us)>{});
        };

        if (other.indx != sizeof...(Us)) {
            dispatch(other.indx, std::move(other));
        }
    }

    /// @brief Copy assignment operator.
    /// @param other The `variant` to copy from.
    variant& operator=(const variant& other) noexcept(
        (std::is_nothrow_copy_constructible_v<Ts> && ...) &&
        (std::is_nothrow_destructible_v<Ts> && ...)) {
        if (this == &other) return *this;

        this->destroy();

        if (other.indx != sizeof...(Ts)) {
            [this, &other]<std::size_t... I>(std::index_sequence<I...>) {
                ((other.indx == I
                      ? (std::construct_at(
                             &storage_dispatcher<I, type_at<I, Ts...>>::access(
                                 this->data),
                             storage_dispatcher<I, type_at<I, Ts...>>::access(
                                 other.data)),
                         this->indx = I, void())
                      : void()),
                 ...);
            }(std::make_index_sequence<sizeof...(Ts)>{});
        }

        return *this;
    }

    /// @brief Move assignment operator.
    /// @param other The `variant` to move from.
    variant& operator=(variant&& other) noexcept(
        (std::is_nothrow_move_constructible_v<Ts> && ...) &&
        (std::is_nothrow_destructible_v<Ts> && ...)) {
        if (this == &other) return *this;

        this->destroy();

        if (other.indx != sizeof...(Ts)) {
            [this, &other]<std::size_t... I>(std::index_sequence<I...>) {
                ((other.indx == I
                      ? (std::construct_at(
                             &storage_dispatcher<I, type_at<I, Ts...>>::access(
                                 this->data),
                             std::move(
                                 storage_dispatcher<I, type_at<I, Ts...>>::
                                     access(other.data))),
                         this->indx = I, other.destroy(), void())
                      : void()),
                 ...);
            }(std::make_index_sequence<sizeof...(Ts)>{});
        }

        return *this;
    }

    /// @brief Copy assignment operator from a `variant` with different types.
    /// @param other The `variant` to copy from.
    /// @note The types `Us...` must be a subset of `Ts...`.
    template <typename... Us>
        requires(contains<Us, Ts...> && ...)
    variant& operator=(const variant<Us...>& other) noexcept(
        (std::is_nothrow_destructible_v<Ts> && ...) &&
        (std::is_nothrow_constructible_v<Us, Us> && ...)) {
        if (this == &other) return *this;

        this->destroy();

        auto copy_construct = [this]<std::size_t It, std::size_t Iu>(
                                  const variant<Us...>& other) {
            using T = type_at<It, Ts...>;
            using U = type_at<Iu, Us...>;
            std::construct_at(&storage_dispatcher<It, T>::access(this->data),
                              storage_dispatcher<Iu, U>::access(other.data));
            this->indx = It;  // index_of<T, Ts...>;
        };

        auto dispatch = [&copy_construct](std::size_t ind,
                                          const variant<Us...>& other) {
            [&ind, &other,
             &copy_construct]<std::size_t... Iu>(std::index_sequence<Iu...>) {
                ((ind == Iu
                      ? copy_construct.template
                        operator()<index_of<type_at<Iu, Us...>, Ts...>, Iu>(
                            other)
                      : void()),
                 ...);
            }(std::make_index_sequence<sizeof...(Us)>{});
        };

        if (other.indx != sizeof...(Us)) {
            dispatch(other.indx, other);
        }

        return *this;
    }

    /// @brief Move assignment operator from a `variant` with different types.
    /// @param other The `variant` to move from.
    /// @note The types `Us...` must be a subset of `Ts...`.
    template <typename... Us>
        requires(contains<Us, Ts...> && ...)
    variant& operator=(variant<Us...>&& other) noexcept(
        (std::is_nothrow_destructible_v<Ts> && ...) &&
        (std::is_nothrow_constructible_v<Us, Us> && ...) &&
        (std::is_nothrow_destructible_v<Us> && ...)) {
        if (this == &other) return *this;

        this->destroy();

        auto move_construct =
            [this]<std::size_t It, std::size_t Iu>(variant<Us...>&& other) {
                using T = type_at<It, Ts...>;
                using U = type_at<Iu, Us...>;
                std::construct_at(
                    &storage_dispatcher<It, T>::access(this->data),
                    std::move(storage_dispatcher<Iu, U>::access(other.data)));
                this->indx = It;  // index_of<T, Ts...>;
                other.destroy();
            };

        auto dispatch = [&move_construct](std::size_t ind,
                                          variant<Us...>&& other) {
            [&ind, &other,
             &move_construct]<std::size_t... Iu>(std::index_sequence<Iu...>) {
                ((ind == Iu
                      ? move_construct.template
                        operator()<index_of<type_at<Iu, Us...>, Ts...>, Iu>(
                            other)
                      : void()),
                 ...);
            }(std::make_index_sequence<sizeof...(Us)>{});
        };

        if (other.indx != sizeof...(Us)) {
            dispatch(other.indx, std::move(other));
        }

        return *this;
    }

    /// @brief Resets the `variant` to its default state.
    void reset() { this->destroy(); }

    /// @brief Returns whether the `variant` holds an alternative of type `T`.
    template <typename T>
        requires contains<T, Ts...>
    constexpr bool holds_alternative() const noexcept {
        constexpr auto I = index_of<T, Ts...>;
        return I == this->indx;
    }

    /// @brief Returns the index of the alternative held by the `variant`.
    constexpr std::size_t index() const noexcept { return this->indx; }

    /// @brief Returns whether the `variant` holds an alternative of type `T`.
    constexpr explicit operator bool() const noexcept {
        return this->indx != sizeof...(Ts);
    }

    /// @brief Returns the alternative held by the `variant` of type `T`.
    /// @tparam T The type of the alternative to get.
    /// @return A reference to the alternative of type `T`.
    template <typename T>
        requires contains<T, Ts...>
    constexpr T& get() & {
        constexpr auto I = index_of<T, Ts...>;
        if (I != this->indx) throw bad_variant_access();
        return storage_dispatcher<I, T>::access(this->data);
    }

    /// @brief Returns the alternative held by the `variant` of type `T`.
    /// @tparam T The type of the alternative to get.
    /// @return A const reference to the alternative of type `T`.
    template <typename T>
        requires contains<T, Ts...>
    constexpr const T& get() const& {
        constexpr auto I = index_of<T, Ts...>;
        if (I != this->indx) throw bad_variant_access();
        return storage_dispatcher<I, T>::access(this->data);
    }

    /// @brief Returns the alternative held by the `variant` of type `T`.
    /// @tparam T The type of the alternative to get.
    /// @return A rvalue reference to the alternative of type `T`.
    template <typename T>
        requires contains<T, Ts...>
    constexpr T&& get() && {
        constexpr auto I = index_of<T, Ts...>;
        if (I != this->indx) throw bad_variant_access();
        return std::move(storage_dispatcher<I, T>::access(this->data));
    }

    /// @brief Returns the alternative held by the `variant` of type `T`.
    /// @tparam T The type of the alternative to get.
    /// @return A const rvalue reference to the alternative of type `T`.
    template <typename T>
        requires contains<T, Ts...>
    constexpr const T&& get() const&& {
        constexpr auto I = index_of<T, Ts...>;
        if (I != this->indx) throw bad_variant_access();
        return std::move(storage_dispatcher<I, T>::access(this->data));
    }

    /// @brief Returns the alternative held by the `variant` at index `I`.
    /// @tparam I The index of the alternative to get.
    /// @return A reference to the alternative at index `I`.
    template <std::size_t I>
        requires(I < sizeof...(Ts))
    constexpr type_at<I, Ts...>& get() & {
        if (I != this->indx) throw bad_variant_access();
        using T = type_at<I, Ts...>;
        return storage_dispatcher<I, T>::access(this->data);
    }

    /// @brief Returns the alternative held by the `variant` at index `I`.
    /// @tparam I The index of the alternative to get.
    /// @return A const reference to the alternative at index `I`.
    template <std::size_t I>
        requires(I < sizeof...(Ts))
    constexpr const type_at<I, Ts...>& get() const& {
        if (I != this->indx) throw bad_variant_access();
        using T = type_at<I, Ts...>;
        return storage_dispatcher<I, T>::access(this->data);
    }

    /// @brief Returns the alternative held by the `variant` at index `I`.
    /// @tparam I The index of the alternative to get.
    /// @return A rvalue reference to the alternative at index `I`.
    template <std::size_t I>
        requires(I < sizeof...(Ts))
    constexpr type_at<I, Ts...>&& get() && {
        if (I != this->indx) throw bad_variant_access();
        using T = type_at<I, Ts...>;
        return std::move(storage_dispatcher<I, T>::access(this->data));
    }

    /// @brief Returns the alternative held by the `variant` at index `I`.
    /// @tparam I The index of the alternative to get.
    /// @return A const rvalue reference to the alternative at index `I`.
    template <std::size_t I>
        requires(I < sizeof...(Ts))
    constexpr const type_at<I, Ts...>&& get() const&& {
        if (I != this->indx) throw bad_variant_access();
        using T = type_at<I, Ts...>;
        return std::move(storage_dispatcher<I, T>::access(this->data));
    }

    /// @brief Swaps the contents of this `variant` with another `variant`.
    /// @param other The `variant` to swap with.
    void swap(variant& other) noexcept(
        (std::is_nothrow_swappable_v<Ts> && ...) &&
        (std::is_nothrow_move_constructible_v<Ts> && ...) &&
        (std::is_nothrow_destructible_v<Ts> && ...)) {
        if (this == &other) return;

        if (this->indx == other.indx && this->indx != sizeof...(Ts)) {
            [this, &other]<std::size_t... I>(std::index_sequence<I...>) {
                auto swap_one = [this, &other]<std::size_t It>() {
                    if (this->indx == It) {
                        using T = type_at<It, Ts...>;
                        using std::swap;
                        swap(storage_dispatcher<It, T>::access(this->data),
                             storage_dispatcher<It, T>::access(other.data));
                    }
                };
                (swap_one.template operator()<I>(), ...);
            }(std::make_index_sequence<sizeof...(Ts)>{});
        } else {
            variant temp = std::move(*this);
            *this = std::move(other);
            other = std::move(temp);
        }
    }
};

/// @brief Returns whether the `variant` holds an alternative of type `T`.
/// @tparam T The type to check for.
/// @param v The `variant` to check.
/// @return `true` if the `variant` holds an alternative of type `T`, `false`
/// otherwise.
template <typename T, typename... Types>
    requires contains<T, Types...>
constexpr bool holds_alternative(const variant<Types...>& v) noexcept {
    return v.template holds_alternative<T>();
}

/// @brief Returns a reference to the alternative of type `T` held by the
/// `variant`.
/// @tparam T The type of the alternative to get.
/// @param v The `variant` to get the alternative from.
/// @return A reference to the alternative of type `T` held by the `variant`.
template <typename T, typename... Types>
    requires contains<T, Types...>
T& get(variant<Types...>& v) {
    return v.template get<T>();
}

/// @brief Returns a const reference to the alternative of type `T` held by the
/// `variant`.
/// @tparam T The type of the alternative to get.
/// @param v The `variant` to get the alternative from.
/// @return A const reference to the alternative of type `T` held by the
/// `variant`.
template <typename T, typename... Types>
    requires contains<T, Types...>
const T& get(const variant<Types...>& v) {
    return v.template get<T>();
}

/// @brief Returns a reference to the alternative of type `T` held by the
/// `variant`.
/// @tparam T The type of the alternative to get.
/// @param v The `variant` to get the alternative from.
/// @return A rvalue reference to the alternative of type `T` held by the
/// `variant`.
template <typename T, typename... Types>
    requires contains<T, Types...>
T&& get(variant<Types...>&& v) {
    return std::move(v.template get<T>());
}

/// @brief Returns a const reference to the alternative of type `T` held by the
/// `variant`.
/// @tparam T The type of the alternative to get.
/// @param v The `variant` to get the alternative from.
/// @return A const rvalue reference to the alternative of type `T` held by the
/// `variant`.
template <typename T, typename... Types>
    requires contains<T, Types...>
const T&& get(const variant<Types...>&& v) {
    return std::move(v.template get<T>());
}

/// @brief Returns a reference to the alternative at index `I` held by the
/// `variant`.
/// @tparam I The index of the alternative to get.
/// @param v The `variant` to get the alternative from.
/// @return A reference to the alternative at index `I` held by the `variant`.
template <std::size_t I, typename... Types>
    requires(I < sizeof...(Types))
auto& get(variant<Types...>& v) {
    return v.template get<I>();
}

/// @brief Returns a const reference to the alternative at index `I` held by the
/// `variant`.
/// @tparam I The index of the alternative to get.
/// @param v The `variant` to get the alternative from.
/// @return A const reference to the alternative at index `I` held by the
/// `variant`.
template <std::size_t I, typename... Types>
    requires(I < sizeof...(Types))
const auto& get(const variant<Types...>& v) {
    return v.template get<I>();
}

/// @brief Returns a rvalue reference to the alternative at index `I` held by
/// the `variant`.
/// @tparam I The index of the alternative to get.
/// @param v The `variant` to get the alternative from.
/// @return A rvalue reference to the alternative at index `I` held by the
/// `variant`.
template <std::size_t I, typename... Types>
    requires(I < sizeof...(Types))
auto&& get(variant<Types...>&& v) {
    return std::move(v.template get<I>());
}

/// @brief Returns a const rvalue reference to the alternative at index `I` held
/// by the `variant`.
/// @tparam I The index of the alternative to get.
/// @param v The `variant` to get the alternative from.
/// @return A const rvalue reference to the alternative at index `I` held by the
/// `variant`.
template <std::size_t I, typename... Types>
    requires(I < sizeof...(Types))
const auto&& get(const variant<Types...>&& v) {
    return std::move(v.template get<I>());
}

/// @brief Swaps the contents of two `variant` objects.
/// @tparam Types The types of the alternatives held by the `variant` objects.
/// @param lhs The first `variant` object to swap.
/// @param rhs The second `variant` object to swap.
template <typename... Types>
void swap(variant<Types...>& lhs,
          variant<Types...>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}
}  // namespace abl

#endif  // ATL_VARIANT_HPP
