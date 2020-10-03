#ifndef UPROAR_TASKS_TASK_FACTORY_HPP
#define UPROAR_TASKS_TASK_FACTORY_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../memory.hpp"
#include "../core/factory_base.hpp"
#include "base_task.hpp"
#include <string>

namespace tc
{
	namespace task
	{
		class UPROAR_API task_factory : public singleton_base<task_factory>, public factory_base<std::string, base_task>
		{
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_TASK_FACTORY_HPP
