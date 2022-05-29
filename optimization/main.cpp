#include"VMD_ABCA.h"
#include"VMDRunFlag.h"
#include<iostream>
using std::cout;
using std::endl;
std::random_device rd;
std::mt19937 gen(rd());
std::normal_distribution<> randn(0, 1);

const double pi = acos(-1);
struct Signal
{
	double* Series;
	int len;
	static const double pi;

	Signal(double ts,double te,int fs)
	{
		len = (te - ts) * fs;
		Series = new double[len];
		for (int i = 0; i < len; i++)
		{
			Series[i] = ts + i / (double)fs;
			Series[i] = 1.6 * cos(2 * pi * 5 * Series[i]) + 1.2 * cos(2 * pi * 50 * Series[i]) + 1.4 * cos(2 * pi * 200 * Series[i]) + 0.01 * randn(gen);
		}
	}
	virtual ~Signal();
};
const double Signal::pi = acos(-1);
Signal::~Signal()
{
	delete[] Series;
}


int main()
{
	Signal S(0, 1, 500);
	VMD_ABCA Ans;
	VMDRunFlag::VMDInit();
	Ans.Solution(S.Series, S.len);
	VMDRunFlag::VMDTerminate();
}
