#ifndef UPROAR_TASKS_BILLOWING_HPP
#define UPROAR_TASKS_BILLOWING_HPP

#include "../config/config.hpp"
#include "../core/noise_config.hpp"
#include "fwd.hpp"
#include "generation.hpp"

#ifndef BILLOWING_OCTIVE_COUNT_DEFAULT
#define BILLOWING_OCTIVE_COUNT_DEFAULT 6
#endif

#ifndef BILLOWING_LACUNARITY_DEFAULT
#define BILLOWING_LACUNARITY_DEFAULT 2.0
#endif

#ifndef BILLOWING_PERSISTANCE_DEFAULT
#define BILLOWING_PERSISTANCE_DEFAULT 0.5
#endif

#ifndef BILLOWING_FREQUENCY_DEFAULT
#define BILLOWING_FREQUENCY_DEFAULT 1.0
#endif

#ifndef BILLOWING_AMPLITUDE_DEFAULT
#define BILLOWING_AMPLITUDE_DEFAULT 1.0
#endif

namespace tc
{
	namespace task
	{
		namespace defaults
		{
			static constexpr UPROAR_OCTAVE_TYPE billowing_octaves{BILLOWING_OCTIVE_COUNT_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE billowing_lacunarity{BILLOWING_LACUNARITY_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE billowing_persistance{BILLOWING_PERSISTANCE_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE billowing_frequency{BILLOWING_FREQUENCY_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE billowing_amplitude{BILLOWING_AMPLITUDE_DEFAULT};
		}

		template <typename Noise>
		class UPROAR_API billowing : public generation<billowing<Noise>>, public fractal_task
		{
			friend class generation<billowing<Noise>>;
		public:
			using octave_t = UPROAR_OCTAVE_TYPE;
			using decimal_t = UPROAR_DECIMAL_TYPE;

			billowing() = default;

			billowing(
				octave_t octaves,
				decimal_t lacunarity = defaults::billowing_lacunarity,
				decimal_t persistance = defaults::billowing_persistance,
				decimal_t frequency = defaults::billowing_frequency,
				decimal_t amplitude = defaults::billowing_amplitude) UPROAR_NOEXCEPT
				: config_{ octaves, lacunarity, persistance, frequency, amplitude }
			{
			}

			billowing(
				uint32_t seed,
				octave_t octaves,
				decimal_t lacunarity = defaults::billowing_lacunarity,
				decimal_t persistance = defaults::billowing_persistance,
				decimal_t frequency = defaults::billowing_frequency,
				decimal_t amplitude = defaults::billowing_amplitude) UPROAR_NOEXCEPT
				: billowing(octaves, lacunarity, persistance, frequency, amplitude), noise{seed}
			{
			}

			const noise_config& config() const
			{
				return config_;
			}

			void set_config(const noise_config& config)
			{
				config_ = config;
			}

			void set_seed(uint32_t seed) UPROAR_NOEXCEPT final
			{
				fractal_task::set_seed(seed);
				noise_ = Noise{seed};
			}

		protected:
			noise_config config_{
				defaults::billowing_octaves,
				defaults::billowing_lacunarity,
				defaults::billowing_persistance,
				defaults::billowing_frequency,
				defaults::billowing_amplitude
			};

			Noise noise_{};

		private:
			template <typename... Args>
			decimal_t eval_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				static constexpr decimal_t one{1};
				static constexpr decimal_t two{2};
				decimal_t result{0.0};
				decimal_t amp{config_.amplitude};
				decimal_t freq{config_.frequency};

				decimal_t weight{0};

				for (auto octave = 0; octave < config_.octaves; ++octave)
				{
					auto tmp = noise_.eval((std::forward<Args>(args) * freq + octave)...);
					tmp = two * fabs(tmp) - one;
					result += tmp * amp;
					
					// used to normalize values generated.
					weight += amp;

					freq *= config_.lacunarity;
					amp *= config_.persistance;
					
				}
				result /= weight;
				result += one / two;

				return result;
			}
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_BILLOWING_HPP
