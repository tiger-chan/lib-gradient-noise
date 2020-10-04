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

		template<typename Task>
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

		void configure(const json::object& obj, configure_callback& callback) override
		{
			static const std::string source_key{"source_"};

			auto end = std::end(obj);
			for (size_t i = 0; i < max_sources_; ++i)
			{
				std::string key = source_key + std::to_string(i);
				auto src_it = obj.find(key);
				if (src_it != end) {
					auto src = callback.eval(src_it->second);
					sources_[i] = *src;
				}
			}
		}

		protected:
		std::vector<task_source> sources_{};
		const size_t max_sources_{defaults::accumulator_max_count};
		};
	} // namespace task
} // namespace tc

#endif // UPROAR_TASKS_ACCUMULATOR_HPP
