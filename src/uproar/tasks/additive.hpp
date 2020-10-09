#ifndef UPROAR_TASKS_ADDITIVE_HPP
#define UPROAR_TASKS_ADDITIVE_HPP

#include "../core/attributes.hpp"
#include "../config/config.hpp"
#include "../core/utlities.hpp"
#include "fwd.hpp"
#include "accumulator.hpp"

namespace tc
{
	namespace task
	{
		class UPROAR_API additive : public accumulator<additive>
		{
			using Super = accumulator<additive>;
			friend class Super;
			friend class mutation<additive>;

		public:
			using Super::Super;

		private:
			template <typename... Args>
			decimal_t eval_impl(Args &&... args) const UPROAR_NOEXCEPT
			{
				decimal_t result = 0.0;
				for (auto i = 0; i < sizeof...(args); ++i)
				{
					auto d = sources_[i].eval(std::forward<Args>(args)...);
					result += d;
				}

				return result;
			}
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_ADDITIVE_HPP
