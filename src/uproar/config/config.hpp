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

#endif