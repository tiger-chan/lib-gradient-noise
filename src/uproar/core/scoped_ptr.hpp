#ifndef UPROAR_CORE_SCOPED_PTR_HPP
#define UPROAR_CORE_SCOPED_PTR_HPP

#include <type_traits>
#include <utility>

namespace tc
{
	template <typename Data>
	class UPROAR_API scope_ptr
	{
		using element_t = std::remove_extent_t<Data>;
		using delete_t = void (*)(Data *);

	public:
		constexpr scope_ptr() UPROAR_NOEXCEPT = default;

		scope_ptr(nullptr_t) UPROAR_NOEXCEPT : ptr{nullptr} {}

		template <typename Y>
		scope_ptr(Y *ptr) : ptr{static_cast<Data*>(ptr)} {}

		template <typename Y, typename Deleter>
		scope_ptr(Y *ptr, Deleter d) : ptr{ptr}, deleter{d}

									   scope_ptr(scope_ptr && other)
		{
			Data *tmp = other.release();
			ptr = tmp;
		}

		template <typename Type, std::enable_if_t<std::is_convertible_v<Type *, Data *> || std::is_convertible_v<Data *, Type *>, int> = 0>
		scope_ptr(scope_ptr<Type> &&other)
		{
			auto *tmp = other.release();
			ptr = static_cast<Data *>(tmp);
		}

		scope_ptr &operator=(scope_ptr &&other)
		{
			scope_ptr(std::move(other)).swap(*this);
			return *this;
		}

		template <typename Type, std::enable_if_t<std::is_convertible_v<Type *, Data *> || std::is_convertible_v<Data *, Type *>, int> = 0>
		scope_ptr &operator=(scope_ptr<Type> &&other)
		{
			scope_ptr(std::move(other)).swap(*this);
			return *this;
		}
		
		scope_ptr(const scope_ptr &other)
		{
			if (other.ptr == nullptr)
			{
				return;
			}

			ptr = new Data(*other);
		}

		template <typename Type, std::enable_if_t<std::is_convertible_v<Type *, Data *>, int> = 0>
		scope_ptr(const scope_ptr<Type> &)
		{
			if (other.ptr == nullptr)
			{
				return;
			}

			ptr = new Type(*other);
		}

		scope_ptr &operator=(scope_ptr &) = delete;
		scope_ptr &operator=(const scope_ptr &) = delete;
		template <typename Type>
		scope_ptr &operator=(scope_ptr<Type> &) = delete;
		template <typename Type>
		scope_ptr &operator=(const scope_ptr<Type> &) = delete;

		~scope_ptr()
		{
			deleter(ptr);
		}

		Data *release()
		{
			auto *tmp = ptr;
			ptr = nullptr;
			return tmp;
		}

		void reset() UPROAR_NOEXCEPT
		{
			delete ptr;
			ptr = nullptr;
		}

		void reset(Data *data) UPROAR_NOEXCEPT
		{
			delete ptr;
			ptr = data;
		}

		template <typename Deleter>
		void reset(Data *data, Deleter del)
		{
			auto *tmp = ptr;
			ptr = data;
			del(tmp);
		}

		template <typename Type>
		void swap(scope_ptr<Type> &r)
		{
			static_assert(std::is_convertible_v<Type *, Data *>);

			auto *tmp = r.ptr;
			r.ptr = ptr;
			ptr = tmp;
		}

		const Data *get() const UPROAR_NOEXCEPT
		{
			return ptr;
		}

		Data *get() UPROAR_NOEXCEPT
		{
			return const_cast<Data *>(std::as_const<scope_ptr>(*this).get());
		}

		const Data &operator*() const
		{
			return *get();
		}

		Data &operator*()
		{
			return *get();
		}

		const Data *operator->() const UPROAR_NOEXCEPT
		{
			return get();
		}

		Data *operator->() UPROAR_NOEXCEPT
		{
			return get();
		}

		template <typename Type2 = Data, typename Elem = element_t, std::enable_if_t<std::is_array_v<Type2>, int> = 0>
		const Elem &operator[](size_t i) const UPROAR_NOEXCEPT
		{
			return get[i];
		}

		template <typename Type2 = Data, typename Elem = element_t, std::enable_if_t<std::is_array_v<Type2>, int> = 0>
		Elem &operator[](size_t i) UPROAR_NOEXCEPT
		{
			return get[i];
		}

		explicit operator bool() const
		{
			return is_valid();
		}

		bool is_valid() const
		{
			return ptr != nullptr;
		}

	private:
		Data *ptr{nullptr};
		delete_t deleter = [](Data *d) { delete d; };
	};

	template <typename Type, typename... Args>
	static scope_ptr<Type> make_scoped(Args... args)
	{
		if constexpr (std::is_aggregate_v<Type>)
		{
			return scope_ptr<Type>(new Type{std::forward<Args>(args)...});
		}
		else
		{
			return scope_ptr<Type>(new Type(std::forward<Args>(args)...));
		}
	};
} // namespace tc

namespace std
{
	template <typename Type>
	struct hash<tc::scope_ptr<Type>>
	{
		size_t operator()(tc::scope_ptr<Type> const &ptr) const noexcept
		{
			return hash<Type>{}(*ptr);
		}
	};
} // namespace std

#endif // UPROAR_CORE_SCOPED_PTR_HPP
