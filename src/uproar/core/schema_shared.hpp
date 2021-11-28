#ifndef UPROAR_CORE_SCHEMA_SHARED_HPP
#define UPROAR_CORE_SCHEMA_SHARED_HPP

#include <type_traits>
#include <vector>
#include <string_view>

#ifndef TC_STDINT
#	define TC_STDINT
using int8 = signed char;
using int16 = signed short;
using int32 = signed int;
using int64 = signed long long;

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;
#endif

namespace tc {
	namespace schema {
		using id_type = uint32;
		static const constexpr id_type null = UINT32_MAX;

		enum member_type : int32 {
			MT_bool,
			MT_char,
			MT_int16,
			MT_int32,
			MT_int64,
			MT_uint8,
			MT_uint16,
			MT_uint32,
			MT_uint64,
			MT_float,
			MT_double,
			MT_string,
			MT_enum,
			MT_object = 100,
			MT_array,

			MT_unknown = INT32_MAX,
		};
	}    // namespace schema
}    // namespace tc

#endif    // UPROAR_CORE_SCHEMA_SHARED_HPP
