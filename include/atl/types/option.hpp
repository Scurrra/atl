#ifndef ATL_OPTION_HPP
#define ATL_OPTION_HPP

#include <exception>  // std::exception
#include <memory>     // std::construct_at, std::destroy_at

namespace abl {

/// @brief Exception thrown when attempting to access a non-existent value of an
/// `abl::option`.
class bad_option_access : public std::exception {
   public:
    explicit bad_option_access() noexcept = default;

    const char* what() const noexcept override {
        return "Attempt to get non-existent value of `abl::option`.";
    }
};

/// @brief Represents an optional value of type `T`.
template <typename T>
struct option {
   private:
    /// @brief The state of the option, either `none` or `some`.
    enum class state : unsigned char { none = 0, some = 1 };

    /// @brief The storage for the value.
    union {
        T value;
    } storage;

    /// @brief The state of the option, either `none` or `some`.
    option<T>::state state = option<T>::state::none;

   public:
    /// @brief Default constructor.
    option() = default;

    /// @brief Constructor that takes a `T` by reference, copying it into the
    /// option.
    /// @param data The value to copy into the option.
    option(const T& data) noexcept(std::is_nothrow_copy_constructible_v<T>) {
        std::construct_at(&this->storage.value, data);
        this->state = state::some;
    }

    /// @brief Constructor that takes a `T` by reference, moving it into the
    /// option.
    /// @param data The value to move into the option.
    option(T&& data) noexcept(std::is_nothrow_move_constructible_v<T>) {
        std::construct_at(&this->storage.value, std::move(data));
        this->state = state::some;
    }

    /// @brief Emplaces a `T` into the option using the given arguments.
    /// @param args The arguments to use for constructing the `T`.
    /// @return A reference to the emplaced `T`.
    template <typename... Args>
    T& emplace(Args&&... args) noexcept(
        std::is_nothrow_constructible_v<T, Args...> &&
        std::is_nothrow_destructible_v<T>) {
        if (this->state == state::some) {
            std::destroy_at(&this->storage.value);
            this->state = state::none;
        }

        std::construct_at(&this->storage.value, std::forward<Args>(args)...);
        this->state = state::some;

        return this->storage.value;
    }

    /// @brief Copy constructor.
    /// @param other The `option` to copy from.
    option(const option<T>& other) noexcept(
        std::is_nothrow_copy_constructible_v<T>)
        : state(state::none) {
        if (other.state == state::some) {
            std::construct_at(&this->storage.value, *other);
            this->state = state::some;
        }
    }

    /// @brief Move constructor.
    /// @param other The `option` to move from.
    option(option<T>&& other) noexcept(
        std::is_nothrow_move_constructible_v<T> &&
        std::is_nothrow_destructible_v<T>) {
        if (other.state == state::some) {
            std::construct_at(&this->storage.value, std::move(*other));
            this->state = state::some;

            other.reset();
        } else {
            this->state = state::none;
        }
    }

    /// @brief Copy assignment operator that takes a `T` by reference, copying
    /// it into the option.
    /// @param value The value to copy into the option.
    /// @return A reference to the modified `option`.
    template <typename U = std::remove_cv_t<T>>
    option& operator=(const U& value) noexcept(
        std::is_nothrow_copy_constructible_v<T> &&
        std::is_nothrow_destructible_v<T>) {
        this->reset();

        std::construct_at(&this->storage.value, value);
        this->state = state::some;

        return *this;
    }

    /// @brief Move assignment operator that takes a `T` by reference, moving it
    /// into the option.
    /// @param value The value to move into the option.
    /// @return A reference to the modified `option`.
    template <typename U = std::remove_cv_t<T>>
    option& operator=(U&& value) noexcept(
        std::is_nothrow_move_constructible_v<T> &&
        std::is_nothrow_destructible_v<T>) {
        this->reset();

        std::construct_at(&this->storage.value, std::move(value));
        this->state = state::some;

        return *this;
    }

    /// @brief Copy assignment operator that takes an `option` by reference,
    /// copying it into the option.
    /// @param other The `option` to copy from.
    /// @return A reference to the modified `option`.
    option& operator=(const option<T>& other) noexcept(
        std::is_nothrow_copy_constructible_v<T> &&
        std::is_nothrow_destructible_v<T>) {
        if (this == &other) return *this;

        this->reset();

        if (other.state == state::some) {
            std::construct_at(&this->storage.value, *other);
            this->state = state::some;
        }

        return *this;
    }

    /// @brief Move assignment operator that takes an `option` by reference,
    /// moving it into the option.
    /// @param other The `option` to move from.
    /// @return A reference to the modified `option`.
    option& operator=(option<T>&& other) noexcept(
        std::is_nothrow_move_constructible_v<T> &&
        std::is_nothrow_destructible_v<T>) {
        if (this == &other) return *this;

        this->reset();

        if (other.state == state::some) {
            std::construct_at(&this->storage.value, std::move(*other));
            this->state = state::some;

            other.reset();
        }

        return *this;
    }

    /// @brief Swaps the contents of this `option` with another `option`.
    /// @param other The `option` to swap with.
    void swap(option<T>& other) noexcept(
        std::is_nothrow_move_constructible_v<T> &&
        std::is_nothrow_destructible_v<T> && std::is_nothrow_swappable_v<T>) {
        if (this->state == state::some && other.state == state::some) {
            using std::swap;
            swap(**this, *other);
        } else if (this->state == state::some) {
            other.emplace(std::move(**this));
            this->reset();
        } else if (other.state == state::some) {
            this->emplace(std::move(*other));
            other.reset();
        }
    }

    /// @brief Returns a reference to the value of this `option`.
    /// @return A reference to the value of this `option`.
    constexpr T& operator*() & noexcept { return this->storage.value; }

    /// @brief Returns a const reference to the value of this `option`.
    /// @return A const reference to the value of this `option`.
    constexpr const T& operator*() const& noexcept {
        return this->storage.value;
    }

    /// @brief Returns a rvalue reference to the value of this `option`.
    /// @return A rvalue reference to the value of this `option`.
    constexpr T&& operator*() && noexcept {
        return std::move(this->storage.value);
    }

    /// @brief Returns a const rvalue reference to the value of this `option`.
    /// @return A const rvalue reference to the value of this `option`.
    constexpr const T&& operator*() const&& noexcept {
        return std::move(this->storage.value);
    }

    /// @brief Returns a pointer to the value of this `option`.
    /// @return A pointer to the value of this `option`.
    constexpr T* operator->() & noexcept { return &this->storage.value; }

    /// @brief Returns a const pointer to the value of this `option`.
    /// @return A const pointer to the value of this `option`.
    constexpr const T* operator->() const& noexcept {
        return &this->storage.value;
    }

    /// @brief Returns whether this `option` has a value.
    /// @return `true` if this `option` has a value, `false` otherwise.
    constexpr bool has_value() const noexcept {
        return this->state == state::some;
    }

    /// @brief Returns whether this `option` has a value.
    /// @return `true` if this `option` has a value, `false` otherwise.
    constexpr explicit operator bool() const noexcept {
        return this->state == state::some;
    }

    /// @brief Returns a reference to the value of this `option`.
    /// @return A reference to the value of this `option`.
    constexpr T& value() & {
        if (this->state == state::none) throw bad_option_access();
        return **this;
    }

    /// @brief Returns a const reference to the value of this `option`.
    /// @return A const reference to the value of this `option`.
    constexpr const T& value() const& {
        if (this->state == state::none) throw bad_option_access();
        return **this;
    }

    /// @brief Returns a rvalue reference to the value of this `option`.
    /// @return A rvalue reference to the value of this `option`.
    constexpr T&& value() && {
        if (this->state == state::none) throw bad_option_access();
        return std::move(**this);
    }

    /// @brief Returns a const rvalue reference to the value of this `option`.
    /// @return A const rvalue reference to the value of this `option`.
    constexpr const T&& value() const&& {
        if (this->state == state::none) throw bad_option_access();
        return std::move(**this);
    }

    /// @brief Returns the value of this `option`, or a default value if the
    /// `option` does not contain a value.
    /// @return The value of this `option`, or a default value if the `option`
    /// does not contain a value.
    template <typename U = std::remove_cv_t<T>>
    T value_or(U&& default_value) const& noexcept(
        std::is_nothrow_constructible_v<T, U&&> &&
        std::is_nothrow_copy_constructible_v<T>) {
        if (this->state == state::some) return **this;
        return static_cast<T>(std::forward<U>(default_value));
    }

    /// @brief Returns the value of this `option`, or a default value if the
    /// `option` does not contain a value.
    /// @return The value of this `option`, or a default value if the `option`
    /// does not contain a value.
    template <typename U = std::remove_cv_t<T>>
    T value_or(U&& default_value) && noexcept(
        std::is_nothrow_constructible_v<T, U&&> &&
        std::is_nothrow_move_constructible_v<T>) {
        if (this->state == state::some) return std::move(**this);
        return static_cast<T>(std::forward<U>(default_value));
    }

    /// @brief Resets this `option` to `none`, destroying the value if it is
    /// `some`.
    void reset() noexcept {
        if (this->state == state::some) {
            std::destroy_at(&this->storage.value);
            this->state = state::none;
        }
    }

    ~option() { this->reset(); }
};

/// @brief Swaps the contents of two `option` objects.
/// @param lhs The first `option` to swap.
/// @param rhs The second `option` to swap.
template <typename T>
void swap(option<T>& lhs, option<T>& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
}

}  // namespace abl

#endif  // ATL_OPTION_HPP
