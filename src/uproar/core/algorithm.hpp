#ifndef UPROAR_CORE_ALGORITHM_HPP
#define UPROAR_CORE_ALGORITHM_HPP

#include <cstdint>

namespace tc
{
	template <typename Type>
	static constexpr auto quick_floor(const Type &v)
	{
		return static_cast<Type>(static_cast<int64_t>(v < 0 ? v - 1 : v));
	}

	template <typename Type, typename Alpha>
	static constexpr auto lerp(const Type &v0, const Type &v1, const Alpha& alpha)
	{
		return static_cast<Type>((1 - alpha) * v0 + alpha * v1);
	}

	template<typename Type>
	static constexpr auto clamp(const Type& v, const Type& min, const Type& max)
	{
		if constexpr (v > max) {
			return max;
		}
		else if constexpr (v < min) {
			return min;
		}
		else {
			return v;
		}
	}
} // namespace tc

#endif // UPROAR_CORE_ALGORITHM_HPP