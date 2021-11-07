#ifndef UPROAR_TASKS_CONSTANT_HPP
#define UPROAR_TASKS_CONSTANT_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../core/utlities.hpp"
#include "fwd.hpp"
#include "generation.hpp"

#include <array>

namespace tc {
	namespace task {
		class UPROAR_API constant : public generation<constant> {
			friend class generation<constant>;

		public:
			constant() {}

			void set_value(decimal_t value) {
				constant_value = value;
			}

		private:
			template<typename... Args>
			decimal_t eval_impl(Args &&...args) const UPROAR_NOEXCEPT {
				return constant_value;
			}

			decimal_t constant_value{ 0 };
		};
	}    // namespace task
}    // namespace tc

#endif    // UPROAR_TASKS_CONSTANT_HPP
