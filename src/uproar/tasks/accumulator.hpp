#ifndef UPROAR_TASKS_ACCUMULATOR_HPP
#define UPROAR_TASKS_ACCUMULATOR_HPP

#include "../config/config.hpp"
#include "fwd.hpp"
#include "mutation.hpp"
#include "task_source.hpp"

#ifndef UPROAR_ACCUMULATOR_MAX_SOURCES_COUNT
#define UPROAR_ACCUMULATOR_MAX_SOURCES_COUNT 20
#endif

namespace tc
{
	namespace task
	{
		namespace defaults
		{
			static constexpr size_t accumulator_max_count{UPROAR_ACCUMULATOR_MAX_SOURCES_COUNT};
		}

		template <typename Task>
		class UPROAR_API accumulator : public mutation<Task>
		{
		public:
			accumulator()
			{
				sources_.resize(max_sources_, task_source{1.0});
			}

			accumulator(size_t max_source_count) : max_sources_{max_source_count}
			{
				sources_.resize(max_sources_, task_source{1.0});
			}

			size_t size() const { return max_sources_; }

			void set_source(size_t index, task_source source)
			{
				UPROAR_ASSERT(index < max_sources_);
				sources_[index] = source;
			}

		protected:
			std::vector<task_source> sources_{};
			const size_t max_sources_{defaults::accumulator_max_count};
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_ACCUMULATOR_HPP
