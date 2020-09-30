#ifndef UPROAR_TASKS_GENERATION_HPP
#define UPROAR_TASKS_GENERATION_HPP

#include "../config/config.hpp"
#include "fwd.hpp"
#include "base_task.hpp"

namespace tc
{
	namespace task
	{
		class UPROAR_API generator_task : public base_task
		{
		};

		template<typename Task>
		class UPROAR_API generation : public generator_task
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

#endif // UPROAR_TASKS_GENERATION_HPP
