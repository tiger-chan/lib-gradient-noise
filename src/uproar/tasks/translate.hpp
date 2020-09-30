#ifndef UPROAR_TASKS_TRANSLATE_HPP
#define UPROAR_TASKS_TRANSLATE_HPP

#include "../core/attributes.hpp"
#include "../config/config.hpp"
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
			static constexpr uint32_t translate_max_sources{UPROAR_MAX_VARIABLES};
		}

		class UPROAR_API translate : public mutation<translate>
		{
			friend class mutation<translate>;
		public:
			void set_source(task_source source)
			{
				source_ = std::move(source);
			}

			void set_translation(math::variable var,  task_source source)
			{
				auto i = math::to_index(var);
				UPROAR_ASSERT(i < defaults::translate_max_sources);
				translations_[i] = std::move(source);
			}

		private:
			template <typename... Args>
			decimal_t eval_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				std::array<decimal_t, defaults::translate_max_sources> t{
					std::forward<Args>(args)...
				};

				for (auto i = 0; i < sizeof...(args); ++i)
				{
					t[i] += translations_[i].eval(std::forward<Args>(args)...);
				}

				return eval_with<0, defaults::translate_max_sources>(source_, t);
			}

			task_source source_;
			std::array<task_source, defaults::translate_max_sources> translations_{};
		};
	}
}

#endif // UPROAR_TASKS_TRANSLATE_HPP
