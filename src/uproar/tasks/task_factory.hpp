#ifndef UPROAR_TASKS_TASK_FACTORY_HPP
#define UPROAR_TASKS_TASK_FACTORY_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../memory.hpp"
#include "../core/factory_base.hpp"
#include "base_task.hpp"
#include "config.hpp"
#include <string>
#include <type_traits>
#include <functional>

namespace tc
{
	namespace task
	{
		template<typename TaskObjectDef, typename TaskObjectDefValue>
		class UPROAR_API task_factory :
			public factory_base<std::string, base_task, task_factory<TaskObjectDef, TaskObjectDefValue>>,
			public singleton_base<task_factory<TaskObjectDef, TaskObjectDefValue>>
		{
			using Super = factory_base<std::string, base_task, task_factory>;
			friend class Super;
			using config_t = void(*)(scope_ptr<base_task>&, const TaskObjectDef&, config_callback<TaskObjectDefValue>&);

			public:

			template<typename Callback>
			void finish_spawn(const std::string& key, scope_ptr<base_task>& task, const TaskObjectDef& config, Callback& cb) const
			{
				if (!task.is_valid()) {
					return;
				}

				auto s = configurations.find(key);
				if (s != std::end(configurations)) {
					s->second(task, config, cb);
				}
			}

			private:

			template<typename Type>
			void record_impl(const std::string& key)
			{
				auto x = configurations.emplace(key, [](auto& x, const auto& y, auto& cb) {
					auto v = static_cast<Type*>(x.get());
					return config<TaskObjectDef, Type>{}(*v, y, cb);
				});
			}

			std::unordered_map<std::string, config_t> configurations;
		};

		template<typename TaskObjectDef, typename TaskObjectDefValue>
		static auto& task_factory_v = task_factory<TaskObjectDef, TaskObjectDefValue>::instance();
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_TASK_FACTORY_HPP
