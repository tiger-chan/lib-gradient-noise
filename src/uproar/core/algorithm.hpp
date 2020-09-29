#ifndef UPROAR_CORE_ALGORITHM_HPP
#define UPROAR_CORE_ALGORITHM_HPP

#include "../config/config.hpp"
#include <cstdint>

namespace tc
{
	template <typename Type>
	static constexpr auto quick_floor(const Type &v) UPROAR_NOEXCEPT
	{
		return static_cast<Type>(static_cast<int64_t>(v < 0 ? v - 1 : v));
	}

	template <typename Type, typename Alpha>
	static constexpr auto lerp(const Type &v0, const Type &v1, const Alpha& alpha) UPROAR_NOEXCEPT
	{
		return static_cast<Type>((1 - alpha) * v0 + alpha * v1);
	}

	template<typename Type>
	static constexpr auto clamp(const Type& v, const Type& min, const Type& max) UPROAR_NOEXCEPT
	{
		if (v > max) {
			return max;
		}
		else if (v < min) {
			return min;
		}
		return v;
	}

	template<typename Type>
	static constexpr auto cubic_curve(const Type& t) UPROAR_NOEXCEPT
	{
		// 3t^2 − 2t^3
		return (t * t) * (3 - (2 * t));
	}

	template<typename Type>
	static constexpr auto quintic_curve(const Type& t) UPROAR_NOEXCEPT
	{
		// 6t^5 - 15t^4 + 10t^3
		return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
	}
} // namespace tc

#endif // UPROAR_CORE_ALGORITHM_HPP