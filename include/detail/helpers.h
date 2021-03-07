#ifndef HELPERS_H
#define HELPERS_H

#include <type_traits>
#include <tuple>

namespace FSM
{
	namespace Detail
	{
		template <typename T, typename... Ts>
		struct UniqueTypesImpl
		{
			using Type = T;
		};


		template<template<typename...> typename Tuple, typename... Ts, typename U, typename... Us>
		struct UniqueTypesImpl<Tuple<Ts...>, U, Us...>
				: std::conditional_t<(std::is_same_v<U, Ts> || ...)
									  , UniqueTypesImpl<Tuple<Ts...>, Us...>
									  , UniqueTypesImpl<Tuple<Ts..., U>, Us...>
									> {};


		template<typename Tuple>
		struct UniqueTypes;


		template<template<typename...> typename Tuple, typename... Ts>
		struct UniqueTypes<Tuple<Ts...>> : public UniqueTypesImpl<std::tuple<>, Ts...> {};


		template <typename T, typename Tuple>
		struct TypeIndex;


		template <typename T, typename... Ts>
		struct TypeIndex<T, std::tuple<T, Ts...>>
		{
			static const std::size_t value = 0;
		};

		template <typename T, typename U, typename... Ts>
		struct TypeIndex<T, std::tuple<U, Ts...>>
		{
			static constexpr std::size_t value = 1 + TypeIndex<T, std::tuple<Ts...>>::value;
		};


		template<typename FrontType, typename Tuple, typename... Ts>
		struct ReplaceTypeFrontImpl
		{
			using Type = Tuple;
		};


		template<typename FrontType, template<typename...> typename Tuple, typename... Ts, typename Type, typename... Types>
		struct ReplaceTypeFrontImpl<FrontType, Tuple<Ts...>, Type, Types...>
				: std::conditional_t<std::is_same_v<FrontType, Type>
									 , ReplaceTypeFrontImpl<FrontType, Tuple<FrontType, Ts...>, Types...>
									 , ReplaceTypeFrontImpl<FrontType, Tuple<Ts..., Type>, Types...>
									>
		{};


		template<typename T, typename Tuple>
		struct ReplaceTypeFront;


		template<typename T, template<typename...> typename Tuple, typename... Ts>
		struct ReplaceTypeFront<T, Tuple<Ts...>> : ReplaceTypeFrontImpl<T, std::tuple<>, Ts...> {};
	}
}

#endif // HELPERS_H
