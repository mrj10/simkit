#include "callback_test.h"

template<typename PAYLOAD>
bool BlockACallbackFunctor<PAYLOAD>::operator()(PAYLOAD p) {
	return blockAPtr->foo(p);
}

int main() {
	BlockA ba;
	OutPort<int> *poke = new OutPort<int>();
	DummyCallbackFunctor<int> dummy;
	InPort<int> *peek = new InPort<int>(&dummy);
	poke->connect(ba.in);
	ba.out->connect(peek);
	poke->send(5);
	delete peek;
	delete poke;
	return 0;
}

