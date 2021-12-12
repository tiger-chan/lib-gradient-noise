#include "schema_member.hpp"

namespace tc {
	namespace schema {
		namespace detail {
			template<typename>
			struct ConatinerMemberHelper;
			template<template<class...> typename Container, typename Type, typename... Rest>
			struct ConatinerMemberHelper<Container<Type, Rest...>> {
				using Array = Container<Type, Rest...>;

				template<typename Outer>
				static void handle(object<Outer> &obj, member<Outer, object<Outer>> &mem) {
					if constexpr (member_type_trait_v<Type> < MT_object) {
						mem.primitive = id_type(obj.primitives.size());
						obj.primitives.emplace_back(obj, member_object_type<Type>{});
					}
				}
			};

			template<typename>
			struct MapMemberHelper;
			template<template<class...> typename Container, typename Key, typename Type, typename... Rest>
			struct MapMemberHelper<Container<Key, Type, Rest...>> {
				using Map = Container<Key, Type, Rest...>;

				template<typename Outer>
				static void handle(object<Outer> &obj, member<Outer, object<Outer>> &mem) {
					if constexpr (member_type_trait_v<Type> < MT_object) {
						mem.primitive = id_type(obj.primitives.size());
						obj.primitives.emplace_back(obj, member_object_type<Type>{});
					}
				}
			};

			template<typename Outer, typename ObjType, typename Type>
			void add_primitive(ObjType &obj, member<Outer, ObjType> &mem, member_object_type<Type>) {
				if constexpr (member_type_trait_v<Type> == MT_array) {
					ConatinerMemberHelper<Type>::handle(obj, mem);
				}
				else if constexpr (member_type_trait_v<Type> == MT_map) {
					MapMemberHelper<Type>::handle(obj, mem);
				}
			}

			template<typename Outer, typename ObjType>
			template<typename Y, typename... Z, template<class> typename... Constraint>
			member<Outer, ObjType>::member(ObjType &obj, std::string_view name, member_object_type<Y> mem, std::string_view desc, Constraint<Z> &&...details)
				: type_id{ type_identifier<Y>() }
				, name{ name }
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
				else if constexpr (member_type_trait_v<Y> == MT_map) {
					child = id_type(obj.children.size());
					obj.children.emplace_back(obj, member_object_type<Y>{});
					add_primitive(obj, *this, member_object_type<Y>{});
				}
				else if constexpr (member_type_trait_v<Y> < MT_object) {
					primitive = id_type(obj.primitives.size());
					obj.primitives.emplace_back(obj, member_object_type_v<Y>);
				}

				(add_constraint(obj, std::forward<Constraint<Z>>(details)), ...);
			}

			template<typename Outer, typename ObjType>
			template<typename Y, typename... Z, template<class> typename... Constraint>
			member<Outer, ObjType>::member(ObjType &obj, std::string_view name, member_ptr<Outer, Y> mem, std::string_view desc, Constraint<Z> &&...details)
				: type_id{ type_identifier<Y>() }
				, name{ name }
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
				else if constexpr (member_type_trait_v<Y> == MT_map) {
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
