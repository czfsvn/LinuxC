#include <iostream>
using namespace std;

class ReplaceAlgorithm {
	public:
		virtual void Replace()	=	0;
};
class LRU_ReplaceAlgorithm : public ReplaceAlgorithm {
	public:
		void Replace()	{	std::cout<<"LRU_ReplaceAlgorithm"<<std::endl;	}
};
class FIFO_ReplaceAlgorithm : public ReplaceAlgorithm {
	public:
		void Replace()	{	std::cout<<"FIFO_ReplaceAlgorithm"<<std::endl;	}
};
class Random_ReplaceAlgorithm : public ReplaceAlgorithm {
	public:
		void Replace()	{	std::cout<<"Random_ReplaceAlgorithm"<<std::endl;	}
};

class Cache1 {
	public:
		Cache1(ReplaceAlgorithm* ra)	{	m_ra = ra;	}
		~Cache1()						{	delete m_ra;}
		void Replace()					{	m_ra->Replace();	}
	private:
		ReplaceAlgorithm* m_ra;
};
class Cache2 {
	public:
		Cache2(int idx)	{
			if (idx == 1)		m_ra = new LRU_ReplaceAlgorithm;
			else if (idx == 2)	m_ra = new FIFO_ReplaceAlgorithm;
			else if (idx == 3)	m_ra = new Random_ReplaceAlgorithm;
			else				m_ra = NULL;
		}
		~Cache2()		{	delete m_ra;	}
		void Replace()					{	m_ra->Replace();	}
	private:
		ReplaceAlgorithm* m_ra;
};
template <class RA> class Cache3 {
	private:
		RA m_ra;
	public:
		Cache3()	{}
		~Cache3()	{}
		void Replace()	{	m_ra.Replace();	}
};

int main()	{
	Cache3<Random_ReplaceAlgorithm> cache;
	cache.Replace();
	return 0;
}
