#ifndef UPROAR_CORE_JSON_HPP
#define UPROAR_CORE_JSON_HPP

#include "../config/config.hpp"
#include "attributes.hpp"
#include "scoped_ptr.hpp"
#include <memory>
#include <unordered_map>
#include <string>
#include <variant>

namespace tc
{
	namespace json
	{
		class object;

		struct numeric
		{
			union numeric_t
			{
				decimal_t decimal;
				float floating_point;
				uint8_t uinteger;
				uint32_t uint;
				int32_t integer;
				int64_t big_integer;

				operator decimal_t() const
				{
					return decimal;
				}

				operator float() const
				{
					return floating_point;
				}

				operator uint8_t() const
				{
					return uinteger;
				}

				operator uint32_t() const
				{
					return uint;
				}

				operator int32_t() const
				{
					return integer;
				}
				
				operator int64_t() const
				{
					return big_integer;
				}
			};

			template<typename Type>
			static constexpr bool holds_alternative_numeric()
			{
				return std::is_same_v<decimal_t, Type>
					|| std::is_same_v<float, Type>
					|| std::is_same_v<uint8_t, Type>
					|| std::is_same_v<uint32_t, Type>
					|| std::is_same_v<int32_t, Type>
					|| std::is_same_v<int64_t, Type>;
			}

			template<typename Type>
			bool holds_alternative() const
			{
				return holds_alternative_numeric<Type>();
			}

			numeric(decimal_t v)
			{
				value.decimal = v;
			}

			numeric(float v)
			{
				value.floating_point = v;
			}

			numeric(uint8_t v)
			{
				value.uinteger = v;
			}

			numeric(uint32_t v)
			{
				value.uint = v;
			}

			numeric(int32_t v)
			{
				value.integer = v;
			}

			numeric(int64_t v)
			{
				value.big_integer = v;
			}

			const numeric_t& get() const
			{
				return value;
			}

			numeric_t& get()
			{
				return value;
			}

			template<typename Type>
			const Type& as() const
			{
				return *(static_cast<const Type*>(reinterpret_cast<const void*>(&value)));
			}

			template<typename Type>
			Type& as()
			{
				return const_cast<Type&>(std::as_const(*this).as<Type>());
			}

			private:
			numeric_t value;
		};

		namespace internal
		{
			using value_t = std::variant<std::nullptr_t, std::string, numeric, std::vector<object>, scope_ptr<object>>;
		}

		class UPROAR_API value : public internal::value_t
		{
			using Super = internal::value_t;
			public:
			using Super::Super;

			value(const char* c_str)
			{
				Super s(std::string{c_str });
				s.swap(*this);
			}

			template<typename Type>
			value& operator=(const Type& v)
			{
				Super s(v);
				s.swap(*this);
				return *this;
			}

			template<typename Type>
			value& operator=(Type&& v)
			{
				Super s(std::move(v));
				s.swap(*this);
				return *this;
			}

			template<typename Type>
			bool holds_alternative() const
			{
				if constexpr (numeric::holds_alternative_numeric<Type>())
				{
					return std::holds_alternative<numeric>(*this) && std::get<numeric>(*this).holds_alternative<Type>();
				}
				else
				{
					return std::holds_alternative<Type>(*this);
				}
			}

			template<typename Type>
			Type& as()
			{
				if constexpr (numeric::holds_alternative_numeric<Type>()) {
					auto& ref = std::get<numeric>(*this);
					return ref.as<Type>();
				}
				else
				{
					UPROAR_ASSERT(holds_alternative<Type>());
					return std::get<Type>(*this);
				}
			}

			template<typename Type>
			const Type& as() const
			{
				if constexpr (numeric::holds_alternative_numeric<Type>()) {
					const auto& ref = std::get<numeric>(*this);
					return ref.as<Type>();
				}
				else
				{
					UPROAR_ASSERT(holds_alternative<Type>());
					return std::get<Type>(*this);
				}
			}
		};

		class UPROAR_API object : public std::unordered_map<std::string, value>
		{
			using Super = std::unordered_map<std::string, value>;
			public:
			using pair_t = typename Super::value_type;
			using Super::Super;
		};
	}
}

#endif // UPROAR_CORE_JSON_HPP
