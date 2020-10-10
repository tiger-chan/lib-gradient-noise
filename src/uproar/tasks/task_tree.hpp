#ifndef UPROAR_TASKS_TASK_TREE
#define UPROAR_TASKS_TASK_TREE

#include <unordered_map>
#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../core/scoped_ptr.hpp"
#include "parse.hpp"
#include "task_factory.hpp"
#include "base_task.hpp"

#include "../core/perlin-noise.hpp"
#include "constant.hpp"
#include "billowing.hpp"
#include "perlin.hpp"
#include "ridged_multifractal.hpp"
#include "gradient.hpp"

#include "map_range.hpp"
#include "scale_bias.hpp"
#include "scale_domain.hpp"
#include "translate_domain.hpp"
#include "bias.hpp"

#include "multiply.hpp"

#include "selector.hpp"
#include "cache.hpp"

namespace tc
{
	namespace task
	{
		template<typename TreeObject, typename TreeObjectValue>
		class UPROAR_API task_tree : public base_task
		{
			using map_t = std::unordered_map<std::string, scope_ptr<base_task>>;
			using factory_t =  task_factory<TreeObject, TreeObjectValue>;
			using parse_t = typename factory_t::parse_t;
			using config_t = factory_callable<parse_t()>;
			public:

			task_tree()
			{
				static std::unordered_map<std::string, config_t> configurations;
				static bool registered = false;
				if (!registered) {
					static const std::string constant_key{"constant"};
					static const std::string billowing_key{"billowing"};
					static const std::string perlin_key{"perlin"};
					static const std::string ridged_multi_key{"ridged_multi"};
					static const std::string gradient_key{"gradient"};
					static const std::string map_range_key{"map_range"};
					static const std::string scale_bias_key{"scale_bias"};
					static const std::string scale_domain_key{"scale"};
					static const std::string translate_domain_key{"translate"};
					static const std::string bias_key{"bias"};
					static const std::string selector_key{"selector"};
					static const std::string cache_key{"cache"};
					static const std::string additive_key{"additive"};
					static const std::string multiply_key{"multiply"};
					auto& instance = task_factory_v<TreeObject, TreeObjectValue>;

					configurations[constant_key] = instance.record_task<constant>(constant_key);
					configurations[billowing_key] =  instance.record_task<billowing<perlin_quintic>>(billowing_key);
					configurations[perlin_key] = instance.record_task<perlin<perlin_quintic>>(perlin_key);
					configurations[ridged_multi_key] = instance.record_task<ridged_multifractal<perlin_quintic>>(ridged_multi_key);
					configurations[gradient_key] = instance.record_task<gradient>(gradient_key);
					configurations[map_range_key] = instance.record_task<map_range>(map_range_key);
					configurations[scale_bias_key] = instance.record_task<scale_bias>(scale_bias_key);
					configurations[scale_domain_key] = instance.record_task<scale_domain>(scale_domain_key);
					configurations[translate_domain_key] = instance.record_task<translate_domain>(translate_domain_key);
					configurations[bias_key] = instance.record_task<bias_task>(bias_key);
					configurations[selector_key] = instance.record_task<selector_quintic>(selector_key);
					configurations[cache_key] = instance.record_task<cache>(cache_key);
					configurations[additive_key] = instance.record_task<additive>(additive_key);
					configurations[multiply_key] = instance.record_task<multiply>(multiply_key);

					registered = true;
				}

				for (const auto& pair: configurations) {
					parsers[pair.first] = pair.second();
				}

				callback.tasks = &tasks;
				rendered_task = std::end(tasks);
			}
			
			template<typename Iter>
			void read(Iter begin, Iter end)
			{
				for (auto it = begin; it != end; ++it)
				{
					spawn(*it);
				}

				for (auto it = begin; it != end; ++it)
				{
					configure(*it);
				}
			}

			void read(const TreeObject& object)
			{
				spawn(object);
				configure(object);
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
			void spawn(const TreeObject& object)
			{
				task_details details{};
				parse<TreeObject, task_details>{}(details, object);

				const auto& type =  details.type;

				auto& instance = task_factory_v<TreeObject, TreeObjectValue>;
				auto t = instance.spawn(type);

				if (!t.is_valid()) {
					// I'm not sure what type of logging I would want, at this point.
					return;
				}

				const auto& name = details.name;

				auto rendered_name = name;
				if (rendered_task != std::end(tasks)) {
					rendered_name = rendered_task->first;
				}

				auto task = tasks.emplace(name, std::move(t));

				if (details.rendered) {
					rendered_task = task.first;
				}
				else
				{
					rendered_task = tasks.find(rendered_name);
				}				
			}

			void configure(const TreeObject& object)
			{
				task_details details{};
				parse<TreeObject, task_details>{}(details, object);

				const auto& type =  details.type;
				const auto& name =  details.name;

				auto task = tasks.find(name);
				if (task == std::end(tasks)) {
					return;
				}

				auto& instance = task_factory_v<TreeObject, TreeObjectValue>;
				parsers[type](task->second, object, callback);
			}

			map_t tasks;
			map_t::iterator rendered_task;
			std::unordered_map<std::string, parse_t> parsers;
			parse_callback<TreeObjectValue> callback{};
		};
	};
}

#endif // UPROAR_TASKS_TASK_TREE
