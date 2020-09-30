#ifndef UPROAR_TASKS_TURBULENCE_HPP
#define UPROAR_TASKS_TURBULENCE_HPP

#include "../config/config.hpp"
#include "fwd.hpp"
#include "mutation.hpp"
#include "task_source.hpp"
#include <array>

#ifndef UPROAR_TURBULENCE_MAX_SOURCES
#define UPROAR_TURBULENCE_MAX_SOURCES 3
#endif

namespace tc
{
	namespace task
	{
		namespace defaults
		{
			static constexpr uint32_t turbulence_max_sources{UPROAR_TURBULENCE_MAX_SOURCES};
		}

		class UPROAR_API turbulence : public mutation_task
		{
		public:
			using decimal_t = UPROAR_DECIMAL_TYPE;

			void set_source(task_source source)
			{
				source_ = std::move(source);
			}

			void set_multiplier(task_source multiplier)
			{
				multiplier_ = std::move(multiplier);
			}

			void set_translation(uint8_t source_index,  task_source source)
			{
				UPROAR_ASSERT(source_index < defaults::turbulence_max_sources);
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
				auto m = multiplier_.eval(std::forward<Args>(args)...);
				
				std::array<decimal_t, defaults::turbulence_max_sources> t{
					std::forward<Args>(args)...
				};

				for (auto j = 0; j < sizeof...(args); ++j) {
					t[j] += translations_[j].eval(std::forward<Args>(args)...) * m;
				}

				return call<0, defaults::turbulence_max_sources>(source_, t);
			}

			template<uint8_t I, uint8_t Size, typename ...Args>
			inline auto call(const task_source& func, std::array<decimal_t, Size>& ar, Args&&... args) const UPROAR_NOEXCEPT
			{
				if constexpr (I < Size) {
					return call<I + 1>(func, ar, std::forward<Args>(args)..., ar[I]);
				}
				else
				{
					return func.eval(std::forward<Args>(args)...);
				}
			}
			
			task_source source_{};
			task_source multiplier_{};
			std::array<task_source, defaults::turbulence_max_sources> translations_{};
		};
	}
}

#endif // UPROAR_TASKS_TURBULENCE_HPP
