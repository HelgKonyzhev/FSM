#include <iostream>
#include <fsm.h>

struct Event1 {};
struct Event2 {};
struct Event3 {};


class State1
{
public:
	void onEnter(Event1&&)
	{
		std::cout << "State1::onEnter(Event1&&)" << std::endl;
	}

	void onEnter(const Event2&)
	{
		std::cout << "State1::onEnter(const Event2&)" << std::endl;
	}

	void onExit(const Event1&)
	{
		std::cout << "State1::onExit(const Event1&)" << std::endl;
	}

private:
	std::string m_name = "State1";
};


class State2
{
public:
	void onEnter(Event2&&)
	{
		std::cout << "State2::onEnter(Event2&&)" << std::endl;
	}

	void onEnter(Event1&&)
	{
		std::cout << "State2::onEnter(Event1&&)" << std::endl;
	}

	void onExit(const Event1&)
	{
		std::cout << "State2::onExit(Event1&)" << std::endl;
	}

	void onExit(const Event2&)
	{
		std::cout << "State2::onExit(Event2&)" << std::endl;
	}
};


class State3
{
public:
	void onEnter(Event2&&)
	{
		std::cout << "State3::onEnter(Event2&&)" << std::endl;
	}

	void onEnter(Event1&&)
	{
		std::cout << "State3::onEnter(Event1&&)" << std::endl;
	}

	void onExit(const Event1&)
	{
		std::cout << "State3::onExit(Event1&)" << std::endl;
	}

	void onExit(const Event2&)
	{
		std::cout << "State3::onExit(Event2&)" << std::endl;
	}
};


struct Condition1
{
	bool operator()(State2&, const Event1&, State2&)
	{
		std::cout << "Condition1::operator()(State2&, const Event1&, State2&)->true" << std::endl;
		return true;
	}
};

struct Condition2
{
	bool operator()(State2&, const Event2&, State1&)
	{
		std::cout << "Condition2::operator()(State2&, const Event2&, State1&)->true" << std::endl;
		return true;
	}
};

struct Condition3
{
	bool operator()(State1&, Event1&&, State3&)
	{
		std::cout << "Condition3::operator()(State1&, Event1&&, State3&)->true" << std::endl;
		return true;
	}
};


struct Condition4
{
	bool operator()(State3&, Event2&&, State2&)
	{
		std::cout << "Condition4::operator()(State3&, Event2&&, State2&)->false" << std::endl;
		return false;
	}
};


struct Condition5
{
	bool operator()(State3&, Event1&&, State2&)
	{
		std::cout << "Condition5::operator()(State3&, Event1&&, State2&)->true" << std::endl;
		return true;
	}
};


class CommonStateMachineDef : public FSM::StateMachineDef<CommonStateMachineDef>
{
public:
	using InitialState = State2;

	using TransitionTable = TransitionTable<
		  Tr<State2, Event1, State2, Condition1>
		, Tr<State2, Event2, State1, Condition2>
		, Tr<State1, Event1, State3, Condition3>
		, Tr<State3, Event2, State2, Condition4>
		, Tr<State3, Event1, State2, Condition5>
	>;
};

using CommonStateMachine = FSM::StateMachine<CommonStateMachineDef>;


int main()
{
	CommonStateMachine fsm;

	fsm.process(Event1{});

	std::cout << std::endl;
	fsm.process(Event2{});

	std::cout << std::endl;
	fsm.process(Event1{});

	std::cout << std::endl;
	fsm.process(Event2{});

	std::cout << std::endl;
	fsm.process(Event1{});

	return 0;
}
