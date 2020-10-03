#ifndef UPROAR_CORE_NOISE_CONFIG_HPP
#define UPROAR_CORE_NOISE_CONFIG_HPP

#include "../config/config.hpp"
#include "fwd.hpp"
#include "json.hpp"

namespace tc
{
	struct UPROAR_API noise_config
	{
		octave_t octaves{};
		decimal_t lacunarity{};
		decimal_t persistance{};
		decimal_t frequency{};
		decimal_t amplitude{};

		void configure(const json::object& obj)
		{
			static const std::string octaves_key{"octaves"};
			static const std::string lacunarity_key{"lacunarity"};
			static const std::string persistance_key{"persistance"};
			static const std::string frequency_key{"frequency"};
			static const std::string amplitude_key{"amplitude"};

			auto end = std::end(obj);

			auto oct = obj.find(octaves_key);
			if (oct != end) {
				octaves = oct->second.as<octave_t>();
			}
			
			auto lac = obj.find(lacunarity_key);
			if (lac != end) {
				lacunarity = lac->second.as<decimal_t>();
			}

			auto per = obj.find(persistance_key);
			if (per != end) {
				persistance = per->second.as<decimal_t>();
			}

			auto freq = obj.find(frequency_key);
			if (freq != end) {
				frequency = freq->second.as<decimal_t>();
			}

			auto amp = obj.find(amplitude_key);
			if (amp != end) {
				amplitude = amp->second.as<decimal_t>();
			}
		}
	};
} // namespace tc

#endif // UPROAR_CORE_NOISE_CONFIG_HPP
