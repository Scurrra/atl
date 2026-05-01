#ifndef ATL_TYPES_FLAGS_HPP
#define ATL_TYPES_FLAGS_HPP

#include <concepts>
#include <type_traits>

namespace abl {

/// @brief `enum class` wrapper that extends the enum with `|` and `&` operators
/// and `.has` method.
template <typename Enum>
    requires std::is_enum_v<Enum>
class enum_wrapper {
    using underlying_type = std::underlying_type_t<Enum>;
    underlying_type value_{0};

   public:
    /// @brief Default constructor.
    constexpr enum_wrapper() = default;

    /// @brief Construct a wrapper from the passed enum.
    ///
    /// @param e The enum value to wrap.
    constexpr enum_wrapper(Enum e) : value_(static_cast<underlying_type>(e)) {}

    /// @brief Construct a wrapper from the integer value.
    ///
    /// @param value The integer value to wrap.
    template <typename T>
        requires std::convertible_to<T, underlying_type>
    constexpr enum_wrapper(T value)
        : value_(static_cast<underlying_type>(value)) {}

    /// @brief Bitwise OR assignment operator.
    constexpr enum_wrapper& operator|=(const enum_wrapper& other) {
        this->value_ |= other.value_;
        return *this;
    }

    /// @brief Bitwise AND assignment operator.
    constexpr enum_wrapper& operator&=(const enum_wrapper& other) {
        this->value_ &= other.value_;
        return *this;
    }

    /// @brief Bitwise OR operator.
    constexpr enum_wrapper operator|(const enum_wrapper& other) const {
        return enum_wrapper(*this) |= other;
    }

    /// @brief Bitwise AND operator.
    constexpr enum_wrapper operator&(const enum_wrapper& other) const {
        return enum_wrapper(*this) &= other;
    }

    /// @brief Explicit conversion to boolean.
    constexpr explicit operator bool() const { return value_ != 0; }

    /// @brief Explicit conversion to `Enum`.
    constexpr explicit operator Enum() const {
        return static_cast<Enum>(value_);
    }

    /// @brief Get the underlying integer value.
    constexpr underlying_type get_value() const { return this->value_; }

    /// @brief Check if the wrapper has the given enum value.
    constexpr bool has(Enum e) const {
        return (*this & enum_wrapper(e)).value_ != 0;
    }

    /// @brief Check including of an instance of underlying integer type.
    template <typename T>
        requires std::convertible_to<T, underlying_type>
    constexpr bool has(T value) const {
        return (*this & enum_wrapper(static_cast<underlying_type>(value)))
                   .value_ != 0;
    }

    /// @brief Check including of an instance of a type derived from this wrapper.
    template <typename T>
        requires std::is_base_of_v<enum_wrapper<Enum>, std::decay_t<T>>
    constexpr bool has(const T& flag) const {
        return (*this & flag).value_ != 0;
    }
};

/// @brief Flag type.
///
/// @tparam E The enum type.
template <typename E>
    requires std::is_enum_v<E>
class flag : public enum_wrapper<E> {
   public:
    using enum_wrapper<E>::enum_wrapper;

    /// @brief Create a flag with the given enum value.
    ///
    /// @tparam Value The enum value to wrap.
    template <E Value>
    static constexpr flag value() {
        return flag(Value);
    }

    /// @brief Combine multiple enum values into a single flag.
    ///
    /// @tparam Values The enum values to combine.
    ///
    /// @return A flag with the combined enum values.
    template <E... Values>
        requires(sizeof...(Values) > 0)
    static constexpr flag combine() {
        constexpr auto combined_value =
            (static_cast<std::underlying_type_t<E>>(Values) | ...);
        return static_cast<E>(combined_value);
    }
};

}  // namespace abl

#endif // ATL_TYPES_FLAGS_HPP
