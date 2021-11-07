#ifndef UPROAR_TASKS_TASK_FACTORY_HPP
#define UPROAR_TASKS_TASK_FACTORY_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include "../core/factory_base.hpp"
#include "../memory.hpp"
#include "base_task.hpp"
#include "generation.hpp"
#include "parse.hpp"

#include <functional>
#include <memory>
#include <string>
#include <type_traits>

namespace tc {
	namespace task {
		template<typename>
		class factory_callable_base;

		template<typename Ret, typename... Args>
		class factory_callable_base<Ret(Args...)> {
		public:
			using fn_t = Ret(Args...);
			virtual ~factory_callable_base() = default;

			virtual Ret operator()(Args...) const = 0;
			virtual Ret operator()(Args...) = 0;
		};

		template<typename Fn, typename Ret, typename... Args>
		class functor_factory_callable : public factory_callable_base<Ret(Args...)> {
		public:
			functor_factory_callable(Fn fn)
				: fn_{ fn } {
			}

			Ret operator()(Args... args) const final {
				return fn_(std::forward<Args>(args)...);
			}

			Ret operator()(Args... args) final {
				return fn_(std::forward<Args>(args)...);
			}

		private:
			Fn fn_;
		};

		template<typename>
		class factory_callable;

		template<typename Ret, typename... Args>
		class factory_callable<Ret(Args...)> {
		public:
			factory_callable() = default;
			template<typename Fn>
			factory_callable(Fn fn)
				: fn_{ new functor_factory_callable<Fn, Ret, Args...>(fn) } {
			}

			Ret operator()(Args... args) const {
				return (*fn_)(std::forward<Args>(args)...);
			}

			Ret operator()(Args... args) {
				return (*fn_)(std::forward<Args>(args)...);
			}

		private:
			std::unique_ptr<factory_callable_base<Ret(Args...)>> fn_{ nullptr };
		};

		template<typename TaskObjectDef, typename TaskObjectDefValue>
		class UPROAR_API task_factory
			: public factory_base<std::string, base_task, task_factory<TaskObjectDef, TaskObjectDefValue>>
			, public singleton_base<task_factory<TaskObjectDef, TaskObjectDefValue>> {
			using Super = factory_base<std::string, base_task, task_factory>;
			friend class Super;

		public:
			using parse_t = factory_callable<void(scope_ptr<base_task> &, const TaskObjectDef &, parse_callback<TaskObjectDefValue> &)>;

			using config_t = parse_t();

			template<typename Type>
			auto record_task(const std::string &key) {
				Super::record<Type>(key);

				if constexpr (std::is_convertible_v<Type *, fractal_task *>) {
					struct fractal_config {
						void operator()(scope_ptr<base_task> &x, const TaskObjectDef &y, parse_callback<TaskObjectDefValue> &cb) const {
							// special case because we need to track the data
							const_cast<fractal_config &> (*this)(x, y, cb);
						}

						void operator()(scope_ptr<base_task> &x, const TaskObjectDef &y, parse_callback<TaskObjectDefValue> &cb) {
							auto v = static_cast<Type *>(x.get());
							auto *fractal = static_cast<fractal_task *>(v);
							auto parser = parse<TaskObjectDef, Type>{};

							parser(*v, y, cb);

							uint32_t &count = seed_counts[fractal->seed()];
							if (count++ == 0) {
								return;
							}

							fractal->set_seed(fractal->seed() + count);
						}

						std::unordered_map<uint32_t, uint32_t> seed_counts{};
					};

					return []() { return parse_t(fractal_config{}); };
				}
				else {
					return []() {
						return parse_t([](auto &x, const auto &y, auto &cb) {
							auto v = static_cast<Type *>(x.get());
							auto parser = parse<TaskObjectDef, Type>{};

							parser(*v, y, cb);
						});
					};
				}
			}

		private:
			template<typename Type>
			void record_impl(const std::string &key) {
			}
		};

		template<typename TaskObjectDef, typename TaskObjectDefValue>
		static auto &task_factory_v = task_factory<TaskObjectDef, TaskObjectDefValue>::instance();
	}    // namespace task
}    // namespace tc

#endif    // UPROAR_TASKS_TASK_FACTORY_HPP
