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
		class UPROAR_API task_factory : public factory_base<std::string, base_task, task_factory>, public singleton_base<task_factory>
		{
			using Super = factory_base<std::string, base_task, task_factory>;
			friend class Super;
			using config_t = void(*)(scope_ptr<base_task>&, const json::object&, configure_callback&);

			public:

			scope_ptr<base_task> spawn(const std::string& key, const json::object& config, configure_callback& cb) const
			{
				auto task = Super::spawn(key);

				if (!task.is_valid()) {
					return { nullptr };
				}

				auto s = configurations.find(key);
				if (s != std::end(configurations)) {
					s->second(task, config, cb);
				}

				return std::move(task);
			}

			private:

			template<typename Type>
			void record_impl(const std::string& key)
			{
				auto x = configurations.emplace(key, [](auto& x, const auto& y, auto& z) {
					return config<Type>{}(*static_cast<Type*>(x.get()), y, z);
				});
			}

			std::unordered_map<std::string, config_t> configurations;
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_TASK_FACTORY_HPP
