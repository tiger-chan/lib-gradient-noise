#ifndef UPROAR_TASKS_MULTIPLY_HPP
#define UPROAR_TASKS_MULTIPLY_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../core/utlities.hpp"
#include "accumulator.hpp"
#include "fwd.hpp"

namespace tc {
	namespace task {
		class UPROAR_API multiply : public accumulator<multiply> {
			using Super = accumulator<multiply>;
			friend class Super;
			friend class mutation<multiply>;

		public:
			using Super::Super;

		private:
			template<typename... Args>
			decimal_t eval_impl(Args &&...args) const UPROAR_NOEXCEPT {
				decimal_t result = 1.0;
				for (auto i = 0; i < sizeof...(args); ++i) {
					auto d = sources_[i].eval(std::forward<Args>(args)...);
					result *= d;
				}

				return result;
			}
		};
	}    // namespace task
}    // namespace tc

#endif    // UPROAR_TASKS_MULTIPLY_HPP
