#ifndef UPROAR_TASKS_BIAS_HPP
#define UPROAR_TASKS_BIAS_HPP

#include "../config/config.hpp"
#include "../core/easing.hpp"
#include "fwd.hpp"
#include "mutation.hpp"
#include "task_source.hpp"
#include <utility>

#ifndef UPROAR_BIAS_BIAS_DEFAULT
#define UPROAR_BIAS_BIAS_DEFAULT 0.5
#endif

#ifndef UPROAR_BIAS_EXPONENT_MAX
#define UPROAR_BIAS_EXPONENT_MAX 4.0
#endif

#ifndef UPROAR_BIAS_EXPONENT_MIN
#define UPROAR_BIAS_EXPONENT_MIN 1.0
#endif

namespace tc
{
	namespace task
	{
		namespace defaults
		{
			static constexpr UPROAR_DECIMAL_TYPE bias_bias{UPROAR_BIAS_BIAS_DEFAULT};
			static constexpr UPROAR_DECIMAL_TYPE bias_exponent_min{UPROAR_BIAS_EXPONENT_MIN};
			static constexpr UPROAR_DECIMAL_TYPE bias_exponent_max{UPROAR_BIAS_EXPONENT_MAX};
		}

		class UPROAR_API bias_task : public mutation<bias_task>
		{
			friend class mutation<bias_task>;

		public:
			bias_task() UPROAR_NOEXCEPT = default;
			bias_task(task_source src) UPROAR_NOEXCEPT : source_{std::move(src)}
			{
			}

			bias_task(task_source src, task_source bias) UPROAR_NOEXCEPT : source_{std::move(src)}, bias_{std::move(bias)}
			{
			}

			void configure(const json::object &obj, configure_callback &callback) final
			{
				static const std::string source_key{"source"};
				static const std::string bias_key{"bias"};

				auto end = std::end(obj);
				auto src_it = obj.find(source_key);
				if (src_it != end)
				{
					auto src = callback.eval(src_it->second);
					source_ = *src;
				}

				auto bias_it = obj.find(bias_key);
				if (bias_it != end)
				{
					auto src = callback.eval(bias_it->second);
					bias_ = *src;
				}
			}

			void bias(task_source src)
			{
				bias_ = std::move(src);
			}

			void source(task_source source)
			{
				source_ = std::move(source);
			}

		private:
			task_source source_{decimal_t{0}};
			task_source bias_{defaults::bias_bias};

			template <typename... Args>
			decimal_t eval_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				auto v = source_.eval(std::forward<Args>(args)...);
				auto b = bias_.eval(std::forward<Args>(args)...);
				// ease in out with higher exponents will push the values further towards the extremes
				auto p = (b * defaults::bias_exponent_max) + defaults::bias_exponent_min;
				return ease_in_out(v, p);
			}
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_BIAS_HPP
