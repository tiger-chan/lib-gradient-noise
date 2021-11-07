#ifndef UPROAR_TASKS_BASE_TASK_HPP
#define UPROAR_TASKS_BASE_TASK_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "fwd.hpp"

namespace tc {
	namespace task {
		class UPROAR_API base_task {
		public:
			virtual ~base_task() = default;

			virtual decimal_t eval(decimal_t x) const {
				return 0;
			}
			virtual decimal_t eval(decimal_t x, decimal_t y) const {
				return 0;
			}
			virtual decimal_t eval(decimal_t x, decimal_t y, decimal_t z) const {
				return 0;
			}
		};
	}    // namespace task
}    // namespace tc

#endif    // UPROAR_TASKS_BASE_TASK_HPP
