#ifndef ATL_COMPARE_COMPARATOR_HPP
#define ATL_COMPARE_COMPARATOR_HPP

#include <concepts>    // std::invocable, std::predicate
#include <functional>  // std::identity, std::less
#include <type_traits>  // std::invoke_result_t, std::is_invocable_r_v, std::decay_t
#include <utility>      // std::forward, std::move

namespace abl {

/// @brief A comparator that applies a key function and a comparison function to
/// values of type `V`.
/// @note Instances of this struct satisfy the `std::predicate` concept.
template <typename V, typename KeyFunc = std::identity,
          typename CompareFunc = std::less<>>
    requires std::invocable<const KeyFunc&, const V&> &&
             std::predicate<
                 const CompareFunc&,
                 const std::decay_t<
                     std::invoke_result_t<const KeyFunc&, const V&>>&,
                 const std::decay_t<
                     std::invoke_result_t<const KeyFunc&, const V&>>&>
struct comparator {
    /// @brief Default constructor.
    /// @note The key function is initialized to `std::identity` and the
    /// comparison function is initialized to `std::less<>`.
    constexpr comparator() = default;

    /// @brief Constructs a comparator with the given key function and
    /// comparison function.
    /// @param kf The key function to apply to values of type `V`.
    /// @param cf The comparison function to apply to the key values.
    template <typename KF, typename CF>
    explicit constexpr comparator(KF&& kf, CF&& cf)
        : key(std::forward<KF>(kf)), compare(std::forward<CF>(cf)) {}

    /// @brief Constructs a comparator with the given key function and a default
    /// comparison function.
    /// @param kf The key function to apply to values of type `V`.
    /// @note The comparison function is initialized to `std::less<>`.
    /// @return A comparator that applies the key function and comparison
    /// function to values of type `V`.
    template <typename KF>
        requires std::invocable<const KF&, const V&>
    [[nodiscard]] static constexpr auto make(KF&& kf) {
        using K = std::decay_t<std::invoke_result_t<const KF&, const V&>>;
        return comparator<V, std::decay_t<KF>, std::less<K>>{
            std::forward<KF>(kf), std::less<K>{}};
    }

    /// @brief Constructs a comparator with the given comparison function and a
    /// default key function.
    /// @param cf The comparison function to apply to the key values.
    /// @note The key function is initialized to `std::identity`.
    /// @return A comparator that applies the key function and comparison
    /// function to values of type `V`.
    template <typename CF>
        requires std::predicate<const CF&, const V&, const V&>
    [[nodiscard]] static constexpr auto make(CF&& cf) {
        return comparator<V, std::identity, std::decay_t<CF>>{
            std::identity{}, std::forward<CF>(cf)};
    }

    /// @brief Constructs a comparator with the given key function and
    /// comparison function.
    /// @param kf The key function to apply to values of type `V`.
    /// @param cf The comparison function to apply to the key values.
    /// @return A comparator that applies the key function and comparison
    /// function to values of type `V`.
    template <typename KF, typename CF>
        requires std::invocable<const KF&, const V&> &&
                 std::predicate<const CF&,
                                const std::decay_t<
                                    std::invoke_result_t<const KF&, const V&>>&,
                                const std::decay_t<
                                    std::invoke_result_t<const KF&, const V&>>&>
    [[nodiscard]] static constexpr auto make(KF&& kf, CF&& cf) {
        return comparator<V, std::decay_t<KF>, std::decay_t<CF>>{
            std::forward<KF>(kf), std::forward<CF>(cf)};
    }

    /// @brief Compares two values of type `V` using the key function and
    /// comparison function.
    /// @param lhs The left-hand side value to compare.
    /// @param rhs The right-hand side value to compare.
    /// @return `true` if `lhs` is less than `rhs`, `false` otherwise.
    /// @note Works perfectly as a predicate for `std::sort` and other
    /// algorithms.
    constexpr bool operator()(const V& lhs, const V& rhs) const
        requires std::predicate<
            const CompareFunc&,
            const std::decay_t<std::invoke_result_t<const KeyFunc&, const V&>>&,
            const std::decay_t<std::invoke_result_t<const KeyFunc&, const V&>>&>
    {
        auto&& l = this->key(lhs);
        auto&& r = this->key(rhs);
        return this->compare(static_cast<decltype(l)&&>(l),
                             static_cast<decltype(r)&&>(r));
    }

   private:
    [[no_unique_address]] const KeyFunc key{};
    [[no_unique_address]] const CompareFunc compare{};
};

}  // namespace abl

#endif  // ATL_COMPARE_COMPARATOR_HPP
