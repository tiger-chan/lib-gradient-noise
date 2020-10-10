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

		class UPROAR_API fractal_task
		{
			public:
			virtual void set_seed(uint32_t seed) UPROAR_NOEXCEPT 
			{
				seed_ = seed;
			}

			uint32_t seed() const {
				return seed_;
			}

			protected:
			uint32_t seed_{0};
		};

	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_GENERATION_HPP
