#include <iostream>
#include <list>
using namespace std;
//组合模式
class Company {
	public:
		Company(std::string name)	{	m_name = name;	}
		virtual ~Company()	{}
		virtual void Add(Company* pCom)		{}
		virtual void Show(int depth)	{}
	protected:
		std::string m_name;
};
class ConcreteCompany : public Company {
	public:
		ConcreteCompany(std::string name) : Company(name)	{}
		virtual ~ConcreteCompany()	{}
		virtual void Add(Company* pCom)		{	m_list.push_back(pCom);	}
		void Show(int depth)  {
			for (int i = 0; i < depth; i++) 
				std::cout<<"-";
			std::cout<<m_name<<std::endl;
			for (auto comp : m_list) 
				comp->Show(depth + 2);
		}
	private:
		std::list<Company*>	m_list;
};
class FinanceDepartment : public Company {
	public:
		FinanceDepartment(std::string name) : Company(name)	{}
		virtual ~FinanceDepartment()	{}
		virtual void Show(int depth) {
			for(int i = 0; i < depth; i++)  
					cout<<"-";  
			cout<<m_name<<endl;
		}
};
class HRDepartment : public Company {
	public:
		HRDepartment(std::string name) : Company(name)	{}
		virtual ~HRDepartment()	{}
		virtual void Show(int depth) {
			for(int i = 0; i < depth; i++)  
					cout<<"-";  
			cout<<m_name<<endl;
		}
};

int main()
{
	Company *root = new ConcreteCompany("总公司");
	{
		Company *leaf1 = new FinanceDepartment("财务部");
		Company *leaf2 = new HRDepartment("人力资源部");
		root->Add(leaf1);
		root->Add(leaf2);
	}

	{
		//分公司A
		Company *mid1 = new ConcreteCompany("分公司A");
		Company *leaf3 = new FinanceDepartment("财务部");
		Company *leaf4 = new HRDepartment("人力资源部");
		mid1->Add(leaf3);
		mid1->Add(leaf4);
		root->Add(mid1);
	}

	{
		//分公司B
		Company *mid2 = new ConcreteCompany("分公司B");
		FinanceDepartment *leaf5 = new FinanceDepartment("财务部");
		HRDepartment *leaf6 = new HRDepartment("人力资源部");
		mid2->Add(leaf5);
		mid2->Add(leaf6);
		root->Add(mid2);
	}

	root->Show(0);

	Company *mid3 = new ConcreteCompany("分公司C");
}

