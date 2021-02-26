#ifndef TRANSITION_H
#define TRANSITION_H

#include <utility>
#include "helpers.h"

namespace Detail
{
	template <typename C, typename Arg>
	class HasOnExitConstRef
	{
		template <typename T>
		static std::true_type testSignature(void(T::*)(const Arg&));

		template <typename T>
		static decltype(testSignature(&T::onExit)) test(std::nullptr_t);

		template <typename T>
		static std::false_type test(...);

	public:
		using type = decltype(test<C>(nullptr));
		static const bool value = type::value;
	};


	template <typename C, typename Arg>
	class HasOnExitMove
	{
		template <typename T>
		static std::true_type testSignature(void(T::*)(Arg&&));

		template <typename T>
		static decltype(testSignature(&T::onExit)) test(std::nullptr_t);

		template <typename T>
		static std::false_type test(...);

	public:
		using type = decltype(test<C>(nullptr));
		static const bool value = type::value;
	};


	template <typename C, typename Arg>
	class HasOnEnterConstRef
	{
		template <typename T>
		static std::true_type testSignature(void(T::*)(const Arg&));

		template <typename T>
		static decltype(testSignature(&T::onEnter)) test(std::nullptr_t);

		template <typename T>
		static std::false_type test(...);

	public:
		using type = decltype(test<C>(nullptr));
		static const bool value = type::value;
	};


	template <typename C, typename Arg>
	class HasOnEnterMove
	{
		template <typename T>
		static std::true_type testSignature(void(T::*)(Arg&&));

		template <typename T>
		static decltype(testSignature(&T::onEnter)) test(std::nullptr_t);

		template <typename T>
		static std::false_type test(...);

	public:
		using type = decltype(test<C>(nullptr));
		static const bool value = type::value;
	};


	template<typename T>
	struct StateWrapper : public T
	{
		using Base = T;

		template<typename Arg,
				 typename = std::enable_if_t<HasOnEnterMove<T, Arg>::value>
				>
		void doOnEnter(Arg&& arg)
		{
			static_assert(!HasOnEnterConstRef<T, Arg>::value, "Must have only one onEnter method for each event with move or const reference. Actually defined both");
			T::onEnter(std::move(arg));
		}

		template<typename Arg,
				 typename = std::enable_if_t<HasOnEnterConstRef<T, Arg>::value>
				>
		void doOnEnter(const Arg& arg)
		{
			static_assert(!HasOnEnterMove<T, Arg>::value, "Must have only one onEnter method for each event with move or const reference. Actually defined both");
			T::onEnter(arg);
		}

		template<typename Arg,
				 typename = std::enable_if_t<!HasOnEnterConstRef<T, Arg>::value && !HasOnEnterMove<T, Arg>::value>
				>
		void doOnEnter(const Arg&) const
		{
			std::cout << "has no onEnter method" << std::endl;
		}

		template<typename Arg,
				 typename = std::enable_if_t<HasOnExitMove<T, Arg>::value>
				>
		void doOnExit(Arg&& arg)
		{
			static_assert(!HasOnExitConstRef<T, Arg>::value, "Must have only one onExit method for each event with move or const reference. Actually defined both");
			T::onExit(std::move(arg));
		}

		template<typename Arg,
				 typename = std::enable_if_t<HasOnExitConstRef<T, Arg>::value>
				>
		void doOnExit(const Arg& arg)
		{
			static_assert(!HasOnExitMove<T, Arg>::value, "Must have only one onExit method for each event with move or const reference. Actually defined both");
			T::onExit(arg);
		}

		template<typename Arg,
				 typename = std::enable_if_t<!HasOnExitConstRef<T, Arg>::value && !HasOnExitMove<T, Arg>::value>
				>
		void doOnExit(const Arg&) const
		{
			std::cout << "has no onExit method" << std::endl;
		}
	};


	template <typename C, typename S1, typename E, typename S2>
	class HasParenthesisConstRef
	{
		template <typename T>
		static std::true_type testSignature(bool(T::*)(S1&, const E&, S2&));

		template <typename T>
		static decltype(testSignature(&T::operator())) test(std::nullptr_t);

		template <typename T>
		static std::false_type test(...);

	public:
		using type = decltype(test<C>(nullptr));
		static const bool value = type::value;
	};


	template <typename C, typename S1, typename E, typename S2>
	class HasParenthesisMove
	{
		template <typename T>
		static std::true_type testSignature(bool(T::*)(S1&, E&&, S2&));

		template <typename T>
		static decltype(testSignature(&T::operator())) test(std::nullptr_t);

		template <typename T>
		static std::false_type test(...);

	public:
		using type = decltype(test<C>(nullptr));
		static const bool value = type::value;
	};


	template<typename FS, typename E, typename TS, typename C>
	struct Transition
	{
		static_assert(HasParenthesisConstRef<C, FS, E, TS>::value || HasParenthesisMove<C, FS, E, TS>::value,
					  "Condition must have operator()(From&, const Event&, To&) or operator()(From&, Event&&, To&)");

		static_assert(!(HasParenthesisConstRef<C, FS, E, TS>::value && HasParenthesisMove<C, FS, E, TS>::value),
					  "Condition must have only one of operator()(From&, const Event&, To&) or operator()(From&, Event&&, To&). Actually defined both.");

		using From = StateWrapper<FS>;
		using To = StateWrapper<TS>;
		using Event = E;
		using Condition = C;

		template<typename States>
		constexpr static size_t transit(States& states, Event&& e)
		{
			auto& from = std::get<From>(states);
			auto& to = std::get<To>(states);

			from.doOnExit(std::move(e));

			if(Condition{}(from, e, to))
			{
				to.doOnEnter(std::move(e));
				return TypeIndex<To, States>::value;
			}
			else
			{
				from.doOnEnter(std::move(e));
				return TypeIndex<From, States>::value;
			}
		}
	};
}

#endif // TRANSITION_H
