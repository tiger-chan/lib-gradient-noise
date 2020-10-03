#ifndef UPROAR_TASKS_MAP_RANGE_HPP
#define UPROAR_TASKS_MAP_RANGE_HPP

#include "../config/config.hpp"
#include "fwd.hpp"
#include "mutation.hpp"
#include "task_source.hpp"
#include <utility>

#ifndef UPROAR_MAP_RANGE_MIN_DEFAULT
#define UPROAR_MAP_RANGE_MIN_DEFAULT -1.0
#endif

#ifndef UPROAR_MAP_RANGE_MAX_DEFAULT
#define UPROAR_MAP_RANGE_MAX_DEFAULT 1.0
#endif

#ifndef UPROAR_MAP_RANGE_LOW_DEFAULT
#define UPROAR_MAP_RANGE_LOW_DEFAULT 0.0
#endif

#ifndef UPROAR_MAP_RANGE_HIGH_DEFAULT
#define UPROAR_MAP_RANGE_HIGH_DEFAULT 1.0
#endif

namespace tc
{
	namespace task
	{
		namespace defaults
		{
			static constexpr decimal_t map_range_min{UPROAR_MAP_RANGE_MIN_DEFAULT};
			static constexpr decimal_t map_range_max{UPROAR_MAP_RANGE_MAX_DEFAULT};
			static constexpr decimal_t map_range_low{UPROAR_MAP_RANGE_LOW_DEFAULT};
			static constexpr decimal_t map_range_high{UPROAR_MAP_RANGE_HIGH_DEFAULT};
		}

		class UPROAR_API map_range : public mutation<map_range>
		{
			friend class mutation<map_range>;
		public:
			map_range() UPROAR_NOEXCEPT = default;
			map_range(task_source src) UPROAR_NOEXCEPT : source_{std::move(src)}
			{
			}

			map_range(task_source src,
					   decimal_t min,
					   decimal_t max = defaults::map_range_max,
					   decimal_t low = defaults::map_range_low,
					   decimal_t high = defaults::map_range_high) UPROAR_NOEXCEPT :
					   source_{std::move(src)},
					   min_{min},
					   max_{max},
					   low_{low},
					   high_{high}
			{
			}

			void configure(const json::object& obj, configure_callback& callback) final
			{
				static const std::string source_key{"source"};
				static const std::string min_key{"min"};
				static const std::string max_key{"max"};
				static const std::string low_key{"low"};
				static const std::string high_key{"high"};

				auto end = std::end(obj);
				auto src_it = obj.find(source_key);
				if (src_it != end) {
					auto src = callback.eval(src_it->second);
					source_ = *src;
				}

				auto min_it = obj.find(min_key);
				if (min_it != end) {
					min_ = min_it->second.as<decimal_t>();
				}

				auto max_it = obj.find(max_key);
				if (max_it != end) {
					max_ = max_it->second.as<decimal_t>();
				}

				auto low_it = obj.find(low_key);
				if (low_it != end) {
					low_ = low_it->second.as<decimal_t>();
				}

				auto high_it = obj.find(high_key);
				if (high_it != end) {
					high_ = high_it->second.as<decimal_t>();
				}
			}

			void set_min(decimal_t min) {
				min_ = min;
			}

			void set_max(decimal_t max) {
				max_ = max;
			}

			void set_low(decimal_t low) {
				low_ = low;
			}

			void set_high(decimal_t high) {
				high_ = high;
			}

			void source(task_source source) {
				source_ = std::move(source);
			}
			
		private:
			task_source source_{decimal_t{0}};
			decimal_t min_{defaults::map_range_min};
			decimal_t max_{defaults::map_range_max};
			decimal_t low_{defaults::map_range_low};
			decimal_t high_{defaults::map_range_high};

			template <typename... Args>
			decimal_t eval_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				auto v = source_.eval(std::forward<Args>(args)...);
				auto orig_range = max_ - min_;
				auto new_range = high_ - low_;
				auto s = (v - min_) / orig_range;
				auto b = low_;
				return s * new_range + b;
			}
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_MAP_RANGE_HPP
