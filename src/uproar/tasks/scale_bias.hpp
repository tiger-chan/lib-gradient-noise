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

		class UPROAR_API scale_bias : public mutation<scale_bias>
		{
			friend class mutation<scale_bias>;
		public:
			scale_bias() UPROAR_NOEXCEPT = default;
			scale_bias(task_source src) UPROAR_NOEXCEPT : source_{std::move(src)}
			{
			}

			scale_bias(task_source src,
					   task_source scale,
					   task_source bias = task_source{defaults::scale_bias_bias}) UPROAR_NOEXCEPT : source_{std::move(src)},
																				  scale_{std::move(scale)}, bias_{std::move(bias)}
			{
			}

			void scale(task_source scale) {
				scale_ = std::move(scale);
			}

			void bias(task_source bias) {
				bias_ = std::move(bias);
			}

			void source(task_source source) {
				source_ = std::move(source);
			}
			
		private:
			task_source source_{decimal_t{0}};
			task_source scale_{defaults::scale_bias_scale};
			task_source bias_{defaults::scale_bias_bias};

			template <typename... Args>
			decimal_t eval_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				auto v = source_.eval(std::forward<Args>(args)...);
				auto s = scale_.eval(std::forward<Args>(args)...);
				auto b = bias_.eval(std::forward<Args>(args)...);
				return v * s + b;
			}
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_SCALE_BIAS_HPP
