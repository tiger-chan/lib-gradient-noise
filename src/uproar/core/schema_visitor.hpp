#ifndef UPROAR_CORE_SCHEMA_VISITOR_HPP
#define UPROAR_CORE_SCHEMA_VISITOR_HPP

#include "schema_shared.hpp"

#include <string_view>

namespace tc {
	namespace schema {
		namespace detail {
			struct visitor_vtable {
				template<typename V>
				using SetterT = void (*)(void *, const std::string_view &, const V &);
				using NullSetterT = void (*)(void *, const std::string_view &);
				using BeginObject = void (*)(void *, const std::string_view &);
				using EndObject = void (*)(void *, const std::string_view &);

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

				BeginObject begin_array;
				BeginObject begin_object;

				EndObject end_array;
				EndObject end_object;
			};
		}    // namespace detail

		struct visitor {
		public:
			template<typename Data>
			static visitor create(Data &obj);

			template<typename Type>
			void set(const std::string_view &name, const Type &value);
			void set_null(const std::string_view &name);

			void begin_array(const std::string_view &name);
			void begin_object(const std::string_view &name);

			void end_array(const std::string_view &name);
			void end_object(const std::string_view &name);

		private:
			detail::visitor_vtable vtable;
			void *data;
		};

		template<typename Type>
		void visit(visitor &visitor, Type &obj);
	}    // namespace schema
}    // namespace tc

#include "schema_visitor.inl"

#endif    // UPROAR_CORE_SCHEMA_VISITOR_HPP
