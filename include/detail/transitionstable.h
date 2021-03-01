#ifndef TRANSITIONSTABLE_H
#define TRANSITIONSTABLE_H

#include <stdexcept>
#include "transition.h"

namespace FSM
{
	namespace Detail
	{
		template<typename T, typename... Ts>
		struct StatesExtractorImpl
		{
			using Type = T;
		};


		template<template<typename...> typename Tuple, typename... Ts, typename U, typename... Us>
		struct StatesExtractorImpl<Tuple<Ts...>, U, Us...> : StatesExtractorImpl<Tuple<Ts..., typename U::From, typename U::To>, Us...> {};


		template<typename... Transitions>
		struct StatesExtractor : StatesExtractorImpl<std::tuple<>, Transitions...> {};


		template<typename... Ts>
		using StatesTuple = typename StatesExtractor<Ts...>::Type;


		struct TransitionStub
		{
			using From = struct{};
			using To = struct{};

			template<typename States, typename Event>
			static size_t transit(States&, Event&&)
			{
				throw std::logic_error("Unimplemented transition");
			}
		};


		template<typename State, typename Event, typename Tuple, typename... Ts>
		struct GetTransitionImpl
		{
			using Type = std::tuple_element_t<0, Tuple>;
		};


		template<typename State, typename Event, template<typename...> typename Tuple, typename... Ts, typename Transition, typename... Transitions>
		struct GetTransitionImpl<State, Event, Tuple<Ts...>, Transition, Transitions...>
				: std::conditional_t<(std::is_same_v<State, typename Transition::From> && std::is_same_v<Event, typename Transition::Event>),
									 GetTransitionImpl<State, Event, Tuple<Ts..., Transition>, Transitions...>,
									 std::conditional_t<(sizeof... (Transitions) == 0 && sizeof... (Ts) == 0),
														GetTransitionImpl<State, Event, Tuple<Ts..., TransitionStub>, Transitions...>,
														GetTransitionImpl<State, Event, Tuple<Ts...>, Transitions...>
													   >
									>
		{};


		template<typename State, typename Event, typename Transitions>
		struct GetTransition;


		template<typename State, typename Event, template<typename...> typename Tuple, typename... Transitions>
		struct GetTransition<State, Event, Tuple<Transitions...>> : GetTransitionImpl<State, Event, std::tuple<>, Transitions...> {};


		template<typename StateMachineDef, typename... Ts>
		class TransitionTable
		{
		public:
			using InititalState = StateWrapper<typename StateMachineDef::InitialState>;
			using States = typename Detail::ReplaceTypeFront<InititalState, typename Detail::UniqueTypes<Detail::StatesTuple<Ts...>>::Type>::Type;
			using Transitions = std::tuple<Ts...>;

			template <typename Event, typename Indices = std::make_index_sequence<sizeof...(Ts)>>
			static constexpr size_t process(States& states, const size_t idx, Event&& e)
			{
				return processImpl(states, idx, e, Indices {});
			}

		private:
			template<size_t Idx, typename Event>
			static void tryTransit(States& states, size_t idx, Event&& e, size_t& nextIdx)
			{
				if(Idx != idx)
					return;
				using Tr = typename Detail::GetTransition<std::tuple_element_t<Idx, States>, Event, Transitions>::Type;
				nextIdx = Tr::transit(states, std::move(e));
			}

			template <typename Event, std::size_t... I>
			static constexpr size_t processImpl(States& states, const size_t idx, Event&& e, std::index_sequence<I...>)
			{
				size_t nextIdx = 0;
				((tryTransit<I>(states, idx, std::move(e), nextIdx)), ...);
				return nextIdx;
			}
		};
	}
}

#endif // TRANSITIONSTABLE_H
