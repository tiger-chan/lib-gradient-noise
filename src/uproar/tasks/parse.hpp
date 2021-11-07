#ifndef UPROAR_TASKS_CONFIG_HPP
#define UPROAR_TASKS_CONFIG_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../memory.hpp"
#include "base_task.hpp"

#include <string>
#include <unordered_map>

namespace tc {
	namespace task {
		struct task_details {
			std::string name{};
			std::string type{};
			bool rendered{ false };
		};

		template<typename ConfigValue>
		struct parse_callback {
			task_source operator()(const ConfigValue &value) const;

			std::unordered_map<std::string, scope_ptr<base_task>> *tasks;
		};

		template<typename ConfigSource, typename Type>
		struct parse {
			template<typename Callback>
			void operator()(Type &task, const ConfigSource &obj, Callback &cb) const {}

			void operator()(Type &task, const ConfigSource &obj) const {}
		};
	}    // namespace task
}    // namespace tc

#endif    // UPROAR_TASKS_CONFIG_HPP
