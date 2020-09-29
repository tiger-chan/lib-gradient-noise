#ifndef UPROAR_TASKS_SCALE_BIAS_HPP
#define UPROAR_TASKS_SCALE_BIAS_HPP

#include "../config/config.hpp"
#include "fwd.hpp"
#include "mutation.hpp"
#include "task_source.hpp"
#include <utility>

#ifndef UPROAR_SCALE_BIAS_SCALE_DEFAULT
#define UPROAR_SCALE_BIAS_SCALE_DEFAULT 1.0
#endif

#ifndef UPROAR_SCALE_BIAS_BIAS_DEFAULT
#define UPROAR_SCALE_BIAS_BIAS_DEFAULT 0.0
#endif

namespace tc
{
	namespace task
	{
		namespace defaults
		{
			static constexpr UPROAR_DECIMAL_TYPE scale_bias_scale{UPROAR_SCALE_BIAS_SCALE_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE scale_bias_bias{UPROAR_SCALE_BIAS_BIAS_DEFAULT};
		}

		class UPROAR_API scale_bias : public mutation_task
		{
		public:
			using decimal_t = UPROAR_DECIMAL_TYPE;

			scale_bias() UPROAR_NOEXCEPT = default;
			scale_bias(task_source src) UPROAR_NOEXCEPT : value_{std::move(src)}
			{
			}

			scale_bias(task_source src,
					   task_source scale,
					   task_source bias = task_source{defaults::scale_bias_bias}) UPROAR_NOEXCEPT : value_{std::move(src)},
																				  scale_{std::move(scale)}, bias_{std::move(bias)}
			{
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
			task_source value_{decimal_t{0}};
			task_source scale_{defaults::scale_bias_scale};
			task_source bias_{defaults::scale_bias_scale};

			template <typename... Args>
			decimal_t eval_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				auto v = value_.eval(std::forward<Args>(args)...);
				auto s = scale_.eval(std::forward<Args>(args)...);
				auto b = bias_.eval(std::forward<Args>(args)...);
				return v * s + b;
			}
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_SCALE_BIAS_HPP
