#ifndef UPROAR_TASKS_BASE_TASK_HPP
#define UPROAR_TASKS_BASE_TASK_HPP

#include "../core/attributes.hpp"
#include "../config/config.hpp"
#include "../core/json.hpp"
#include "fwd.hpp"

namespace tc
{
	namespace task
	{	
		class UPROAR_API configure_callback
		{
			public:
			 virtual scope_ptr<task_source> eval(const json::value& val) const = 0;
		};


		class UPROAR_API base_task
		{
		public:
			virtual ~base_task() = default;
			
			virtual void configure(const json::object& obj, configure_callback& callback) {}

			virtual decimal_t eval(decimal_t x) const { return 0; }
			virtual decimal_t eval(decimal_t x, decimal_t y) const { return 0; }
			virtual decimal_t eval(decimal_t x, decimal_t y, decimal_t z) const { return 0; }
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_BASE_TASK_HPP
