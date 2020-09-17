#pragma once

#include <cstdint>

namespace tc
{
	template <typename Type>
	static auto quick_floor(const Type &v)
	{
		return static_cast<Type>(static_cast<int64_t>(v < 0 ? v - 1 : v));
	}

	template <typename Type>
	static auto lerp(const Type &v0, const Type &v1, const Type& alpha)
	{
		return static_cast<Type>((1 - alpha) * v0 + alpha * v1);
	}
} // namespace tc
