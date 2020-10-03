#ifndef UPROAR_CORE_SINGLETON_BASE_HPP
#define UPROAR_CORE_SINGLETON_BASE_HPP

#include "../config/config.hpp"
#include "attributes.hpp"

namespace tc
{
	template<typename Type>
	class UPROAR_API singleton_base
	{
		public:
		static Type& instance() {
			static Type t{};
			return t;
		}
	};
}

#endif // UPROAR_CORE_SINGLETON_BASE_HPP
