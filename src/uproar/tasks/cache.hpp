#ifndef UPROAR_TASKS_CACHE_HPP
#define UPROAR_TASKS_CACHE_HPP

#include <array>
#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../core/utlities.hpp"
#include "fwd.hpp"
#include "base_task.hpp"
#include "task_source.hpp"

namespace tc
{
	namespace task
	{
		namespace defaults
		{
			static constexpr uint8_t cache_max_dimension{UPROAR_MAX_VARIABLES};
			static constexpr uint8_t cache_max_variables{UPROAR_MAX_VARIABLES};
		} // namespace defaults

		class UPROAR_API cache : public base_task
		{
			struct UPROAR_API item : public std::array<decimal_t, defaults::cache_max_variables>
			{
				using Super = std::array<decimal_t, defaults::cache_max_variables>;
				using Super::Super;

				bool is_valid{false};
				decimal_t value;
			};

		public:
			void set_source(task_source src)
			{
				source_ = src;
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
				constexpr uint32_t arg_count = sizeof...(args);
				std::array<decimal_t, defaults::cache_max_dimension> t_args{
					std::forward<Args>(args)...};

				item &store = stack_[arg_count - 1];
				auto is_stale = !store.is_valid;
				for (uint32_t i = 0; i < arg_count; ++i)
				{
					is_stale |= (store[i] != t_args[i]);
					store[i] = t_args[i];
				}

				if (is_stale)
				{
					store.value = source_.eval(std::forward<Args>(args)...);
					store.is_valid = true;
				}

				return store.value;
			}

		private:
			task_source source_{};
			mutable std::array<item, defaults::cache_max_dimension> stack_{};
		};

		template <>
		struct config<cache>
		{
			void operator()(cache &task, const json::object &obj, configure_callback &callback) const
			{
				static const std::string source_key{"source"};

				auto end = std::end(obj);
				auto src_it = obj.find(source_key);
				if (src_it != end)
				{
					auto src = callback.eval(src_it->second);
					task.set_source(*src);
				}
			}
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_CACHE_HPP
