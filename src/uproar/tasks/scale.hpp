#ifndef UPROAR_TASKS_SCALE_HPP
#define UPROAR_TASKS_SCALE_HPP

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
			static constexpr uint32_t scale_max_sources{UPROAR_MAX_VARIABLES};
		}

		class UPROAR_API scale : public mutation<scale>
		{
			friend class mutation<scale>;
		public:
			scale()
			{
				scalars_.fill(1.0);
			}

			void set_source(task_source source)
			{
				source_ = std::move(source);
			}

			void set_scale(math::variable var,  task_source source)
			{
				auto i = math::to_index(var);
				UPROAR_ASSERT(i < defaults::scale_max_sources);
				scalars_[i] = std::move(source);
			}

		private:
			template <typename... Args>
			decimal_t eval_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				std::array<decimal_t, defaults::scale_max_sources> t{
					std::forward<Args>(args)...
				};

				for (auto i = 0; i < sizeof...(args); ++i)
				{
					t[i] *= scalars_[i].eval(std::forward<Args>(args)...);
				}

				return eval_with<0, defaults::scale_max_sources>(source_, t);
			}

			task_source source_;
			std::array<task_source, defaults::scale_max_sources> scalars_{};
		};
	}
}

#endif // UPROAR_TASKS_SCALE_HPP
