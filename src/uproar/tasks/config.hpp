#ifndef UPROAR_TASKS_CONFIG_HPP
#define UPROAR_TASKS_CONFIG_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "base_task.hpp"

namespace tc
{
	namespace task
	{
		template<typename Type>
		struct config
		{
			void operator()(Type& task, const json::object& obj, configure_callback& callback) const;
		};
	}
}

#endif // UPROAR_TASKS_CONFIG_HPP
