#include "schema_object.hpp"
#include "mapper.hpp"

namespace tc {
	namespace schema {
		template<typename T>
		parser_interface parser_interface::create(T &obj) {
			parser_interface interface{};
			interface.data = &obj;

			auto setter = [](context_stack &stack, void *data, std::string_view name, const auto &value) {
				T *obj = static_cast<T *>(data);
				detail::object<T> &root = detail::object<T>::instance;
				root.set_value<decltype(value)>(stack, *obj, name, value);
			};
			interface.vtable.set_bool = setter;
			interface.vtable.set_char = setter;
			interface.vtable.set_int16 = setter;
			interface.vtable.set_int32 = setter;
			interface.vtable.set_int64 = setter;
			interface.vtable.set_uint8 = setter;
			interface.vtable.set_uint16 = setter;
			interface.vtable.set_uint32 = setter;
			interface.vtable.set_uint64 = setter;
			interface.vtable.set_float = setter;
			interface.vtable.set_double = setter;
			interface.vtable.set_string = setter;

			interface.vtable.set_null = [](context_stack &stack, void *data, std::string_view name) {
				// Not sure if there is a specific action to be taken here.
			};

			interface.vtable.begin_object = [](context_stack &stack, std::string_view name) {
				if (name.empty()) {
					// Starting case where the outer object is unnamed.
					return;
				}
				detail::object<T> &root = detail::object<T>::instance;
				root.push_back(stack, name);
			};

			interface.vtable.end_object = [](context_stack &stack) {
				detail::object<T> &root = detail::object<T>::instance;
				if (stack.empty()) {
					return;
				}
				root.pop_back(stack);
			};

			interface.vtable.begin_array = [](context_stack &stack, std::string_view name) {
				detail::object<T> &root = detail::object<T>::instance;
				if (name.empty()) {
					// Starting case where the outer object is unnamed.
					return;
				}
				root.push_back(stack, name);
			};
			
			interface.vtable.begin_array_element = [](context_stack &stack, int32 idx) {
				detail::object<T> &root = detail::object<T>::instance;
				root.push_back(stack, idx);
			};

			interface.vtable.end_array = [](context_stack &stack) {
				detail::object<T> &root = detail::object<T>::instance;
				if (stack.empty()) {
					return;
				}
				root.pop_back(stack);
			};

			return interface;
		}

		template<typename V>
		void parser_interface::set_value(std::string_view name, const V &value) {

#define SET_VTABLE(NAME, TYPE) \
	if constexpr (std::is_same_v<V, TYPE>) { \
		vtable.NAME(stack, data, name, value); \
	}

			SET_VTABLE(set_bool, bool)
			else SET_VTABLE(set_char, char) else SET_VTABLE(set_int16, int16) else SET_VTABLE(set_int32, int32) else SET_VTABLE(set_int64, int64)
			else SET_VTABLE(set_uint8, uint8) else SET_VTABLE(set_uint16, uint16) else SET_VTABLE(set_uint32, uint32) else SET_VTABLE(set_uint64, uint64)
			else SET_VTABLE(set_float, float) else SET_VTABLE(set_double, double)
			else SET_VTABLE(set_string, std::string)

#undef SET_VTABLE

		}

		void parser_interface::set_null(std::string_view name) {
			vtable.set_null(stack, data, name);
		}

		void parser_interface::begin_object(std::string_view name) {
			vtable.begin_object(stack, name);
		}

		void parser_interface::end_object() {
			vtable.end_object(stack);
		}

		void parser_interface::begin_array(std::string_view name) {
			vtable.begin_array(stack, name);
		}

		void parser_interface::begin_array_element(int32 i) {
			vtable.begin_array_element(stack, i);
		}

		void parser_interface::end_array_element() {
			if (stack.empty()) {
				return;
			}
			stack.pop_back();
		}

		void parser_interface::end_array() {
			vtable.end_array(stack);
		}
	}    // namespace schema
}    // namespace tc

namespace tc {
	namespace schema {
		template<typename Object, typename Content>
		void to_object(Object &obj, Content &content) {
			parser_interface interface = parser_interface::create(obj);

			parse(interface, content);
		}
	}    // namespace schema
}    // namespace tc
