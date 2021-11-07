#ifndef UPROAR_CORE_CURVE_HPP
#define UPROAR_CORE_CURVE_HPP

#include "../config/config.hpp"
#include "algorithm.hpp"

namespace tc {
	template<typename T>
	struct curve_blend {
		using decimal_t = UPROAR_DECIMAL_TYPE;
		decimal_t operator()(const decimal_t &v) const UPROAR_NOEXCEPT {
			return 0;
		}
	};

	template<>
	struct curve_blend<struct cubic_blend_t> {
		using decimal_t = UPROAR_DECIMAL_TYPE;
		decimal_t operator()(const decimal_t &v) const UPROAR_NOEXCEPT {
			return cubic_curve(v);
		}
	};

	template<>
	struct curve_blend<struct quintic_blend_t> {
		using decimal_t = UPROAR_DECIMAL_TYPE;
		decimal_t operator()(const decimal_t &v) const UPROAR_NOEXCEPT {
			return quintic_curve(v);
		}
	};

	using cubic_blend = curve_blend<struct cubic_blend_t>;
	using quintic_blend = curve_blend<struct quintic_blend_t>;
}    // namespace tc

#endif    // UPROAR_CORE_CURVE_HPP
