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

				return call<0, defaults::translate_max_sources>(source_, t);
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


			task_source source_;
			std::array<task_source, defaults::translate_max_sources> translations_{};
		};
	}
}

#endif // UPROAR_TASKS_TRANSLATE_HPP
