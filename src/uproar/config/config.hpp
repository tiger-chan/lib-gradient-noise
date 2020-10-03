#ifndef UPROAR_CONFIG_CONFIG_HPP
#define UPROAR_CONFIG_CONFIG_HPP


#ifndef UPROAR_NOEXCEPT
#   define UPROAR_NOEXCEPT noexcept
#endif

#ifndef UPROAR_ASSERT
#   include <cassert>
#   define UPROAR_ASSERT(condition) assert(condition)
#endif

#ifndef UPROAR_OCTAVE_TYPE
#		include <cstdint>
#		define UPROAR_OCTAVE_TYPE uint8_t
#endif

#ifndef UPROAR_DECIMAL_TYPE
#		define UPROAR_DECIMAL_TYPE double
#endif

#ifndef UPROAR_MAX_VARIABLES
#		define UPROAR_MAX_VARIABLES 3
#endif

namespace tc
{
	using octave_t = UPROAR_OCTAVE_TYPE;
	using decimal_t = UPROAR_DECIMAL_TYPE;
	namespace math {
		using variable_t = unsigned char;
		enum class variable : unsigned char {
			x, y, z
		};

		inline variable_t to_index(const variable& v) {
			return static_cast<variable_t>(v);
		}

		inline const char* to_c_str(const variable& v) {
			switch (v)
			{
			case variable::x: return "x";
			case variable::y: return "y";
			case variable::z: return "z";
			default: return "\0";
			}
		}
	}
}

#endif // UPROAR_CONFIG_CONFIG_HPP
