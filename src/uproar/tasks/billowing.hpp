#ifndef UPROAR_TASKS_BILLOWING_HPP
#define UPROAR_TASKS_BILLOWING_HPP

#include "../config/config.hpp"
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
		class UPROAR_API billowing : public generation_task
		{
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
				: octaves_{octaves},
				  lacunarity_{lacunarity},
				  persistance_{persistance},
				  frequency_{frequency},
				  amplitude_{amplitude}
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

			billowing& set_octaves(octave_t octaves) UPROAR_NOEXCEPT {
				return octaves_ = octaves, *this;
			}

			auto octaves() const UPROAR_NOEXCEPT {
				return octaves_;
			}

			billowing& set_lacunarity(decimal_t lacunarity) UPROAR_NOEXCEPT {
				return lacunarity_ = lacunarity, *this;
			}

			auto lacunarity() const UPROAR_NOEXCEPT {
				return lacunarity_;
			}

			billowing& set_persistance(decimal_t persistance) UPROAR_NOEXCEPT {
				return persistance_ = persistance, *this;
			}

			auto persistance() const UPROAR_NOEXCEPT {
				return persistance_;
			}

			billowing& set_frequency(decimal_t frequency) UPROAR_NOEXCEPT {
				return frequency_ = frequency, *this;
			}

			auto frequency() const UPROAR_NOEXCEPT {
				return frequency_;
			}

			billowing& set_amplitude(decimal_t amplitude) UPROAR_NOEXCEPT {
				return amplitude_ = amplitude, *this;
			}

			auto amplitude() const UPROAR_NOEXCEPT {
				return amplitude_;
			}

			billowing& set_seed(uint32_t seed) UPROAR_NOEXCEPT {
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
			UPROAR_OCTAVE_TYPE octaves_{defaults::billowing_octaves};
			decimal_t lacunarity_{defaults::billowing_lacunarity};
			decimal_t persistance_{defaults::billowing_persistance};
			decimal_t frequency_{defaults::billowing_frequency};
			decimal_t amplitude_{defaults::billowing_amplitude};
			Noise noise_{};

		private:
			template <typename... Args>
			decimal_t eval_normalized_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				static constexpr decimal_t one{1};
				static constexpr decimal_t two{2};
				decimal_t result{0.0};
				decimal_t amp{amplitude_};
				decimal_t freq{frequency_};

				decimal_t weight{0};

				for (auto octave = 0; octave < octaves_; ++octave)
				{
					auto tmp = noise_.eval((std::forward<Args>(args) * freq + octave)...);
					tmp = two * fabs(tmp) - one;
					result += tmp * amp;
					
					// used to normalize values generated.
					weight += amp;

					freq *= lacunarity_;
					amp *= persistance_;
					
				}
				result /= weight;
				result += one / two;

				return result;
			}
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_BILLOWING_HPP
