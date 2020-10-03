#ifndef UPROAR_TASKS_TURBULENCE_HPP
#define UPROAR_TASKS_TURBULENCE_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../core/utlities.hpp"
#include "fwd.hpp"
#include "mutation.hpp"
#include "task_source.hpp"
#include <array>

namespace tc
{
	namespace task
	{
		namespace defaults
		{
			static constexpr uint32_t turbulence_max_sources{UPROAR_MAX_VARIABLES};
		}

		class UPROAR_API turbulence : public mutation<turbulence>
		{
			friend class mutation<turbulence>;

		public:
			void set_source(task_source source)
			{
				source_ = std::move(source);
			}

			void set_multiplier(task_source multiplier)
			{
				multiplier_ = std::move(multiplier);
			}

			void set_translation(math::variable var, task_source source)
			{
				auto i = math::to_index(var);
				UPROAR_ASSERT(i < defaults::turbulence_max_sources);
				translations_[i] = std::move(source);
			}

		private:
			template <typename... Args>
			decimal_t eval_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				auto m = multiplier_.eval(std::forward<Args>(args)...);

				std::array<decimal_t, defaults::turbulence_max_sources> t{
					std::forward<Args>(args)...};

				for (auto j = 0; j < sizeof...(args); ++j)
				{
					t[j] += translations_[j].eval(std::forward<Args>(args)...) * m;
				}

				return eval_with<0, sizeof...(args)>(source_, t);
			}

			task_source source_{};
			task_source multiplier_{};
			std::array<task_source, defaults::turbulence_max_sources> translations_{};
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_TURBULENCE_HPP
