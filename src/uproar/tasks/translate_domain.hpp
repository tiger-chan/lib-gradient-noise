#ifndef UPROAR_TASKS_TRANSLATE_DOMAIN_HPP
#define UPROAR_TASKS_TRANSLATE_DOMAIN_HPP

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

		class UPROAR_API translate_domain : public mutation<translate_domain>
		{
			friend class mutation<translate_domain>;

		public:
			void set_source(task_source source)
			{
				source_ = std::move(source);
			}

			void set_translation(math::variable var, task_source source)
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
					std::forward<Args>(args)...};

				for (auto i = 0; i < sizeof...(args); ++i)
				{
					auto d = translations_[i].eval(std::forward<Args>(args)...);
					t[i] += d;
				}

				return eval_with<0, sizeof...(args)>(source_, t);
			}

			task_source source_;
			std::array<task_source, defaults::translate_max_sources> translations_{};
		};

		template <>
		struct config<translate_domain>
		{
			void operator()(translate_domain &task, const json::object &obj, configure_callback &callback) const
			{
				static const std::string source_key{"source"};

				auto end = std::end(obj);
				auto src_it = obj.find(source_key);
				if (src_it != end)
				{
					auto src = callback.eval(src_it->second);
					task.set_source(*src);
				}

				for (auto i = 0; i < defaults::translate_max_sources; ++i)
				{
					auto v = static_cast<math::variable>(i);
					auto variable_it = obj.find(math::to_c_str(v));
					if (variable_it != end)
					{
						auto src = callback.eval(variable_it->second);
						task.set_translation(v, *src);
					}
				}
			}
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_TRANSLATE_DOMAIN_HPP
