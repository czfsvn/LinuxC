#include <iostream>
using namespace std;

class Deque {
	public:
		void push_back(int idx)		{	std::cout<<"Deque push_back"<<std::endl;	}
		void push_front(int idx)		{	std::cout<<"Deque push_front"<<std::endl;	}
		void pop_back()		{	std::cout<<"Deque pop_back"<<std::endl;	}
		void pop_front()		{	std::cout<<"Deque pop_front"<<std::endl;	}
};
class Sequence {
	public:
		virtual void push(int x)	= 0;
		virtual void pop()	=	0;
};
class Stack : public Sequence {
	public:
		void push(int x)	{	deque.push_back(x);	}
		void pop()		{	deque.pop_back();	}
	private:
		Deque deque;
};
class Queue: public Sequence {
	public:
		void push(int x)	{	deque.push_back(x);	}
		void pop()		{	deque.pop_front();	}
	private:
		Deque deque;
};

int main()
{
	Sequence* s1 = new Stack();
	Sequence* s2 = new Queue();
	s1->push(1);
	s1->pop();
	s2->push(2);
	s2->pop();
	delete s1;
	delete s2;
	return 0;
}
