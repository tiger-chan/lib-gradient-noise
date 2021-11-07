#ifndef UPROAR_TASKS_GRADIENT_HPP
#define UPROAR_TASKS_GRADIENT_HPP

#include "../config/config.hpp"
#include "fwd.hpp"
#include "generation.hpp"

#include <array>

#ifndef UPROAR_GRADIENT_MAX_DIMENSIONS
#	define UPROAR_GRADIENT_MAX_DIMENSIONS 3
#endif

namespace tc {
	namespace task {
		namespace defaults {
			static constexpr uint8_t gradient_max_dimensions{ UPROAR_GRADIENT_MAX_DIMENSIONS };
		}    // namespace defaults

		class UPROAR_API gradient : public generation<gradient> {
			friend class generation<gradient>;

		public:
			using decimal_t = UPROAR_DECIMAL_TYPE;

			struct component {
				decimal_t _0;
				decimal_t _1;
			};

			gradient() = default;

			void set(const std::array<decimal_t, defaults::gradient_max_dimensions> &v1, const std::array<decimal_t, defaults::gradient_max_dimensions> &v2) {
				ac_size = decimal_t{ 0 };
				for (uint8_t i = 0; i < defaults::gradient_max_dimensions; ++i) {
					component p{
						v1[i],
						v2[i]
					};

					ac_[i] = p._1 - p._0;
					ac_size += (ac_[i] * ac_[i]);
					points_[i] = std::move(p);
				}

				UPROAR_ASSERT(ac_size > decimal_t{ 0 });
			}

		private:
			template<typename... Args>
			decimal_t eval_impl(Args &&...args) const UPROAR_NOEXCEPT {
				std::array<decimal_t, defaults::gradient_max_dimensions> ab{};

				auto i = 0;
				((ab[i++] = args - points_[i]._0), ...);

				decimal_t ac_X_ab{ 0 };
				for (auto j = 0; j < defaults::gradient_max_dimensions; ++j) {
					ac_X_ab += ac_[j] * ab[j];
				}
				static constexpr decimal_t zero{ 0 };
				static constexpr decimal_t one{ 1 };
				decimal_t result = ac_X_ab / ac_size;
				result = clamp(result, zero, one);
				result = lerp(-one, one, result);

				return result;
			}

			std::array<component, defaults::gradient_max_dimensions> points_{};
			std::array<decimal_t, defaults::gradient_max_dimensions> ac_{};
			decimal_t ac_size{ 1 };
		};
	}    // namespace task
}    // namespace tc

#endif    // UPROAR_TASKS_GRADIENT_HPP
