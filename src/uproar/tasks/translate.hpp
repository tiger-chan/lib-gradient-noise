#ifndef UPROAR_TASKS_TRANSLATE_HPP
#define UPROAR_TASKS_TRANSLATE_HPP

#include "../config/config.hpp"
#include "fwd.hpp"
#include "mutation.hpp"
#include "task_source.hpp"
#include <array>

#ifndef UPROAR_TRANSLATE_MAX_SOURCES
#define UPROAR_TRANSLATE_MAX_SOURCES 3
#endif

namespace tc
{
	namespace task
	{
		namespace defaults
		{
			static constexpr uint32_t translate_max_sources{UPROAR_TRANSLATE_MAX_SOURCES};
		}

		class UPROAR_API translate : public mutation_task
		{
		public:
			using decimal_t = UPROAR_DECIMAL_TYPE;

			void set_source(task_source source)
			{
				source_ = std::move(source);
			}

			void set_translation(uint8_t source_index,  task_source source)
			{
				UPROAR_ASSERT(source_index < defaults::translate_max_sources);
				translations_[source_index] = std::move(source);
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
			template <typename... Args>
			decimal_t eval_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				auto i = 0;
				std::array<decimal_t, defaults::translate_max_sources> t{ };
				auto x = [this, &t, &i, pack = std::forward_as_tuple(args...)](const auto& y) {
					auto tr = &(translations_[i]);
					auto f = [tr](auto&& ... args) -> decimal_t {
						return tr->eval(args...);
					};
					t[i++] = std::apply(f, std::move(pack));
				};
				(x(args),...);
				
				// using expander = int8_t[];
				// (void)expander{ (t[i] = translations_[i].eval(std::forward<Args>(args)...), args, ++i, void(), 0)... };

				i = 0;
				return source_.eval((std::forward<Args>(args) + t[i++])...);
			}




			task_source source_;
			std::array<task_source, defaults::translate_max_sources> translations_{};
		};
	}
}

#endif // UPROAR_TASKS_TRANSLATE_HPP
