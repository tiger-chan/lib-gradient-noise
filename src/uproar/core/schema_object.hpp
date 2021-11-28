#ifndef UPROAR_CORE_SCHEMA_OBJECT_HPP
#define UPROAR_CORE_SCHEMA_OBJECT_HPP

#include "schema.hpp"
#include "schema_context.hpp"
#include "schema_member.hpp"
#include "schema_member_object.hpp"
#include "schema_member_primitive.hpp"
#include "schema_options.hpp"

#include <unordered_map>

namespace tc {
	namespace schema {
		namespace detail {
			template<typename Type>
			struct object {
			public:
				using obj_member = detail::member<Type, object<Type>>;
				using obj_forward = detail::member_object<Type, object<Type>>;
				using obj_primitive = detail::member_primitive<Type, object<Type>>;

				template<typename X>
				friend struct object;

				template<typename Y, typename... Z, template<class> typename... Constraint>
				void add_member(const char *name, member_ptr<Type, Y> mem_ptr, const char *desc, Constraint<Z> &&...constraints);

				template<typename Value>
				void set_value(context_stack &stack, Type &obj, std::string_view name, const Value &value);

				template<typename Prop, typename Value>
				void set_value(context_stack &stack, int stack_pos, Type &obj, std::string_view name, const Value &value);

				void push_back(context_stack &stack, int stack_pos, std::string_view name);
				void push_back(context_stack &stack, std::string_view name);
				void push_back(context_stack &stack, int stack_pos, int32 array_idx);
				void push_back(context_stack &stack, int32 array_idx);
				context pop_back(context_stack &stack);

				/**
				 * @brief Set the value object
				 *
				 * @tparam X -- The type we are setting to
				 * @tparam Y -- A compatable type to X
				 * @param member
				 * @param obj
				 * @param value
				 */
				template<typename Prop, typename Value>
				void set_value(member_context &member_ctx, obj_member &member, Type &obj, const Value &value);

				static object<Type> instance;
				static constexpr member_type type = member_type_trait_v<Type>;

				std::vector<obj_member> props;
				std::vector<obj_forward> children;
				std::vector<obj_primitive> primitives;
				std::unordered_map<std::string_view, id_type> prop_lookup;
				template<typename Return>
				inline static std::vector<member_ptr<Type, Return>> prop_ptrs{};
				template<typename X>
				inline static std::vector<schema::default_value<X>> default_values{};
				template<typename X>
				inline static std::vector<schema::range<X>> ranges{};
			};
		}    // namespace detail
	}    // namespace schema
}    // namespace tc

#include "schema_object.inl"

#endif    // UPROAR_CORE_SCHEMA_OBJECT_HPP
