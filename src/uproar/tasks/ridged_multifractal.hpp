#ifndef UPROAR_TASKS_RIDGED_MULTIFRACTAL_HPP
#define UPROAR_TASKS_RIDGED_MULTIFRACTAL_HPP

#include "../config/config.hpp"
#include "fwd.hpp"
#include "generation.hpp"
#include <array>

#ifndef RIDGED_MULTIFRACTAL_OCTIVE_COUNT_DEFAULT
#define RIDGED_MULTIFRACTAL_OCTIVE_COUNT_DEFAULT 6
#endif

#ifndef RIDGED_MULTIFRACTAL_LACUNARITY_DEFAULT
#define RIDGED_MULTIFRACTAL_LACUNARITY_DEFAULT 2.0
#endif

#ifndef RIDGED_MULTIFRACTAL_PERSISTANCE_DEFAULT
#define RIDGED_MULTIFRACTAL_PERSISTANCE_DEFAULT 0.5
#endif

#ifndef RIDGED_MULTIFRACTAL_FREQUENCY_DEFAULT
#define RIDGED_MULTIFRACTAL_FREQUENCY_DEFAULT 1.0
#endif

#ifndef RIDGED_MULTIFRACTAL_EXPONENT_DEFAULT
#define RIDGED_MULTIFRACTAL_EXPONENT_DEFAULT 0.9
#endif

#ifndef RIDGED_MULTIFRACTAL_OFFSET_DEFAULT
#define RIDGED_MULTIFRACTAL_OFFSET_DEFAULT 1.0
#endif

#ifndef RIDGED_MULTIFRACTAL_MAX_OCTAVES_DEFAULT
#define RIDGED_MULTIFRACTAL_MAX_OCTAVES_DEFAULT 30
#endif

namespace tc
{
	namespace task
	{
		namespace defaults
		{
			static constexpr UPROAR_OCTAVE_TYPE ridged_multifractal_octaves{RIDGED_MULTIFRACTAL_OCTIVE_COUNT_DEFAULT};
			static constexpr UPROAR_OCTAVE_TYPE ridged_multifractal_max_octaves{RIDGED_MULTIFRACTAL_MAX_OCTAVES_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE ridged_multifractal_lacunarity{RIDGED_MULTIFRACTAL_LACUNARITY_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE ridged_multifractal_persistance{RIDGED_MULTIFRACTAL_PERSISTANCE_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE ridged_multifractal_frequency{RIDGED_MULTIFRACTAL_FREQUENCY_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE ridged_multifractal_exponent{RIDGED_MULTIFRACTAL_EXPONENT_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE ridged_multifractal_offset{RIDGED_MULTIFRACTAL_OFFSET_DEFAULT};
		} // namespace defaults

		template <typename Noise>
		class UPROAR_API ridged_multifractal : public generation_task
		{
		public:
			using octave_t = UPROAR_OCTAVE_TYPE;
			using decimal_t = UPROAR_DECIMAL_TYPE;

			struct correction
			{
				decimal_t scale{0};
				decimal_t bias{0};
			};

			ridged_multifractal() UPROAR_NOEXCEPT
			{
				calc_weights();
			}

			ridged_multifractal(
				octave_t octaves,
				decimal_t lacunarity = defaults::ridged_multifractal_lacunarity,
				decimal_t persistance = defaults::ridged_multifractal_persistance,
				decimal_t frequency = defaults::ridged_multifractal_frequency,
				decimal_t exponent = defaults::ridged_multifractal_exponent,
				decimal_t offset = defaults::ridged_multifractal_exponent) UPROAR_NOEXCEPT
				: octaves_{octaves},
				  lacunarity_{lacunarity},
				  persistance_{persistance},
				  frequency_{frequency},
				  exponent_{exponent},
				  offset_{offset}
			{
				calc_weights();
			}

			ridged_multifractal(
				uint32_t seed,
				octave_t octaves,
				decimal_t lacunarity = defaults::ridged_multifractal_lacunarity,
				decimal_t persistance = defaults::ridged_multifractal_persistance,
				decimal_t frequency = defaults::ridged_multifractal_frequency,
				decimal_t exponent = defaults::ridged_multifractal_exponent,
				decimal_t offset = defaults::ridged_multifractal_exponent) UPROAR_NOEXCEPT
				: ridged_multifractal(octaves, lacunarity, persistance, frequency, exponent, offset), noise{seed}
			{
			}

			ridged_multifractal &set_octaves(octave_t octaves) UPROAR_NOEXCEPT
			{
				return octaves_ = octaves, *this;
			}

			auto octaves() const UPROAR_NOEXCEPT
			{
				return octaves_;
			}

			ridged_multifractal &set_lacunarity(decimal_t lacunarity) UPROAR_NOEXCEPT
			{
				return lacunarity_ = lacunarity, calc_weights(), *this;
			}

			auto lacunarity() const UPROAR_NOEXCEPT
			{
				return lacunarity_;
			}

			ridged_multifractal &set_offset(decimal_t offset) UPROAR_NOEXCEPT
			{
				return offset_ = offset, calc_weights(), *this;
			}

			auto offset() const UPROAR_NOEXCEPT
			{
				return offset_;
			}

			ridged_multifractal &set_persistance(decimal_t persistance) UPROAR_NOEXCEPT
			{
				return persistance_ = persistance, *this;
			}

			auto persistance() const UPROAR_NOEXCEPT
			{
				return persistance_;
			}

			ridged_multifractal &set_frequency(decimal_t frequency) UPROAR_NOEXCEPT
			{
				return frequency_ = frequency, *this;
			}

			auto frequency() const UPROAR_NOEXCEPT
			{
				return frequency_;
			}

			ridged_multifractal &set_seed(uint32_t seed) UPROAR_NOEXCEPT
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
			UPROAR_OCTAVE_TYPE octaves_{defaults::ridged_multifractal_octaves};
			decimal_t lacunarity_{defaults::ridged_multifractal_lacunarity};
			decimal_t persistance_{defaults::ridged_multifractal_persistance};
			decimal_t frequency_{defaults::ridged_multifractal_frequency};
			decimal_t exponent_{defaults::ridged_multifractal_exponent};
			decimal_t offset_{defaults::ridged_multifractal_offset};
			std::array<decimal_t, defaults::ridged_multifractal_max_octaves> weights_{};
			std::array<correction, defaults::ridged_multifractal_max_octaves> corrections_{};
			Noise noise_{};

		private:
			template <typename... Args>
			decimal_t eval_normalized_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				decimal_t result{0.0};
				decimal_t freq{frequency_};

				for (auto octave = 0; octave < octaves_; ++octave)
				{
					auto tmp = noise_.eval((std::forward<Args>(args) * freq + octave)...);
					tmp = offset_ - fabs(tmp);
					tmp *= tmp;

					result += tmp * weights_[octave];

					freq *= lacunarity_;
				}

				auto &corrections = corrections_[octaves_ - 1];
				return result * corrections.scale + corrections.bias;
			}

			void calc_weights() UPROAR_NOEXCEPT
			{
				static constexpr decimal_t one{1};
				decimal_t min{0};
				decimal_t max{0};
				decimal_t freq{lacunarity_};
				for (auto i = 0; i < defaults::ridged_multifractal_max_octaves; ++i)
				{
					weights_[i] = pow(freq, -exponent_);
					freq *= lacunarity_;

					min += (offset_ - one) * (offset_ - one) * weights_[i];
					max += offset_ * offset_ * weights_[i];
					decimal_t a{-1}, b{1};
					decimal_t scale = (b - a) / (max - min);
					decimal_t bias = a - (min * scale);
					corrections_[i] = correction{scale, bias};
				}
			}
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_RIDGED_MULTIFRACTAL_HPP
