#include <iostream>
using namespace std;
//抽象工行模式
class SingleCore {
	public:
		virtual void Show()	=	0;
};
class SingleCoreA {
	pubic:
		void Show()	{	std::cout<<"SingleCoreA"<<std::endl;	}
};
class SingleCoreB {
	pubic:
		void Show()	{	std::cout<<"SingleCoreB"<<std::endl;	}
};

class MultiCore {
	public:
		virtual void Show()	= 0;
};
class MultiCoreA : public MultiCore {
	public:
		void Show()	{	std::cout<<"MultiCoreA"<<std::endl;	}
};
class MultiCoreB : public MultiCore {
	public:
		void Show()	{	std::cout<<"MultiCoreB"<<std::endl;	}
};

class CoreFactory {
	public:
		virtual SingleCore*	CreateSingleCore()	=	0;
		virtual MultiCore*	CreateMultiCore()	=	0;
};
class FactoryA : public CoreFactory {
	public:
		SingleCore*	CreateSingleCore()	{	return new SingleCoreA;	}
		MultiCore*	CreateMultiCore()	{	return new MultiCoreA;	}
};
class FactoryB : public CoreFactory {
	public:
		SingleCore*	CreateSingleCore()	{	return new SingleCoreB;	}
		MultiCore*	CreateMultiCore()	{	return new MultiCoreB;	}
};
