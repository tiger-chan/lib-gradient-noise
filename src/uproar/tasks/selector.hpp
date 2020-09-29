#ifndef UPROAR_TASKS_SELECTOR_HPP
#define UPROAR_TASKS_SELECTOR_HPP

#include "../config/config.hpp"
#include "../core/algorithm.hpp"
#include "../core/curve_blend.hpp"
#include "fwd.hpp"
#include "base_task.hpp"
#include "task_source.hpp"
#include <utility>

#ifndef UPROAR_SELECTOR_THRESHOLD_DEFAULT
#define UPROAR_SELECTOR_THRESHOLD_DEFAULT 0.0
#endif

#ifndef UPROAR_SELECTOR_FALLOFF_DEFAULT
#define UPROAR_SELECTOR_FALLOFF_DEFAULT 0.0
#endif

namespace tc
{
	namespace task
	{
		namespace defaults
		{
			static constexpr UPROAR_DECIMAL_TYPE selector_threshold{UPROAR_SELECTOR_THRESHOLD_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE selector_falloff{UPROAR_SELECTOR_FALLOFF_DEFAULT};
		} // namespace defaults

		template <typename Blender>
		class selector : public base_task
		{
		public:
			using decimal_t = UPROAR_DECIMAL_TYPE;

			selector() UPROAR_NOEXCEPT = default;

			selector(task_source selector, task_source src0, task_source src1) UPROAR_NOEXCEPT : selector_{std::move(selector)},
																								 source_0_{std::move(src0)},
																								 source_1_{std::move(src1)}
			{
			}

			selector(
				const task_source &selector,
				const task_source &src0,
				const task_source &src1,
				task_source threshold,
				task_source falloff = task_source{defaults::selector_falloff}) UPROAR_NOEXCEPT : selector(selector, src0, src1),
																								 threshold_{std::move(threshold)},
																								 falloff_{std::move(falloff)}

			{
			}

			void set_selector(task_source selector) UPROAR_NOEXCEPT
			{
				selector_ = std::move(selector);
			}

			void set_source_0(task_source source) UPROAR_NOEXCEPT
			{
				source_0_ = std::move(source);
			}

			void set_source_1(task_source source) UPROAR_NOEXCEPT
			{
				source_1_ = std::move(source);
			}

			void set_threshold(task_source threshold) UPROAR_NOEXCEPT
			{
				threshold_ = std::move(threshold);
			}

			void set_falloff(task_source falloff) UPROAR_NOEXCEPT
			{
				falloff_ = std::move(falloff);
			}

			decimal_t eval(decimal_t x) const override
			{
				return eval_impl(x);
			}

			decimal_t eval(decimal_t x, decimal_t y) const override
			{
				return eval_impl(x, y);
			}

			decimal_t eval(decimal_t x, decimal_t y, decimal_t z) const override
			{
				return eval_impl(x, y, z);
			}

		private:
			task_source source_0_{decimal_t{-1}};
			task_source source_1_{decimal_t{1}};
			task_source selector_{decimal_t{0.5}};
			task_source threshold_{defaults::selector_threshold};
			task_source falloff_{defaults::selector_falloff};
			Blender blender{};

			template <typename... Args>
			decimal_t eval_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				auto s = selector_.eval(std::forward<Args>(args)...);
				auto f = falloff_.eval(std::forward<Args>(args)...);
				auto t = threshold_.eval(std::forward<Args>(args)...);

				// no falloff, just select source.
				if (f <= 0)
				{
					if (s < t)
					{
						auto s0 = source_0_.eval(std::forward<Args>(args)...);
						return s0;
					}
					auto s1 = source_1_.eval(std::forward<Args>(args)...);
					return s1;
				}

				// first check if the value is within the falloff
				// if not just select source.
				// lower bound
				auto lb = t - f;
				if (s < lb)
				{
					auto s0 = source_0_.eval(std::forward<Args>(args)...);
					return s0;
				}

				// upper bound
				auto ub = t + f;
				if (s > ub)
				{
					auto s1 = source_1_.eval(std::forward<Args>(args)...);
					return s1;
				}

				// blend
				auto b = blender((s - lb) / (ub - lb));
				auto s0 = source_0_.eval(std::forward<Args>(args)...);
				auto s1 = source_1_.eval(std::forward<Args>(args)...);
				auto r = lerp(s0, s1, b);
				return r;
			}
		};

		using selector_cubic = selector<cubic_blend>;
		using selector_quintic = selector<quintic_blend>;
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_SELECTOR_HPP
