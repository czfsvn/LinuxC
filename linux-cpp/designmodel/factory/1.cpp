#include <iostream>
using namespace std;

//简单工行模式

enum cType {
	COREA, COREB,
};
class SingleCore {
	public:
	 virtual void Show()	=	0;
};
class SingleCoreA : public SingleCore {
	public:
	virtual void Show()	{	std::cout<<"SingleCoreA"<<std::endl;	}	
};
class SingleCoreB : public SingleCore	{
	public:
	virtual void Show() {	std::cout<<"SingleCoreB"<<std::endl;	};
};
class Factory {
	public:
	SingleCore* CreateSingleCore(enum cType type) {
		if (type == COREA)
				return new SingleCoreA();
		else if (type == COREA)
				return new SingleCoreA();
		else
				return NULL;
	}
};

