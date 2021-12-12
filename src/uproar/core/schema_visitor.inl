#include "schema_visitor.hpp"

#include <ios>

namespace tc {
	namespace schema {
		template<typename V>
		void visitor::set(const std::string_view &name, const V &value) {
#define SET_VTABLE(NAME, TYPE) \
	if constexpr (std::is_same_v<V, TYPE>) { \
		vtable.NAME(data, name, value); \
	}

			SET_VTABLE(set_bool, bool)
			else SET_VTABLE(set_char, char) else SET_VTABLE(set_int16, int16) else SET_VTABLE(set_int32, int32) else SET_VTABLE(set_int64, int64) else SET_VTABLE(set_uint8, uint8) else SET_VTABLE(set_uint16, uint16) else SET_VTABLE(set_uint32, uint32) else SET_VTABLE(set_uint64, uint64) else SET_VTABLE(set_float, float) else SET_VTABLE(set_double, double) else SET_VTABLE(set_string, std::string)

#undef SET_VTABLE

			else if constexpr (std::is_enum_v<V>) {
				std::string val{ enum_to_string<V>::to_string(value) };
				vtable.set_string(data, name, val);
			}
		}

		void visitor::set_null(const std::string_view &name) {
			vtable.set_null(data, name);
		}

		void visitor::begin_object(const std::string_view &name) {
			vtable.begin_object(data, name);
		}

		void visitor::end_object(const std::string_view &name) {
			vtable.end_object(data, name);
		}

		void visitor::begin_array(const std::string_view &name) {
			vtable.begin_array(data, name);
		}

		void visitor::end_array(const std::string_view &name) {
			vtable.end_array(data, name);
		}
	}    // namespace schema
}    // namespace tc
