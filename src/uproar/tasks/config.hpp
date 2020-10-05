#ifndef UPROAR_TASKS_CONFIG_HPP
#define UPROAR_TASKS_CONFIG_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../memory.hpp"
#include "base_task.hpp"
#include <unordered_map>

namespace tc
{
	namespace task
	{
		template<typename ConfigValue>
		struct config_callback
		{
			task_source operator()(const ConfigValue& value) const;

			std::unordered_map<std::string, scope_ptr<base_task>>* tasks;
		};

		template<typename ConfigSource, typename Type>
		struct config
		{
			template<typename Callback>
			void operator()(Type& task, const ConfigSource& obj, Callback& cb) const;
		};
	}
}

#endif // UPROAR_TASKS_CONFIG_HPP
