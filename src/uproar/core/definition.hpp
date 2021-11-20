#ifndef UPROAR_CORE_SCHEMA_DEFINITION_HPP
#define UPROAR_CORE_SCHEMA_DEFINITION_HPP

#include "../config/config.hpp"

#include <type_traits>
#include <unordered_map>
#include <tuple>
#include <variant>

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
		struct context;
		struct member_context;
		struct context_stack;

		template<typename>
		struct required;
		template<typename>
		struct not_null;
		template<typename>
		struct default_value;
		template<typename>
		struct range;

		template<typename>
		struct is_member_constraint : std::false_type {};

		template<typename T>
		constexpr bool is_member_constraint_v = is_member_constraint<T>::value;

		template<typename X>
		struct is_member_constraint<schema::required<X>> : std::true_type {};
		template<typename X>
		struct is_member_constraint<schema::not_null<X>> : std::true_type {};
		template<typename X>
		struct is_member_constraint<schema::default_value<X>> : std::true_type {};
		template<typename X>
		struct is_member_constraint<schema::range<X>> : std::true_type {};

		template<typename X, typename = std::enable_if_t<std::is_enum_v<X>>>
		struct enum_to_string {
			static std::string_view to_string(X x);
			static X to_enum(std::string_view x);
		};

		namespace detail {
			using id_type = uint32_t;
			static const constexpr id_type null = UINT32_MAX;
			template<typename Type, typename Return>
			using member_ptr = Return Type::*;

			enum member_type {
				MT_bool,
				MT_char,
				MT_int16,
				MT_int32,
				MT_int64,
				MT_uint8,
				MT_uint16,
				MT_uint32,
				MT_uint64,
				MT_float,
				MT_double,
				MT_string,
				MT_enum,
				MT_object,
			};

			template<member_type Value>
			struct member_type_value_trait {
				static constexpr member_type value = Value;
			};

			template<typename Value>
			struct member_type_trait : public member_type_value_trait<std::is_enum_v<Value> ? MT_enum : MT_object> {};

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

			template<typename X, typename Obj, typename Member>
			extern void add_constraint(schema::required<X> &detail, Obj &obj, Member &member);
			template<typename X, typename Obj, typename Member>
			extern void add_constraint(schema::not_null<X> &detail, Obj &obj, Member &member);
			template<typename X, typename Obj, typename Member>
			extern void add_constraint(schema::default_value<X> &detail, Obj &obj, Member &member);
			template<typename X, typename Obj, typename Member>
			extern void add_constraint(schema::range<X> &detail, Obj &obj, Member &member);
			
			template<typename X, typename Member, typename Y>
			extern bool check_constraint(const schema::range<X> &constraint, const Member &member, const Y& value);

			template<typename Outer, typename ObjType>
			struct member;

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
				id_type sub_obj{ null };
				id_type primitive{ null };
				id_type default_value{ null };
				id_type range{ null };
				bool required : 1;
				bool not_null : 1;
			};

			template<typename Outer, typename ObjType>
			struct member_object {
				template<typename SetType>
				using Setter = void (*)(ObjType &, context_stack &, int, Outer &, id_type, std::string_view, const SetType&);

				template<typename Y>
				member_object(ObjType &obj, member_ptr<Outer, Y> mem);

				using push_back_t = void(*)(context_stack&, int, std::string_view);

				template<typename Value>
				void set_value(ObjType &obj, context_stack& stack, int stack_pos, Outer& outer, id_type member_idx, std::string_view name, const Value& value);

				std::tuple<Setter<bool>,
					Setter<char>,
					Setter<int16>,
					Setter<int32>,
					Setter<int64>,
					Setter<uint8>,
					Setter<uint16>,
					Setter<uint32>,
					Setter<uint64>,
					Setter<float>,
					Setter<double>,
					Setter<std::string>> setters;
				
				push_back_t push_back;


			private:
				template<typename Prop, typename Value>
				static void set_impl(context_stack& stack, int stack_pos, Outer& outer, member_ptr<Outer, Prop> ptr, std::string_view name, const Value& value);
				
				template<typename Y>
				static void push_back_impl(context_stack& stack, int stack_pos, std::string_view name);
			};

			template<typename T>
			struct PrimitiveType {
				using type = T;
			};

			using PrimitiveTypeVariant = std::variant<
					PrimitiveType<bool>,
					PrimitiveType<char>,
					PrimitiveType<int16>,
					PrimitiveType<int32>,
					PrimitiveType<int64>,
					PrimitiveType<uint8>,
					PrimitiveType<uint16>,
					PrimitiveType<uint32>,
					PrimitiveType<uint64>,
					PrimitiveType<float>,
					PrimitiveType<double>,
					PrimitiveType<std::string>>;

			template<typename Outer, typename ObjType>
			struct member_primitive {
				template<typename SetType>
				using PrimitiveSetter = void (*)(ObjType &obj, member_context& ctx, Outer &outer, id_type member_idx, const SetType&);
				using PrimitiveSetterVariant = std::variant<
					PrimitiveSetter<bool>,
					PrimitiveSetter<char>,
					PrimitiveSetter<int16>,
					PrimitiveSetter<int32>,
					PrimitiveSetter<int64>,
					PrimitiveSetter<uint8>,
					PrimitiveSetter<uint16>,
					PrimitiveSetter<uint32>,
					PrimitiveSetter<uint64>,
					PrimitiveSetter<float>,
					PrimitiveSetter<double>,
					PrimitiveSetter<std::string>>;

				template<typename Y>
				member_primitive(ObjType &obj, member_ptr<Outer, Y> mem);

				template<typename Value>
				void set_value(ObjType &obj, member_context& ctx, Outer &outer, id_type member_idx, const Value &value);
				
				PrimitiveSetterVariant setter;
				PrimitiveTypeVariant setter_type;

				template<typename Prop, typename Value>
				static void set_impl(ObjType &obj, member_context& ctx, Outer &outer, member<Outer, ObjType>& mem, const Value &value);
			};
		}    // namespace detail

		template<typename Type>
		struct object {
			template<typename X>
			friend struct object;

			using obj_member = detail::member<Type, object<Type>>;
			using obj_forward = detail::member_object<Type, object<Type>>;
			using obj_primitive = detail::member_primitive<Type, object<Type>>;

			template<typename Y, typename... Z, template<class> typename... Constraint>
			static void add_member(const char *name, detail::member_ptr<Type, Y> mem_ptr, const char *desc, Constraint<Z> &&...constraints);

			static object<Type> instance;

			std::vector<obj_member> props;
			std::vector<obj_forward> sub_objects;
			std::vector<obj_primitive> primitives;
			std::unordered_map<std::string_view, detail::id_type> prop_lookup;
			template<typename Return>
			inline static std::vector<detail::member_ptr<Type, Return>> prop_ptrs{};
			template<typename X>
			inline static std::vector<schema::default_value<X>> default_values{};
			template<typename X>
			inline static std::vector<schema::range<X>> ranges{};

			template<typename Value>
			void set_value(context_stack &stack, Type &obj, std::string_view name, const Value& value);
			
			template<typename Enum>
			static void set_enum(context_stack& stack, Type &obj, std::string_view name, Enum value);
			
			static void set_string(context_stack& stack, Type &obj, std::string_view name, const std::string& value);
			
			template<typename Prop, typename Value>
			static void set_value(context_stack& stack, int stack_pos, Type &obj, std::string_view name, const Value& value);

			static void push_back(context_stack& stack, int stack_pos, std::string_view name);
			static void push_back(context_stack& stack, std::string_view name);
			static context pop_back(context_stack& stack);

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
			static void set_value(member_context& member_ctx, obj_member& member, Type& obj, const Value& value);
		};

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

		struct member_context {
			bool dirty : 1;
			bool is_in_range : 1;
		};

		struct context {
			detail::id_type object;
			detail::member_type type;
			std::string_view name;
			std::vector<member_context> member_context;
		};

		struct context_stack {
			[[nodiscard]] context& emplace_back();
			[[maybe_unused]] context pop_back();

			context& operator[](int i);
			const context& operator[](int i) const;

			int size() const;
			bool empty() const;

			std::vector<context> stack;
		};
	}    // namespace schema
}    // namespace tc

#include "definition.inl"

#endif    // UPROAR_CORE_SCHEMA_DEFINITION_HPP
