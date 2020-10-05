#ifndef UPROAR_TASKS_RIDGED_MULTIFRACTAL_HPP
#define UPROAR_TASKS_RIDGED_MULTIFRACTAL_HPP

#include "../config/config.hpp"
#include "../core/noise_config.hpp"
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
		class UPROAR_API ridged_multifractal : public generation<ridged_multifractal<Noise>>
		{
			friend class generation<ridged_multifractal<Noise>>;

		public:
			using octave_t = UPROAR_OCTAVE_TYPE;
			using decimal_t = UPROAR_DECIMAL_TYPE;

			struct UPROAR_API ridged_multi_config : public noise_config
			{
				decimal_t exponent{defaults::ridged_multifractal_exponent};
				decimal_t offset{defaults::ridged_multifractal_offset};

				void ex_configure(const json::object &obj)
				{
					static const std::string exponent_key{"exponent"};
					static const std::string offset_key{"exponent"};

					auto end = std::end(obj);
					auto exp = obj.find(exponent_key);
					if (exp != end)
					{
						exponent = exp->second.as<decimal_t>();
					}

					auto off = obj.find(offset_key);
					if (off != end)
					{
						offset = off->second.as<decimal_t>();
					}

					configure(obj);
				}
			};

			struct correction
			{
				decimal_t scale{0};
				decimal_t bias{0};
			};

			ridged_multifractal() UPROAR_NOEXCEPT
			{
				calc_weights(config_);
			}

			ridged_multifractal(
				octave_t octaves,
				decimal_t lacunarity = defaults::ridged_multifractal_lacunarity,
				decimal_t persistance = defaults::ridged_multifractal_persistance,
				decimal_t frequency = defaults::ridged_multifractal_frequency,
				decimal_t exponent = defaults::ridged_multifractal_exponent,
				decimal_t offset = defaults::ridged_multifractal_offset) UPROAR_NOEXCEPT
				: config_{octaves, lacunarity, persistance, frequency, 0, exponent, offset}
			{
				calc_weights(config_);
			}

			ridged_multifractal(
				uint32_t seed,
				octave_t octaves,
				decimal_t lacunarity = defaults::ridged_multifractal_lacunarity,
				decimal_t persistance = defaults::ridged_multifractal_persistance,
				decimal_t frequency = defaults::ridged_multifractal_frequency,
				decimal_t exponent = defaults::ridged_multifractal_exponent,
				decimal_t offset = defaults::ridged_multifractal_exponent) UPROAR_NOEXCEPT
				: ridged_multifractal(octaves, lacunarity, persistance, frequency, exponent, offset),
				  noise{seed}
			{
			}

			const ridged_multi_config &config() const
			{
				return config_;
			}

			void set_config(const ridged_multi_config &config)
			{
				if (config.lacunarity != config_.lacunarity || config.offset != config_.offset || config.exponent != config.exponent)
				{
					calc_weights(config);
				}
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
				decimal_t freq{config_.frequency};

				for (auto octave = 0; octave < config_.octaves; ++octave)
				{
					auto tmp = noise_.eval((std::forward<Args>(args) * freq + octave)...);
					tmp = config_.offset - fabs(tmp);
					tmp *= tmp;

					result += tmp * weights_[octave];

					freq *= config_.lacunarity;
				}

				auto &corrections = corrections_[config_.octaves - 1];
				return result * corrections.scale + corrections.bias;
			}

			void calc_weights(const ridged_multi_config &config) UPROAR_NOEXCEPT
			{
				static constexpr decimal_t one{1};
				decimal_t min{0};
				decimal_t max{0};
				decimal_t freq{config.lacunarity};
				for (auto i = 0; i < defaults::ridged_multifractal_max_octaves; ++i)
				{
					weights_[i] = pow(freq, -config.exponent);
					freq *= config.lacunarity;

					min += (config.offset - one) * (config.offset - one) * weights_[i];
					max += config.offset * config.offset * weights_[i];
					decimal_t a{-1}, b{1};
					decimal_t scale = (b - a) / (max - min);
					decimal_t bias = a - (min * scale);
					corrections_[i] = correction{scale, bias};
				}
			}

			ridged_multi_config config_{
				defaults::ridged_multifractal_octaves,
				defaults::ridged_multifractal_lacunarity,
				defaults::ridged_multifractal_persistance,
				defaults::ridged_multifractal_frequency,
				0,
				defaults::ridged_multifractal_exponent,
				defaults::ridged_multifractal_offset,
			};
			Noise noise_{};

			std::array<decimal_t, defaults::ridged_multifractal_max_octaves> weights_{};
			std::array<correction, defaults::ridged_multifractal_max_octaves> corrections_{};
		};

		template <typename Noise>
		struct config<ridged_multifractal<Noise>>
		{
			void operator()(ridged_multifractal<Noise> &task, const json::object &obj, configure_callback &callback) const
			{
				static const std::string seed_key{"seed"};

				auto src_it = obj.find(seed_key);
				if (src_it != std::end(obj))
				{
					task.set_seed(src_it->second.as<uint32_t>());
				}

				auto config = task.config();
				config.ex_configure(obj);
				task.set_config(config);
			}
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_RIDGED_MULTIFRACTAL_HPP
