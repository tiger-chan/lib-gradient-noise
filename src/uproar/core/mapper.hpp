#ifndef UPROAR_CORE_SCHEMA_MAPPER_HPP
#define UPROAR_CORE_SCHEMA_MAPPER_HPP

#include <string_view>

#ifndef TC_STDINT
#define TC_STDINT
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
		namespace detail {
			struct parser_vtable {
				template<typename V>
				using SetterT = void(*)(context_stack &, void*, std::string_view, const V&);
				using NullSetterT = void(*)(context_stack &, void*, std::string_view);

				SetterT<bool> set_bool;
				
				SetterT<char> set_char;
				SetterT<int16> set_int16;
				SetterT<int32> set_int32;
				SetterT<int64> set_int64;
				
				SetterT<uint8> set_uint8;
				SetterT<uint16> set_uint16;
				SetterT<uint32> set_uint32;
				SetterT<uint64> set_uint64;
				
				SetterT<float> set_float;
				SetterT<double> set_double;

				SetterT<std::string> set_string;

				NullSetterT set_null;

				using begin_object_t = void(*)(context_stack &, std::string_view);
				using end_object_t = void(*)(context_stack &);
				
				begin_object_t begin_object;
				end_object_t end_object;
				begin_object_t begin_array;
				end_object_t end_array;
			};
		}

		struct parser_interface {
			template<typename V>
			void set_value(std::string_view name, const V& value);

			void set_null(std::string_view name);

			void begin_object(std::string_view name);
			void end_object();
			void begin_array(std::string_view name);
			void end_array();
			void begin_map(std::string_view name);
			void end_map();

			template<typename T>
			static parser_interface create(T& obj);

			private:
			detail::parser_vtable vtable;
			context_stack stack;
			void *data;
		};

		template<typename Type, typename ContentWrapper>
		void to_object(Type &obj, ContentWrapper& content);

		extern void parse(parser_interface& interface, std::string_view content);
	}
}

#include "mapper.inl"

#endif // UPROAR_CORE_SCHEMA_MAPPER_HPP
