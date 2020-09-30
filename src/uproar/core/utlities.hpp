#ifndef UPROAR_CORE_UTILITIES_HPP
#define UPROAR_CORE_UTILITIES_HPP

#include "../config/config.hpp"

namespace tc
{
	template<uint8_t I, uint8_t Size, typename Source, typename Container, typename ...Args>
	inline auto eval_with(const Source& func, const Container& ar, Args&&... args) UPROAR_NOEXCEPT
	{
		if constexpr (I < Size) {
			return eval_with<I + 1, Size>(func, ar, std::forward<Args>(args)..., ar[I]);
		}
		else
		{
			return func.eval(std::forward<Args>(args)...);
		}
	}
}

#endif // UPROAR_CORE_UTILITIES_HPP
