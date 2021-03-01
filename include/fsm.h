#ifndef FSM_H
#define FSM_H

#include <detail/transitionstable.h>

namespace FSM
{
	template<typename T>
	class StateMachineDef
	{
	public:
		using Base = T;

		template<typename... Ts>
		using TransitionTable = typename Detail::TransitionTable<T, Ts...>;

		template<typename FS, typename E, typename TS, typename C>
		using Tr = FSM::Detail::Transition<FS, E, TS, C>;
	};


	template<typename Def>
	class StateMachine : public Def::Base
	{
	public:
		using TransitionTable = typename Def::TransitionTable;
		using InitialState = typename Detail::StateWrapper<typename Def::InitialState>;
		using States = typename TransitionTable::States;

		using Def::Base::Base;

		template<typename Event>
		void process(Event&& e)
		{
			std::cout << "enter state idx: " << m_currentStateIdx << std::endl;
			m_currentStateIdx = TransitionTable::process(m_states, m_currentStateIdx, e);
			std::cout << "exit state idx: " << m_currentStateIdx << std::endl;
		}

	private:
		States m_states;
		size_t m_currentStateIdx = 0;
	};
}

#endif // FSM_H
