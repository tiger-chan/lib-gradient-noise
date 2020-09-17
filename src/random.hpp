#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include "utils.hpp"

namespace tc
{
	namespace internal
	{
		static constexpr int32_t lce_a = 4096;
		static constexpr int32_t lce_c = 150889;
		static constexpr int32_t lce_m = 714025;

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

	class random
	{
	public:
		template <typename T>
		auto next(T max) const
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
		auto next(T min, T max) const
		{
			return next(max - min) + min;
		}

		double next() const
		{
			return internal::uniform_distribution_n(previous);
		}

		template <typename T>
		auto uniform_distribution(T min, T max) const
		{
			return internal::uniform_distribution(previous, min, max);
		}

		template <typename T, size_t size, uint32_t seed>
		static constexpr auto uniform_distribution(T min, T max)
		{
			return internal::uniform_distribution<T, size, seed>(min, max);
		}

		mutable uint32_t previous{0};
	};
} // namespace tc
