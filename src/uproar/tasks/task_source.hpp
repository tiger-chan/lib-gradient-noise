#ifndef UPROAR_TASKS_TASK_SOURCE_HPP
#define UPROAR_TASKS_TASK_SOURCE_HPP

#include "../config/config.hpp"
#include "fwd.hpp"
#include "base_task.hpp"

namespace tc
{
	namespace task
	{
		struct task_source
		{
			enum class source_type
			{
				task,
				constant
			};

			union source_t
			{
				decimal_t value;
				base_task *task;
			};

			task_source(decimal_t constant = decimal_t{0}) UPROAR_NOEXCEPT : source{constant}
			{
			}

			task_source(base_task* task) UPROAR_NOEXCEPT
			{
				set(task);
			}

			inline task_source& set(decimal_t c) UPROAR_NOEXCEPT
			{
				source.value = c;
				type = source_type::constant;
				return *this;
			}

			inline task_source& set(base_task* t) UPROAR_NOEXCEPT
			{
				UPROAR_ASSERT(t != nullptr);
				source.task = t;
				type = source_type::task;
				return *this;
			}

			template <typename... Args>
			inline decimal_t eval(Args &&... args) const UPROAR_NOEXCEPT
			{
				switch (type)
				{
				default:
				case source_type::constant:
					return source.value;
				case source_type::task:
					return source.task->eval(std::forward<Args>(args)...);
				}
			}

		private:
			source_t source{0};
			source_type type{source_type::constant};
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_TASK_SOURCE_HPP
