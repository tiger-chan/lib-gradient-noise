#ifndef UPROAR_CORE_EASING_HPP
#define UPROAR_CORE_EASING_HPP

#include "../config/config.hpp"
#include "attributes.hpp"

namespace tc {
	namespace internal {
		template<typename Type>
		inline static constexpr auto c_pow_int(const Type &t, int32_t p) UPROAR_NOEXCEPT {
			if (p == 0) {
				return Type{ 1 };
			}

			auto r = t;
			auto power = p < 0 ? -p : p;
			for (auto i = 1; i < power; ++i) {
				r *= t;
			}

			return p > 0 ? r : Type{ 1 } / r;
		}

		template<typename Type>
		inline static constexpr auto c_exponent_half(const Type &x, uint32_t y) UPROAR_NOEXCEPT {
			// f(x,y) = 2^(y-1) * x^y
			auto y1 = y == 0 ? 0 : y - 1;
			return c_pow_int(2, y) * c_pow_int(x, y);
		}

		template<typename Type>
		inline static auto exponent_half(const Type &x, double y) UPROAR_NOEXCEPT {
			// f(x,y) = 2^(y-1) * x^y
			auto y1 = y == 0 ? 0 : y - 1;
			return pow(2, y) * pow(x, y);
		}
	}    // namespace internal

	template<typename Type>
	static constexpr auto c_ease_in(const Type &t, uint32_t p = 2) UPROAR_NOEXCEPT {
		// t^p
		return internal::c_pow_int(t, p);
	}

	template<typename Type>
	static auto ease_in(const Type &t, double p = 2.0) UPROAR_NOEXCEPT {
		// t^p
		return pow(t, p);
	}

	template<typename Type>
	static constexpr auto c_ease_out(const Type &t, uint32_t p = 2) UPROAR_NOEXCEPT {
		// 1 - (1 - x)^2
		return 1 - internal::c_pow_int(1 - t, p);
	}

	template<typename Type>
	static auto ease_out(const Type &t, double p = 2.0) UPROAR_NOEXCEPT {
		// 1 - (1 - x)^2
		return 1 - pow(1 - t, p);
	}

	template<typename Type>
	static constexpr auto c_ease_in_out(const Type &t, uint32_t exp) UPROAR_NOEXCEPT {
		UPROAR_ASSERT(0.0 <= t && t <= 1.0);
		return t < 0.5
				 ? internal::c_exponent_half(t, exp)
				 : 1 - internal::c_exponent_half(1 - t, exp);
	}

	template<typename Type>
	static auto ease_in_out(const Type &t, double exp) UPROAR_NOEXCEPT {
		UPROAR_ASSERT(0.0 <= t && t <= 1.0);
		UPROAR_ASSERT(0.0 <= exp);
		return t < 0.5
				 ? internal::exponent_half(t, exp)
				 : 1 - internal::exponent_half(1 - t, exp);
	}
}    // namespace tc

#endif    // UPROAR_CORE_EASING_HPP
