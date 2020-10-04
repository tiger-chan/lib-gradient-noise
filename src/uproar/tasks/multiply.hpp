#ifndef UPROAR_TASKS_MULTIPLY_HPP
#define UPROAR_TASKS_MULTIPLY_HPP

#include "../core/attributes.hpp"
#include "../config/config.hpp"
#include "../core/utlities.hpp"
#include "fwd.hpp"
#include "accumulator.hpp"

namespace tc
{
	namespace task
	{
		class UPROAR_API multiply : public accumulator<multiply>
		{
			using Super = accumulator<multiply>;
			friend class Super;
			friend class mutation<multiply>;
		public:
			using Super::Super;

		private:
			template <typename... Args>
			decimal_t eval_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				decimal_t result = 1.0;
				for (auto i = 0; i < sizeof...(args); ++i)
				{
					auto d = sources_[i].eval(std::forward<Args>(args)...);
					result *= d;
				}

				return result;
			}
		};
	}
}

#endif // UPROAR_TASKS_MULTIPLY_HPP
