#ifndef UPROAR_CORE_SCHEMA_TRAITS_HPP
#define UPROAR_CORE_SCHEMA_TRAITS_HPP

#include "schema_shared.hpp"

#include <type_traits>

namespace tc {
	namespace schema {
		template<typename>
		struct is_member_constraint : std::false_type {};

		template<typename T>
		constexpr bool is_member_constraint_v = is_member_constraint<T>::value;

		namespace detail {
			/************ Resize Concept **********/
			template<typename, typename = std::void_t<>>
			struct resize_concept : std::false_type {
			};

			template<typename T>
			struct resize_concept<T, std::void_t<decltype(std::declval<T>().resize(0))>> : std::true_type {
			};

			template<typename T>
			static constexpr bool resize_concept_v = resize_concept<T>::value;
			/************ Resize Concept End *******/

			/************ Size Concept *************/
			template<typename, typename = std::void_t<>>
			struct size_concept : std::false_type {
			};

			template<typename T>
			struct size_concept<T, std::void_t<decltype(std::declval<T>().size())>> : std::true_type {
			};

			template<typename T>
			static constexpr bool size_concept_v = size_concept<T>::value;
			/************ Size Concept End **********/

			/************ Int Indexer Concept *************/
			template<typename, typename = std::void_t<>>
			struct int_indexer_concept : std::false_type {
			};

			template<typename T>
			struct int_indexer_concept<T, std::void_t<decltype(std::declval<T>()[0])>> : std::true_type {
			};

			template<typename T>
			static constexpr bool int_indexer_concept_v = int_indexer_concept<T>::value;
			/************ Int Indexer Concept End **********/

			/************ String Indexer Concept *************/
			template<typename, typename = std::void_t<>>
			struct str_indexer_concept : std::false_type {
			};

			template<typename T>
			struct str_indexer_concept<T, std::void_t<decltype(std::declval<T>()[std::string{}])>> : std::true_type {
			};

			template<typename T>
			static constexpr bool str_indexer_concept_v = str_indexer_concept<T>::value;
			/************ String Indexer Concept End **********/
		}    // namespace detail

		template<typename Type>
		struct is_container {
			// A needed feature of a container type.
			static constexpr bool value = detail::resize_concept_v<Type> && detail::size_concept_v<Type> && detail::int_indexer_concept_v<Type>;
		};

		template<>
		struct is_container<std::string> : std::false_type {};

		template<typename T>
		struct is_container<std::vector<T>> : std::true_type {};

		template<typename Type>
		static constexpr bool is_container_v = is_container<Type>::value;

		template<typename Type>
		struct is_map {
			// A needed feature of a container type.
			static constexpr bool value = detail::str_indexer_concept_v<Type> && detail::size_concept_v<Type>;
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
			else if (is_map_v<T>) {
				return MT_map;
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
