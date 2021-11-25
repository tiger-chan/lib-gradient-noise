#ifndef UPROAR_CORE_SCHEMA_MEMBER_HPP
#define UPROAR_CORE_SCHEMA_MEMBER_HPP

#include "schema.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			/**
			 * @brief member is a generic wrapper for a member property of Outer.
			 *  
			 * Containes only id/idx to the locations where the actual pointers are.
			 * 
			 * @tparam Outer 
			 * @tparam ObjType 
			 */
			template<typename Outer, typename ObjType>
			struct member {
				template<typename Y, typename... Z, template<class> typename... Constraint>
				member(ObjType &obj, std::string_view name, member_ptr<Outer, Y> mem, std::string_view desc, Constraint<Z> &&...details);

				template<typename Y, template<typename> typename Constraint>
				void add_constraint(ObjType &obj, Constraint<Y> &detail);

				std::string_view name{};
				std::string_view desc{};
				member_type type{};

				id_type ptr{ null };
				id_type child{ null };
				id_type primitive{ null };
				id_type default_value{ null };
				id_type range{ null };
				bool required : 1;
				bool not_null : 1;
			};
		}    // namespace detail
	}    // namespace schema
}    // namespace tc

#include "schema_member.inl"

#endif    // UPROAR_CORE_SCHEMA_MEMBER_HPP
