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

			selector(task_source control, task_source low, task_source high) UPROAR_NOEXCEPT : switch_{std::move(control)},
																							   low_{std::move(low)},
																							   high_{std::move(high)}
			{
			}

			selector(
				const task_source &control,
				const task_source &low,
				const task_source &high,
				task_source threshold,
				task_source falloff = task_source{defaults::selector_falloff}) UPROAR_NOEXCEPT : selector(control, low, high),
																								 threshold_{std::move(threshold)},
																								 falloff_{std::move(falloff)}

			{
			}

			void set_switch(task_source src) UPROAR_NOEXCEPT
			{
				switch_ = std::move(src);
			}

			void set_low(task_source src) UPROAR_NOEXCEPT
			{
				low_ = std::move(src);
			}

			void set_high(task_source src) UPROAR_NOEXCEPT
			{
				high_ = std::move(src);
			}

			void set_threshold(task_source src) UPROAR_NOEXCEPT
			{
				threshold_ = std::move(src);
			}

			void set_falloff(task_source src) UPROAR_NOEXCEPT
			{
				falloff_ = std::move(src);
			}

			decimal_t eval(decimal_t x) const final
			{
				return eval_impl(x);
			}

			decimal_t eval(decimal_t x, decimal_t y) const final
			{
				return eval_impl(x, y);
			}

			decimal_t eval(decimal_t x, decimal_t y, decimal_t z) const final
			{
				return eval_impl(x, y, z);
			}

		private:
			template <typename... Args>
			decimal_t eval_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				auto s = switch_.eval(std::forward<Args>(args)...);
				auto f = falloff_.eval(std::forward<Args>(args)...);
				auto t = threshold_.eval(std::forward<Args>(args)...);

				// no falloff, just select source.
				if (f <= 0)
				{
					if (s < t)
					{
						auto s0 = low_.eval(std::forward<Args>(args)...);
						return s0;
					}
					auto s1 = high_.eval(std::forward<Args>(args)...);
					return s1;
				}

				// first check if the value is within the falloff
				// if not just select source.
				// lower bound
				auto lb = t - f;
				if (s < lb)
				{
					auto s0 = low_.eval(std::forward<Args>(args)...);
					return s0;
				}

				// upper bound
				auto ub = t + f;
				if (s > ub)
				{
					auto s1 = high_.eval(std::forward<Args>(args)...);
					return s1;
				}

				// blend
				auto b = blender((s - lb) / (ub - lb));
				auto s0 = low_.eval(std::forward<Args>(args)...);
				auto s1 = high_.eval(std::forward<Args>(args)...);
				auto r = lerp(s0, s1, b);
				return r;
			}

			task_source low_{decimal_t{-1}};
			task_source high_{decimal_t{1}};
			task_source switch_{decimal_t{0.5}};
			task_source threshold_{defaults::selector_threshold};
			task_source falloff_{defaults::selector_falloff};
			Blender blender{};
		};

		template <typename Blender>
		struct config<selector<Blender>>
		{
			void operator()(selector<Blender> &task, const json::object &obj, configure_callback &callback) const
			{
				static const std::string switch_key{"switch"};
				static const std::string low_key{"low"};
				static const std::string high_key{"high"};
				static const std::string threshold_key{"threshold"};
				static const std::string falloff_key{"falloff"};

				auto end = std::end(obj);
				auto src_it = obj.find(switch_key);
				if (src_it != end)
				{
					auto src = callback.eval(src_it->second);
					task.set_switch(*src);
				}

				src_it = obj.find(low_key);
				if (src_it != end)
				{
					auto src = callback.eval(src_it->second);
					task.set_low(*src);
				}

				src_it = obj.find(high_key);
				if (src_it != end)
				{
					auto src = callback.eval(src_it->second);
					task.set_high(*src);
				}

				src_it = obj.find(threshold_key);
				if (src_it != end)
				{
					auto src = callback.eval(src_it->second);
					task.set_threshold(*src);
				}

				src_it = obj.find(falloff_key);
				if (src_it != end)
				{
					auto src = callback.eval(src_it->second);
					task.set_falloff(*src);
				}
			}
		};

		using selector_cubic = selector<cubic_blend>;
		using selector_quintic = selector<quintic_blend>;
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_SELECTOR_HPP
