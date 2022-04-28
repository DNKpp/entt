#ifndef ENTT_CORE_TUPLE_HPP
#define ENTT_CORE_TUPLE_HPP

#include <tuple>
#include <type_traits>
#include <utility>
#include "../config/config.h"

namespace entt {

/**
 * @brief Utility function to unwrap tuples of a single element.
 * @tparam Type Tuple type of any sizes.
 * @param value A tuple object of the given type.
 * @return The tuple itself if it contains more than one element, the first
 * element otherwise.
 */
template<typename Type>
constexpr decltype(auto) unwrap_tuple(Type &&value) ENTT_NOEXCEPT {
    if constexpr(std::tuple_size_v<std::remove_reference_t<Type>> == 1u) {
        return std::get<0>(std::forward<Type>(value));
    } else {
        return std::forward<Type>(value);
    }
}

/**
 * @brief Utility class to forward-and-apply tuple objects.
 * @tparam Func Type of underlying invocable object.
 */
template<typename Func>
struct forward_apply: private Func {
    /**
     * @brief Constructs a forward-and-apply object.
     * @tparam Args Types of arguments to use to construct the new instance.
     * @param args Parameters to use to construct the instance.
     */
    template<class... Args>
    constexpr forward_apply(Args &&...args) ENTT_NOEXCEPT_IF((std::is_nothrow_constructible_v<Func, Args...>))
        : Func{std::forward<Args>(args)...} {}

    /**
     * @brief Forwards and applies the arguments with the underlying function.
     * @tparam Type Tuple-like type to forward to the underlying function.
     * @param args Parameters to forward to the underlying function.
     * @return Return value of the underlying function, if any.
     */
    template<class Type>
    constexpr decltype(auto) operator()(Type &&args) ENTT_NOEXCEPT_IF(noexcept(std::apply(std::declval<Func &>(), args))) {
        return std::apply(static_cast<Func &>(*this), std::forward<Type>(args));
    }

    /*! @copydoc operator()() */
    template<class Type>
    constexpr decltype(auto) operator()(Type &&args) const ENTT_NOEXCEPT_IF(noexcept(std::apply(std::declval<const Func &>(), args))) {
        return std::apply(static_cast<const Func &>(*this), std::forward<Type>(args));
    }
};

/**
 * @brief Deduction guide.
 * @tparam Func Type of underlying invocable object.
 */
template<typename Func>
forward_apply(Func) -> forward_apply<std::remove_reference_t<std::remove_cv_t<Func>>>;

} // namespace entt

#endif
