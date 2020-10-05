#ifndef UPROAR_TASKS_PERLIN_HPP
#define UPROAR_TASKS_PERLIN_HPP

#include "../config/config.hpp"
#include "../core/noise_config.hpp"
#include "fwd.hpp"
#include "config.hpp"
#include "generation.hpp"

#ifndef UPROAR_PERLIN_OCTIVE_COUNT_DEFAULT
#define UPROAR_PERLIN_OCTIVE_COUNT_DEFAULT 6
#endif

#ifndef UPROAR_PERLIN_LACUNARITY_DEFAULT
#define UPROAR_PERLIN_LACUNARITY_DEFAULT 2.0
#endif

#ifndef UPROAR_PERLIN_PERSISTANCE_DEFAULT
#define UPROAR_PERLIN_PERSISTANCE_DEFAULT 0.5
#endif

#ifndef UPROAR_PERLIN_FREQUENCY_DEFAULT
#define UPROAR_PERLIN_FREQUENCY_DEFAULT 1.0
#endif

#ifndef UPROAR_PERLIN_AMPLITUDE_DEFAULT
#define UPROAR_PERLIN_AMPLITUDE_DEFAULT 1.0
#endif

namespace tc
{
	namespace task
	{
		namespace defaults
		{
			static constexpr UPROAR_OCTAVE_TYPE perlin_octaves{UPROAR_PERLIN_OCTIVE_COUNT_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE perlin_lacunarity{UPROAR_PERLIN_LACUNARITY_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE perlin_persistance{UPROAR_PERLIN_PERSISTANCE_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE perlin_frequency{UPROAR_PERLIN_FREQUENCY_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE perlin_amplitude{UPROAR_PERLIN_AMPLITUDE_DEFAULT};
		} // namespace defaults

		template <typename Noise>
		class UPROAR_API perlin : public generation<perlin<Noise>>
		{
			friend class generation<perlin<Noise>>;

		public:
			using octave_t = UPROAR_OCTAVE_TYPE;
			using decimal_t = UPROAR_DECIMAL_TYPE;

			perlin() = default;

			perlin(
				octave_t octaves,
				decimal_t lacunarity = defaults::perlin_lacunarity,
				decimal_t persistance = defaults::perlin_persistance,
				decimal_t frequency = defaults::perlin_frequency,
				decimal_t amplitude = defaults::perlin_amplitude) UPROAR_NOEXCEPT
				: octaves_{octaves},
				  lacunarity_{lacunarity},
				  persistance_{persistance},
				  frequency_{frequency},
				  amplitude_{amplitude}
			{
			}

			perlin(
				uint32_t seed,
				octave_t octaves,
				decimal_t lacunarity = defaults::perlin_lacunarity,
				decimal_t persistance = defaults::perlin_persistance,
				decimal_t frequency = defaults::perlin_frequency,
				decimal_t amplitude = defaults::perlin_amplitude) UPROAR_NOEXCEPT
				: perlin(octaves, lacunarity, persistance, frequency, amplitude),
				  noise{seed}
			{
			}

			const noise_config &config() const
			{
				return config_;
			}

			void set_config(const noise_config &config)
			{
				config_ = config;
			}

			void set_seed(uint32_t seed) UPROAR_NOEXCEPT
			{
				noise_ = Noise{seed};
			}

		private:
			template <typename... Args>
			decimal_t eval_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				decimal_t result{0.0};
				decimal_t amp{config_.amplitude};
				decimal_t freq{config_.frequency};

				decimal_t weight{0};

				for (auto octave = 0; octave < config_.octaves; ++octave)
				{
					auto tmp = noise_.eval((std::forward<Args>(args) * freq + octave)...) * amp;
					result += tmp;

					// used to normalize values generated.
					weight += amp;

					freq *= config_.lacunarity;
					amp *= config_.persistance;
				}
				result /= weight;

				return result;
			}

			noise_config config_{
				defaults::perlin_octaves,
				defaults::perlin_lacunarity,
				defaults::perlin_persistance,
				defaults::perlin_frequency,
				defaults::perlin_amplitude};

			Noise noise_{};
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_PERLIN_HPP
