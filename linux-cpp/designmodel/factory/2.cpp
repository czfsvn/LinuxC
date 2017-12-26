#include <iostream>
using namespace std;
// 工厂方法模式
class SingleCore {
	public:
		virtual void Show()	=	0;
};
class SingleCoreA : public SingleCore {
	pubic:
		void Show()	{	std::cout<<"SingleCorea A"<<std::endl;	}
};
class SingleCoreB : public SingleCore {
	pubic:
		void Show()	{	std::cout<<"SingleCorea B"<<std::endl;	}
};

class Factory {
	public:
		virtual	SingleCore*		CreateSingleCore()	=	0;	
};
class FactoryA : public Factory{
	public:
		SingleCoreA* CreateSingleCore()	{	return new SingleCoreA;	}
};
class FactoryB : public Factory{
	public:
		SingleCoreB* CreateSingleCore()	{	return new SingleCoreB;	}
};
