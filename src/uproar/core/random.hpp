#ifndef UPROAR_CORE_RANDOM_HPP
#define UPROAR_CORE_RANDOM_HPP

#include "../config/config.hpp"
#include "../core/attributes.hpp"
#include <cstdint>
#include <array>
#include <vector>
#include <ctime>
#include "algorithm.hpp"

#ifndef UPROAR_CORE_RANDOM_LCE_A
#	define UPROAR_CORE_RANDOM_LCE_A 4096
#endif

#ifndef UPROAR_CORE_RANDOM_LCE_C
#	define UPROAR_CORE_RANDOM_LCE_C 150889
#endif

#ifndef UPROAR_CORE_RANDOM_LCE_M
#	define UPROAR_CORE_RANDOM_LCE_M 714025
#endif

namespace tc
{
	namespace internal
	{
		// https://en.wikipedia.org/wiki/Linear_congruential_generator
		static constexpr int32_t lce_a = UPROAR_CORE_RANDOM_LCE_A;
		static constexpr int32_t lce_c = UPROAR_CORE_RANDOM_LCE_C;
		static constexpr int32_t lce_m = UPROAR_CORE_RANDOM_LCE_M;

		constexpr static uint32_t uniform_distribution(uint32_t &previous)
		{
			previous = ((lce_a * previous + lce_c) % lce_m);
			return previous;
		}

		constexpr static double uniform_distribution_n(uint32_t &previous)
		{
			auto dst = uniform_distribution(previous);
			return static_cast<double>(dst) / lce_m;
		}

		template <typename T, size_t size, uint32_t seed>
		constexpr static auto uniform_distribution(T min, T max)
		{
			std::array<T, size> result{};
			auto prev = seed;
			for (auto &el : result)
			{
				el = static_cast<T>(uniform_distribution_n(prev) * (max - min) + min);
			}

			return result;
		}

		template <typename T>
		static auto uniform_distribution(uint32_t &seed, T min, T max)
		{
			std::vector<T> result{};
			result.resize(static_cast<size_t>(max - min) + 1);
			auto prev = uniform_distribution(seed);
			for (auto &el : result)
			{
				el = static_cast<T>(uniform_distribution_n(prev) * (max - min) + min);
			}

			return result;
		}
	} // namespace internal

	class UPROAR_API random
	{
	public:
		random() UPROAR_NOEXCEPT : previous{static_cast<uint32_t>(time(nullptr))}
		{
			original = previous;
		}
		random(uint32_t seed) UPROAR_NOEXCEPT : previous{seed}, original{seed}
		{
		}

		void reset() UPROAR_NOEXCEPT
		{
			previous = original;
		}

		void reset(uint32_t seed) UPROAR_NOEXCEPT
		{
			original = seed;
			previous = seed;
		}

		template <typename T>
		auto next(T max) const UPROAR_NOEXCEPT
		{
			auto val = internal::uniform_distribution_n(previous);
			if constexpr (std::is_integral_v<T>)
			{
				return static_cast<T>(tc::quick_floor((val < 0 ? -val : val) * max));
			}
			else
			{
				return static_cast<T>(val < 0 ? -val : val);
			}
		}

		template <typename T>
		auto next(T min, T max) const UPROAR_NOEXCEPT
		{
			return next(max - min) + min;
		}

		double next() const UPROAR_NOEXCEPT
		{
			return internal::uniform_distribution_n(previous);
		}

		template <typename T>
		auto uniform_distribution(T min, T max) const UPROAR_NOEXCEPT
		{
			return internal::uniform_distribution(previous, min, max);
		}

		template <typename T, size_t size, uint32_t seed>
		static constexpr auto uniform_distribution(T min, T max) UPROAR_NOEXCEPT
		{
			return internal::uniform_distribution<T, size, seed>(min, max);
		}

		mutable uint32_t previous{0};
		uint32_t original{0};
	};
} // namespace tc

#endif // UPROAR_CORE_RANDOM_HPP
