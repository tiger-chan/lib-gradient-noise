#pragma once

#include <array>
#include "utils.hpp"
#include "random.hpp"

namespace tc
{
	namespace internal
	{
		// https://mrl.nyu.edu/~perlin/noise/
		static constexpr std::array<int32_t, 512> perlin_permutations()
		{
			constexpr auto permutation_size = 256;
			constexpr auto p_size = 512;
			std::array<int32_t, permutation_size> permutation = {
				151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225,
				140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148,
				247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57,
				177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74,
				165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111, 229, 122, 60,
				211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65,
				25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200,
				196, 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52,
				217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207,
				206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183, 170, 213, 119,
				248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9, 129,
				22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218,
				246, 97, 228, 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81,
				51, 145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157, 184,
				84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222,
				114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180};

			std::array<int, p_size> p{};

			for (int i = 0; i < permutation_size; i++)
			{
				p[permutation_size + i] = p[i] = permutation[i];
			}

			return p;
		}

		static std::array<int32_t, 512> perlin_permutations(uint32_t seed)
		{
			random r{seed};
			std::array<int32_t, 512> result;
			auto tmp = r.uniform_distribution<int32_t>(0, 255);

			for (auto i = 0; i < 256; ++i)
			{
				result[i] = result[i + 256] = tmp[i];
			}

			return result;
		}

		static constexpr std::array<double, 16> perlin_gradiant_1d()
		{
			constexpr int32_t count = 16;
			constexpr double half_count = count / 2.0;
			std::array<double, count> grad{};
			for (auto i = 0; i < count; ++i)
			{
				if (i < half_count)
				{
					grad[i] = (i - half_count) / half_count;
				}
				else
				{
					grad[i] = (i - half_count + 1.0) / half_count;
				}
			}

			return grad;
		}
	} // namespace internal

	class perlin_noise
	{
	public:
		perlin_noise() : perm{internal::perlin_permutations()}
		{
		}

		perlin_noise(uint32_t seed) : perm{internal::perlin_permutations(seed)}
		{
		}

		void reset()
		{
			perm = internal::perlin_permutations();
		}

		void reset(uint32_t seed)
		{
			perm = internal::perlin_permutations(seed);
		}

		double eval(double x) const
		{
			auto x0 = tc::quick_floor(x);
			auto x1 = x0 + 1.0;

			auto dx = x - x0;
			auto u = fade(dx);

			auto A = perm[static_cast<int32_t>(x0) & 255];
			auto B = perm[static_cast<int32_t>(x1) & 255];

			auto gx0 = grad(A, x0);
			auto gx1 = grad(B, x1);

			auto p0 = gx0 * (x - x0);
			auto p1 = gx1 * (x - x1);
			return lerp(p0, p1, u);
		}

		double eval(double x, double y) const
		{
			double x0 = tc::quick_floor(x);
			double y0 = tc::quick_floor(y);
			int32_t xi = static_cast<int32_t>(x0) & index_mask;
			int32_t yi = static_cast<int32_t>(y0) & index_mask;
			x0 = x - x0;
			y0 = y - y0;
			auto x1 = x0 - 1.0;
			auto y1 = y0 - 1.0;

			auto AA = perm[xi] + yi;
			auto AB = AA + 1;
			auto BA = perm[xi + 1] + yi;
			auto BB = BA + 1;

			auto u = fade(x0);
			auto v = fade(y0);

			auto l1 = lerp(
				grad(perm[AA], x0, y0),
				grad(perm[BA], x1, y0),
				u);
			auto l2 = lerp(
				grad(perm[AB], x0, y1),
				grad(perm[BB], x1, y1),
				u);

			return lerp(l1, l2, v);
		}

		double eval(double x, double y, double z) const
		{
			// https://mrl.nyu.edu/~perlin/noise/
			// Find unit cube that contains point.
			double x0 = tc::quick_floor(x);
			double y0 = tc::quick_floor(y);
			double z0 = tc::quick_floor(z);
			int32_t xi = static_cast<int32_t>(x0) & index_mask;
			int32_t yi = static_cast<int32_t>(y0) & index_mask;
			int32_t zi = static_cast<int32_t>(z0) & index_mask;

			//  Find relative x,y,z of point in cube.
			x0 = x - x0;
			y0 = y - y0;
			z0 = z - z0;
			auto x1 = x0 - 1.0;
			auto y1 = y0 - 1.0;
			auto z1 = z0 - 1.0;

			// Hash coordinates of the 8 cube corners
			auto A = perm[xi] + yi;
			auto AA = perm[A] + zi;
			auto AB = perm[A + 1] + zi;
			auto B = perm[xi + 1] + yi;
			auto BA = perm[B] + zi;
			auto BB = perm[B + 1] + zi;

			// Compute fade curves for each of x,y,z.
			auto u = fade(x0);
			auto v = fade(y0);
			auto w = fade(z0);

			// And add blended results from 8 corners of cube
			auto lu1 = lerp(
				grad(perm[AA], x0, y0, z0),
				grad(perm[BA], x1, y0, z0),
				u);

			auto lu2 = lerp(
				grad(perm[AB], x0, y1, z0),
				grad(perm[BB], x1, y1, z0),
				u);

			auto lu3 = lerp(
				grad(perm[AA + 1], x0, y0, z1),
				grad(perm[BA + 1], x1, y0, z1),
				u);

			auto lu4 = lerp(
				grad(perm[AB + 1], x0, y1, z1),
				grad(perm[BB + 1], x1, y1, z1),
				u);

			auto lv1 = lerp( lu1, lu2, v);
			auto lv2 = lerp( lu3, lu4, v);

			return lerp(lv1, lv2, w);
		}

	private:
		static double fade(double t)
		{
			// 6t^5 - 15t^4 + 10t^3
			return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
		}

		static double grad(int32_t hash, double x)
		{
			static constexpr auto grad = internal::perlin_gradiant_1d();
			int h = hash & 15; // Convert lo 4 bits of hash code
			return grad[h] * x;
		}

		static double grad(int32_t hash, double x, double y)
		{
			int32_t h = hash & 7; // Convert lo 3 bits of hash code
			switch (h)
			{
			case 0:
				return x;
			case 1:
				return x + y;
			case 2:
				return y;
			case 3:
				return -x + y;
			case 4:
				return -x;
			case 5:
				return -x - y;
			case 6:
				return -y;
			case 7:
				return x - y;
			}

			return 0;
		}

		double grad(int hash, double x, double y, double z) const
		{
			switch(hash & 15)
			{
				// 12 cube midpoints
				case 0: return x + z;
				case 1: return x + y;
				case 2: return y + z;
				case 3: return -x + y;
				case 4: return -x + z;
				case 5: return -x - y;
				case 6: return -y + z;
				case 7: return x - y;
				case 8: return x - z;
				case 9: return y - z;
				case 10: return -x - z;
				case 11: return -y - z;
				// 4 vertices of regular tetrahedron
				case 12: return x + y;
				case 13: return -x + y;
				case 14: return -y + z;
				case 15: return -y - z;
				// This can't happen
				default: return 0;
			}

			// https://mrl.nyu.edu/~perlin/noise/
			// this seems like it would be slower to compute.
			// Convert lo 4 bits of hash code into 12 gradient directions.
			// int h = hash & 15;
			// double u = h < 8 ? x : y,
			// 	   v = h < 4 ? y : h == 12 || h == 14 ? x : z;
			// return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
		}

		static constexpr int32_t index_mask = 255;
		std::array<int32_t, 512> perm;
	}; // namespace tc
} // namespace tc
