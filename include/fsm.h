#ifndef FSM_H
#define FSM_H

#include <detail/transitionstable.h>

namespace FSM
{
	template<typename T>
	class StateMachineDef
	{
	public:
		using StateMachineType = T;

		template<typename... Ts>
		using TransitionTable = typename Detail::TransitionTable<T, Ts...>;

		template<typename FS, typename E, typename TS, typename C>
		using Tr = FSM::Detail::Transition<FS, E, TS, C>;
	};


	template<typename Def>
	class StateMachine : public Def::StateMachineType
	{
	public:
		using TransitionTable = typename Def::TransitionTable;
		using InitialState = typename Detail::StateWrapper<typename Def::InitialState>;
		using States = typename TransitionTable::States;

		using Def::StateMachineType::StateMachineType;

		template<typename Event>
		void process(Event&& e)
		{
			m_currentStateIdx = TransitionTable::process(m_states, m_currentStateIdx, e);
		}

	private:
		States m_states;
		size_t m_currentStateIdx = 0;
	};
}

#endif // FSM_H
