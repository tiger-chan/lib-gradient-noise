#ifndef UPROAR_TASKS_FWD_HPP
#define UPROAR_TASKS_FWD_HPP

namespace tc {
	namespace task {
		class base_task;
		class generator_task;
		class mutation_task;

		struct task_source;

		template<typename>
		class perlin;
		template<typename>
		class billowing;
		template<typename>
		class ridged_multifractal;
	}    // namespace task
}    // namespace tc

#endif    // UPROAR_TASKS_FWD_HPP
