#ifndef UPROAR_CORE_NOISE_CONFIG_HPP
#define UPROAR_CORE_NOISE_CONFIG_HPP

#include "../config/config.hpp"
#include "fwd.hpp"

namespace tc
{
	struct UPROAR_API noise_config
	{
		octave_t octaves{};
		decimal_t lacunarity{};
		decimal_t persistance{};
		decimal_t frequency{};
		decimal_t amplitude{};
	};
} // namespace tc

#endif // UPROAR_CORE_NOISE_CONFIG_HPP
