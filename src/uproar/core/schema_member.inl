#include "schema_member.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			template<typename Outer, typename ObjType, typename Type, template<class...> typename Container, typename ...Rest>
			void add_primitive(ObjType &obj, member<Outer, ObjType>& mem, member_object_type<Container<Type, Rest...>>) {
				using ContainerType = Container<Type, Rest...>;
				if constexpr (member_type_trait_v<Type> < MT_object) {
					mem.primitive = id_type(obj.primitives.size());
					obj.primitives.emplace_back(obj, member_object_type<Type>{});
				}
			}

			template<typename Outer, typename ObjType>
			template<typename Y, typename... Z, template<class> typename... Constraint>
			member<Outer, ObjType>::member(ObjType &obj, std::string_view name, member_object_type<Y> mem, std::string_view desc, Constraint<Z> &&...details)
				: name{ name }
				, desc{ desc }
				, type{ member_type_trait_v<Y> } {
				if constexpr (member_type_trait_v<Y> == MT_object) {
					child = id_type(obj.children.size());
					obj.children.emplace_back(obj, member_object_type<Y>{});
				}
				if constexpr (member_type_trait_v<Y> == MT_array) {
					child = id_type(obj.children.size());
					obj.children.emplace_back(obj, member_object_type<Y>{});
					add_primitive(obj, *this, member_object_type<Y>{});
				}

				(add_constraint(obj, std::forward<Constraint<Z>>(details)), ...);
			}

			template<typename Outer, typename ObjType>
			template<typename Y, typename... Z, template<class> typename... Constraint>
			member<Outer, ObjType>::member(ObjType &obj, std::string_view name, member_ptr<Outer, Y> mem, std::string_view desc, Constraint<Z> &&...details)
				: name{ name }
				, desc{ desc }
				, type{ member_type_trait_v<Y> } {
				ptr = id_type(obj.prop_ptrs<Y>.size());
				obj.prop_ptrs<Y>.emplace_back(mem);

				if constexpr (member_type_trait_v<Y> == MT_object) {
					child = id_type(obj.children.size());
					obj.children.emplace_back(obj, member_object_type<Y>{});
				}
				else if constexpr (member_type_trait_v<Y> == MT_array) {
					child = id_type(obj.children.size());
					obj.children.emplace_back(obj, member_object_type<Y>{});
				}
				else {
					primitive = id_type(obj.primitives.size());
					obj.primitives.emplace_back(obj, mem);
				}

				(add_constraint(obj, std::forward<Constraint<Z>>(details)), ...);
			}

			template<typename Outer, typename ObjType>
			template<typename Y, template<typename> typename Constraint>
			void member<Outer, ObjType>::add_constraint(ObjType &obj, Constraint<Y> &detail) {
				static_assert(is_member_constraint_v<Constraint<Y>>, "must be a known member constraint type");
				detail::add_constraint(detail, obj, *this);
			}
		}    // namespace detail
	}    // namespace schema
}    // namespace tc
