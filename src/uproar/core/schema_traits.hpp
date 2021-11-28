#ifndef UPROAR_CORE_SCHEMA_TRAITS_HPP
#define UPROAR_CORE_SCHEMA_TRAITS_HPP

#include <type_traits>
#include "schema_shared.hpp"

namespace tc {
	namespace schema {
		template<typename>
		struct is_member_constraint : std::false_type {};

		template<typename T>
		constexpr bool is_member_constraint_v = is_member_constraint<T>::value;

		namespace detail {
			template<typename, typename = std::void_t<>>
			struct emplace_back_concept : std::false_type {
			};

			template<typename T>
			struct emplace_back_concept<T, std::void_t<decltype(std::declval<T>().emplace_back())>> : std::true_type {
			};

			template<typename T>
			static constexpr bool emplace_back_concept_v = emplace_back_concept<T>::value;
		}

		template<typename Type>
		struct is_container {
			// A needed feature of a container type.
			static constexpr bool value = detail::emplace_back_concept_v<Type>;
		};

		template<>
		struct is_container<std::string> : std::false_type {};
		
		template<typename T>
		struct is_container<std::vector<T>> : std::true_type {};

		template<typename Type>
		static constexpr bool is_container_v = is_container<Type>::value;
		
		template<typename Type>
		struct is_map : std::false_type {
		};
		
		template<typename Type>
		static constexpr bool is_map_v = is_map<Type>::value;

		template<member_type Value>
		struct member_type_value_trait {
			static constexpr member_type value = Value;
		};

		template<typename T>
		static constexpr member_type default_type() {
			if (std::is_enum_v<T>) {
				return MT_enum;
			}
			else if (is_container_v<T>) {
				return MT_array;
			}
			else {
				return MT_object;
			}
		}

		template<typename Value>
		struct member_type_trait : public member_type_value_trait<default_type<Value>()> {};

		template<>
		struct member_type_trait<bool> : public member_type_value_trait<MT_bool> {};

		template<>
		struct member_type_trait<uint8> : public member_type_value_trait<MT_uint8> {};

		template<>
		struct member_type_trait<uint16> : public member_type_value_trait<MT_uint16> {};

		template<>
		struct member_type_trait<uint32> : public member_type_value_trait<MT_uint32> {};
		
		template<>
		struct member_type_trait<uint64> : public member_type_value_trait<MT_uint64> {};

		template<>
		struct member_type_trait<char> : public member_type_value_trait<MT_char> {};

		template<>
		struct member_type_trait<int16> : public member_type_value_trait<MT_int16> {};

		template<>
		struct member_type_trait<int32> : public member_type_value_trait<MT_int32> {};
		
		template<>
		struct member_type_trait<int64> : public member_type_value_trait<MT_int64> {};
		
		template<>
		struct member_type_trait<float> : public member_type_value_trait<MT_float> {};
		
		template<>
		struct member_type_trait<double> : public member_type_value_trait<MT_double> {};
		
		template<>
		struct member_type_trait<std::string> : public member_type_value_trait<MT_string> {};
		
		template<>
		struct member_type_trait<char *> : public member_type_value_trait<MT_string> {};

		template<typename T>
		static constexpr member_type member_type_trait_v = member_type_trait<T>::value;

	}    // namespace schema
}    // namespace tc

#endif    // UPROAR_CORE_SCHEMA_TRAITS_HPP
