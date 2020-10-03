#ifndef UPROAR_CORE_FACTORY_BASE_HPP
#define UPROAR_CORE_FACTORY_BASE_HPP

#include "../config/config.hpp"
#include "attributes.hpp"
#include "unordered_map"
#include "scoped_ptr.hpp"

namespace tc
{
	template<typename Key, typename Base>
	class UPROAR_API factory_base
	{
		using spawn_t = scope_ptr<Base>(*)();

		template<typename Type>
		static scope_ptr<Base> make_type() {
			auto t = make_scoped<Type>();
			scope_ptr<Base> base;
			base = std::move(t);
			return base;
		};

		public:
		template<typename Type>
		bool record(const Key& key)
		{
			if (registered_types.find(key) != std::end(registered_types)) {
				return false;
			}

			auto x = registered_types.emplace(key, []() { return make_type<Type>(); });
			return x.second;
		}

		scope_ptr<Base> spawn(const Key& key) const {
			auto s = registered_types.find(key);
			if (s != std::end(registered_types)) {
				auto spawner = s->second;
				return spawner();
			}

			return {nullptr};
		}

		template<typename Type>
		scope_ptr<Type> spawn(const Key& key) const {
			auto s = registered_types.find(key);
			if (s != std::end(registered_types)) {
				auto spawner = s->second;
				return scope_ptr<Type>(spawner());
			}

			return {nullptr};
		}

		private:
		std::unordered_map<Key, spawn_t> registered_types;
	};
}

#endif // UPROAR_CORE_FACTORY_BASE_HPP
