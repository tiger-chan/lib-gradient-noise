#include "mapper.hpp"
#include "schema_object.hpp"
#include "schema_visitor.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			template<typename T>
			void fill_value_interface(parser_vtable &parser) {
				auto setter = [](context_stack &stack, void *data, std::string_view name, const auto &value) {
					T *obj = static_cast<T *>(data);
					using ValueType = std::decay_t<decltype(value)>;
					if constexpr (std::is_convertible_v<ValueType, T>) {
						*obj = static_cast<T>(value);
					}
				};

				parser.set_bool = setter;
				parser.set_char = setter;
				parser.set_int16 = setter;
				parser.set_int32 = setter;
				parser.set_int64 = setter;
				parser.set_uint8 = setter;
				parser.set_uint16 = setter;
				parser.set_uint32 = setter;
				parser.set_uint64 = setter;
				parser.set_float = setter;
				parser.set_double = setter;
				parser.set_string = setter;

				parser.set_null = [](context_stack &stack, void *data, std::string_view name) {
					// Not sure if there is a specific action to be taken here.
				};

				auto noop_objects = [](context_stack &, std::string_view) {};
				parser.begin_object = noop_objects;
				parser.end_object = noop_objects;
				parser.begin_array = noop_objects;

				parser.begin_array_element = [](context_stack &, int32) {};

				parser.end_array = noop_objects;
			}

			template<typename T>
			void fill_object_interface(parser_vtable &parser) {
				auto setter = [](context_stack &stack, void *data, std::string_view name, const auto &value) {
					detail::object<T> &root = detail::object<T>::instance;
					if (!name.empty()) {
						root.push_back(stack, name);
					}
					T *obj = static_cast<T *>(data);
					root.set_value<decltype(value)>(stack, *obj, value);
					if (!name.empty() && !stack.empty()) {
						root.pop_back(stack);
					}
				};

				parser.set_bool = setter;
				parser.set_char = setter;
				parser.set_int16 = setter;
				parser.set_int32 = setter;
				parser.set_int64 = setter;
				parser.set_uint8 = setter;
				parser.set_uint16 = setter;
				parser.set_uint32 = setter;
				parser.set_uint64 = setter;
				parser.set_float = setter;
				parser.set_double = setter;
				parser.set_string = setter;

				parser.set_null = [](context_stack &stack, void *data, std::string_view name) {
					// Not sure if there is a specific action to be taken here.
				};

				parser.begin_object = [](context_stack &stack, std::string_view name) {
					if (name.empty()) {
						// Starting case where the outer object is unnamed.
						return;
					}
					detail::object<T> &root = detail::object<T>::instance;
					root.push_back(stack, name);
				};

				parser.end_object = [](context_stack &stack, std::string_view name) {
					detail::object<T> &root = detail::object<T>::instance;
					if (stack.empty() || name.empty()) {
						return;
					}
					root.pop_back(stack);
				};

				parser.begin_array = [](context_stack &stack, std::string_view name) {
					detail::object<T> &root = detail::object<T>::instance;
					root.push_back(stack, name);
				};

				parser.begin_array_element = [](context_stack &stack, int32 idx) {
					detail::object<T> &root = detail::object<T>::instance;
					root.push_back(stack, idx);
				};

				parser.end_array = [](context_stack &stack, std::string_view name) {
					detail::object<T> &root = detail::object<T>::instance;
					if (stack.empty() || name.empty()) {
						return;
					}
					root.pop_back(stack);
				};
			}
		}    // namespace detail

		template<typename T>
		parser_interface parser_interface::create(T &obj) {
			parser_interface interface{};
			interface.data = &obj;

			if constexpr (member_type_trait_v<T> < MT_object) {
				detail::fill_value_interface<T>(interface.vtable);
			}
			else {
				detail::fill_object_interface<T>(interface.vtable);
			}

			return interface;
		}

		template<typename V>
		void parser_interface::set_value(std::string_view name, const V &value) {
#define SET_VTABLE(NAME, TYPE) \
	if constexpr (std::is_same_v<V, TYPE>) { \
		vtable.NAME(stack, data, name, value); \
	}

			SET_VTABLE(set_bool, bool)
			else SET_VTABLE(set_char, char) else SET_VTABLE(set_int16, int16) else SET_VTABLE(set_int32, int32) else SET_VTABLE(set_int64, int64) else SET_VTABLE(set_uint8, uint8) else SET_VTABLE(set_uint16, uint16) else SET_VTABLE(set_uint32, uint32) else SET_VTABLE(set_uint64, uint64) else SET_VTABLE(set_float, float) else SET_VTABLE(set_double, double) else SET_VTABLE(set_string, std::string)

#undef SET_VTABLE
		}

		void parser_interface::set_null(std::string_view name) {
			vtable.set_null(stack, data, name);
		}

		void parser_interface::begin_object(std::string_view name) {
			vtable.begin_object(stack, name);
		}

		void parser_interface::end_object(std::string_view name) {
			vtable.end_object(stack, name);
		}

		void parser_interface::begin_array(std::string_view name) {
			vtable.begin_array(stack, name);
		}

		void parser_interface::begin_array_element(int32 i) {
			vtable.begin_array_element(stack, i);
		}

		void parser_interface::end_array_element() {
			stack.pop_back();
		}

		void parser_interface::end_array(std::string_view name) {
			vtable.end_array(stack, name);
		}
	}    // namespace schema
}    // namespace tc

namespace tc {
	namespace schema {
		namespace json {
			struct visitor_state {
				options opt{};
				std::basic_ostream<char> *stream{ nullptr };
				int32 indent_level{ 0 };
				bool is_first_value{ true };
			};
		}    // namespace json

		template<>
		visitor visitor::create<json::visitor_state>(json::visitor_state &obj) {
			using State = json::visitor_state;
			// TODO specialize this for json writing
			visitor interface{};
			interface.data = &obj;
			static auto as_state = [](void *d) -> State & {
				return *static_cast<State *>(d);
			};

			static auto add_indentation = [](State &state) {
				if (state.indent_level > 0 && !state.opt.indent.empty()) {
					*state.stream << '\n';
					for (int32 i = 0; i < state.indent_level; ++i) {
						*state.stream << state.opt.indent;
					}
				}
			};

			static auto add_value_separator = [](State &state) {
				if (!state.is_first_value) {
					*state.stream << ',';
				}
			};

			static auto escape_string = [](const std::string_view &value) -> std::string {
				std::string str{ value.data(), value.size() };
				return str;
			};

			static auto add_name = [](State &state, std::string_view name) {
				if (!name.empty()) {
					*state.stream << '"' << escape_string(name) << '"' << ": ";
				}
			};

			auto setter = [](void *data, const std::string_view &name, const auto &value) {
				State &d = as_state(data);
				add_value_separator(d);
				add_indentation(d);
				add_name(d, name);

				using ValueType = std::decay_t<decltype(value)>;
				if constexpr (std::is_same_v<ValueType, bool>) {
					*d.stream << std::boolalpha << value;
				}
				else if constexpr (std::is_same_v<ValueType, std::string>) {
					*d.stream << '"' << escape_string(value) << '"';
				}
				else {
					*d.stream << value;
				}

				d.is_first_value = false;
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

			interface.vtable.set_null = [](void *data, const std::string_view &name) {
				State &d = as_state(data);
				add_value_separator(d);
				add_indentation(d);
				add_name(d, name);
				*d.stream << "null";
				d.is_first_value = false;
			};

			interface.vtable.begin_object = [](void *data, const std::string_view &name) {
				State &d = as_state(data);
				add_value_separator(d);
				add_indentation(d);

				// starting to write a new depth
				d.is_first_value = true;

				add_name(d, name);
				*d.stream << '{';
				++d.indent_level;
			};

			interface.vtable.end_object = [](void *data, const std::string_view &name) {
				State &d = as_state(data);

				--d.indent_level;
				add_indentation(d);

				*d.stream << '}';
				d.is_first_value = false;
			};

			interface.vtable.begin_array = [](void *data, const std::string_view &name) {
				State &d = as_state(data);
				add_value_separator(d);
				add_indentation(d);

				// starting to write a new depth
				d.is_first_value = true;

				add_name(d, name);

				*d.stream << '[';
				++d.indent_level;
			};

			interface.vtable.end_array = [](void *data, const std::string_view &name) {
				State &d = as_state(data);

				--d.indent_level;
				add_indentation(d);

				*d.stream << ']';
				d.is_first_value = false;
			};

			return interface;
		}
	}    // namespace schema
}    // namespace tc

namespace tc {
	namespace schema {
		namespace json {
			template<typename Object, typename Content>
			void to_object(Object &obj, Content &content) {
				parser_interface interface = parser_interface::create(obj);

				parse(interface, content);
			}

			template<typename Object, typename Stream>
			void to_stream(Object &obj, Stream &stream, options opt) {
				json::visitor_state state;
				state.opt = opt;
				state.stream = &stream;

				visitor interface = visitor::create(state);
				visit(interface, obj);
			}
		}    // namespace json
	}    // namespace schema
}    // namespace tc
