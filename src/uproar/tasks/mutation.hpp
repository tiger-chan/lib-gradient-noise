#ifndef UPROAR_TASKS_MUTATIONS_HPP
#define UPROAR_TASKS_MUTATIONS_HPP

#include "../config/config.hpp"
#include "fwd.hpp"
#include "base_task.hpp"

namespace tc
{
	namespace task
	{
		class UPROAR_API mutation_task : public base_task
		{
		public:
		};

		template<typename Task>
		class UPROAR_API mutation : public mutation_task
		{
		public:
			decimal_t eval(decimal_t x) const final
			{
				return static_cast<const Task*>(this)->eval_impl(x);
			}

			decimal_t eval(decimal_t x, decimal_t y) const final
			{
				return static_cast<const Task*>(this)->eval_impl(x, y);
			}

			decimal_t eval(decimal_t x, decimal_t y, decimal_t z) const final
			{
				return static_cast<const Task*>(this)->eval_impl(x, y, z);
			}
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_MUTATIONS_HPP
