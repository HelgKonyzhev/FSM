#ifndef HELPERS_H
#define HELPERS_H

#include <type_traits>
#include <tuple>
#include <iostream>

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
		static const std::size_t value = 1 + TypeIndex<T, std::tuple<Ts...>>::value;
	};
}

#endif // HELPERS_H
