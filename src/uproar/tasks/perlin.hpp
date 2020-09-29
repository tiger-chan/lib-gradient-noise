#ifndef UPROAR_TASKS_PERLIN_HPP
#define UPROAR_TASKS_PERLIN_HPP

#include "../config/config.hpp"
#include "fwd.hpp"
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
		class UPROAR_API perlin : public generation_task
		{
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
				: perlin(octaves, lacunarity, persistance, frequency, amplitude), noise{seed}
			{
			}

			perlin &set_octaves(octave_t octaves) UPROAR_NOEXCEPT
			{
				return octaves_ = octaves, *this;
			}

			auto octaves() const UPROAR_NOEXCEPT
			{
				return octaves_;
			}

			perlin &set_lacunarity(decimal_t lacunarity) UPROAR_NOEXCEPT
			{
				return lacunarity_ = lacunarity, *this;
			}

			auto lacunarity() const UPROAR_NOEXCEPT
			{
				return lacunarity_;
			}

			perlin &set_persistance(decimal_t persistance) UPROAR_NOEXCEPT
			{
				return persistance_ = persistance, *this;
			}

			auto persistance() const UPROAR_NOEXCEPT
			{
				return persistance_;
			}

			perlin &set_frequency(decimal_t frequency) UPROAR_NOEXCEPT
			{
				return frequency_ = frequency, *this;
			}

			auto frequency() const UPROAR_NOEXCEPT
			{
				return frequency_;
			}

			perlin &set_amplitude(decimal_t amplitude) UPROAR_NOEXCEPT
			{
				return amplitude_ = amplitude, *this;
			}

			auto amplitude() const UPROAR_NOEXCEPT
			{
				return amplitude_;
			}

			perlin &set_seed(uint32_t seed) UPROAR_NOEXCEPT
			{
				return noise_ = Noise{seed}, *this;
			}

			decimal_t eval(decimal_t x) const override
			{
				return eval_normalized_impl(x);
			}

			decimal_t eval(decimal_t x, decimal_t y) const override
			{
				return eval_normalized_impl(x, y);
			}

			decimal_t eval(decimal_t x, decimal_t y, decimal_t z) const override
			{
				return eval_normalized_impl(x, y, z);
			}

		protected:
			UPROAR_OCTAVE_TYPE octaves_{defaults::perlin_octaves};
			decimal_t lacunarity_{defaults::perlin_lacunarity};
			decimal_t persistance_{defaults::perlin_persistance};
			decimal_t frequency_{defaults::perlin_frequency};
			decimal_t amplitude_{defaults::perlin_amplitude};
			Noise noise_{};

		private:
			template <typename... Args>
			decimal_t eval_normalized_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				decimal_t result{0.0};
				decimal_t amp{amplitude_};
				decimal_t freq{frequency_};

				decimal_t weight{0};

				for (auto octave = 0; octave < octaves_; ++octave)
				{
					auto tmp = noise_.eval((std::forward<Args>(args) * freq + octave)...) * amp;
					result += tmp;

					// used to normalize values generated.
					weight += amp;

					freq *= lacunarity_;
					amp *= persistance_;
				}
				result /= weight;

				return result;
			}
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_PERLIN_HPP
