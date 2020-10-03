#ifndef UPROAR_TASKS_TASK_TREE
#define UPROAR_TASKS_TASK_TREE

#include <unordered_map>
#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../core/scoped_ptr.hpp"
#include "../core/json.hpp"
#include "task_factory.hpp"
#include "base_task.hpp"

#include "../core/perlin-noise.hpp"
#include "billowing.hpp"
#include "perlin.hpp"
#include "ridged_multifractal.hpp"
#include "gradient.hpp"

#include "map_range.hpp"
#include "scale_bias.hpp"
#include "scale.hpp"
#include "translate.hpp"

#include "selector.hpp"
#include "cache.hpp"

namespace tc
{
	namespace task
	{
		class UPROAR_API task_tree : public base_task
		{
			public:
			task_tree()
			{
				static bool registered = false;
				if (!registered) {
					auto& instance = task_factory::instance();
					instance.record<billowing<perlin_quintic>>(std::string("billowing"));
					instance.record<perlin<perlin_quintic>>(std::string("perlin"));
					instance.record<ridged_multifractal<perlin_quintic>>(std::string("ridged_multi"));
					instance.record<gradient>(std::string("gradient"));
					instance.record<map_range>("map_range");
					instance.record<scale_bias>("scale_bias");
					instance.record<scale>("scale");
					instance.record<translate>("translate");
					instance.record<selector_quintic>("selector");
					instance.record<cache>("cache");

					registered = true;
				}

				rendered_task = std::end(tasks);
			}
			
			template<typename Iter>
			void read(Iter begin, Iter end)
			{
				for (; begin != end; ++begin)
				{
					read(*begin);
				}
			}

			void read(const json::object& object)
			{
				static const std::string name_key{"name"};
				static const std::string type_key{"type"};
				static const std::string rendered_key{"rendered"};

				auto type_val = object.at(type_key);

				const auto& type = type_val.as<std::string>();
				auto t = task_factory::instance().spawn(type);

				if (!t.is_valid()) {
					// I'm not sure what type of logging I would want, at this point.
					return;
				}
				
				auto& map = tasks;

				struct callback_t : public configure_callback
				{
					scope_ptr<task_source> eval(const json::value& val) const final
					{
						if (val.holds_alternative<decimal_t>()) {
							return make_scoped<task_source>(val.as<decimal_t>());
						}

						auto n = val.as<std::string>();
						auto task = map->find(n);

						if (task != std::end(*map)) {
							return make_scoped<task_source>(task->second.get());
						}
						
						return make_scoped<task_source>(0.0);
					}

					std::unordered_map<std::string, scope_ptr<base_task>>* map{nullptr};
				};

				callback_t callback{};
				callback.map = &tasks;
				t->configure(object, callback);

				const auto& name = object.at(name_key).as<std::string>();

				auto rendered_name = name;
				if (rendered_task != std::end(tasks)) {
					rendered_name = rendered_task->first;
				}

				auto task = tasks.emplace(name, std::move(t));

				auto rendered_it = object.find(rendered_key);
				if (rendered_it != std::end(object)) {
					rendered_task = task.first;
				}
				else
				{
					rendered_task = tasks.find(rendered_name);
				}
			}

			decimal_t eval(decimal_t x) const final
			{
				return rendered_task->second->eval(x);
			}

			decimal_t eval(decimal_t x, decimal_t y) const final
			{
				return rendered_task->second->eval(x, y);
			}

			decimal_t eval(decimal_t x, decimal_t y, decimal_t z) const final
			{
				return rendered_task->second->eval(x, y, z);
			}

			private:
			std::unordered_map<std::string, scope_ptr<base_task>> tasks;
			std::unordered_map<std::string, scope_ptr<base_task>>::iterator rendered_task;
		};
	};
}

#endif // UPROAR_TASKS_TASK_TREE
