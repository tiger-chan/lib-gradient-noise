#ifndef UPROAR_CORE_SCHEMA_OPTIONS_HPP
#define UPROAR_CORE_SCHEMA_OPTIONS_HPP

#include "schema.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			template<typename X, typename Obj, typename Member>
			void add_constraint(schema::required<X> &detail, Obj &obj, Member &member);
			template<typename X, typename Obj, typename Member>
			void add_constraint(schema::not_null<X> &detail, Obj &obj, Member &member);
			template<typename X, typename Obj, typename Member>
			void add_constraint(schema::default_value<X> &detail, Obj &obj, Member &member);
			template<typename X, typename Obj, typename Member>
			void add_constraint(schema::range<X> &detail, Obj &obj, Member &member);
			
			template<typename X, typename Member, typename Y>
			bool check_constraint(const schema::range<X> &constraint, const Member &member, const Y& value);
		}

		template<typename Type>
		struct default_value {
			default_value(Type val)
				: fallback{ val } {
			}

			Type fallback;
		};

		template<typename Type>
		struct range {
			range(Type min, Type max)
				: min{ std::min(min, max) }
				, max{ std::max(min, max) } {
			}

			Type min;
			Type max;
		};

		template<typename Type>
		struct required {
		};

		template<typename Type>
		struct not_null {
		};

		template<typename X>
		struct is_member_constraint<schema::required<X>> : std::true_type {};
		template<typename X>
		struct is_member_constraint<schema::not_null<X>> : std::true_type {};
		template<typename X>
		struct is_member_constraint<schema::default_value<X>> : std::true_type {};
		template<typename X>
		struct is_member_constraint<schema::range<X>> : std::true_type {};
	}    // namespace schema
}    // namespace tc

#include "schema_options.inl"

#endif    // UPROAR_CORE_SCHEMA_OPTIONS_HPP
