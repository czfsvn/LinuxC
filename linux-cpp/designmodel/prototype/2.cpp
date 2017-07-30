#include <iostream>
using namespace std;
//模板方法模式
class Resume {
	protected:
		virtual void SetPersonalInfo()	{};
		virtual void SetEducation()	{}
		virtual void SetWorkExp()	{}
	public:
		void FillResume() {
			SetPersonalInfo();
			SetEducation();
			SetWorkExp();
		}
};
class ResumeA : public Resume {
	protected:
		void SetPersonalInfo()	{	std::cout<<"A's PersonInfo"<<std::endl;	};
		void SetEducation()	{   std::cout<<"A's Education"<<std::endl;}
		void SetWorkExp()	{   std::cout<<"A's Work Experience"<<std::endl;}
};
class ResumeB : public Resume {
	protected:
		void SetPersonalInfo()	{	std::cout<<"B's PersonInfo"<<std::endl;	};
		void SetEducation()	{   std::cout<<"B's Education"<<std::endl;}
		void SetWorkExp()	{   std::cout<<"B's Work Experience"<<std::endl;}
};
int main()
{
	Resume* r1 = new ResumeA;
	r1->FillResume();
	delete r1;
	r1 = new ResumeB;
	r1->FillResume();
	delete r1;
	r1 = NULL;
	return 0;
}

