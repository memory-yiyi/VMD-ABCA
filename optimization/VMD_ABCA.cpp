#include"VMD_ABCA.h"
#include"mathexpand.h"
#include<cmath>
#include<iostream>
using namespace math;
namespace rt = rentype;
using std::cout;
using std::endl;

/* 承载用于生成随机数的对象 */
namespace Random
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> randK(FSpara::Kmin, FSpara::Kmax);							// 随机数K
	std::uniform_int_distribution<> randAlpha(FSpara::Alphamin, FSpara::Alphamax);		// 随机数Alpha
	std::uniform_int_distribution<> randt(0, Npara::num - 1);											// 随机数，指定非自身蜜源
	double rand = 0.;													// 随机数(引子)，用于跟随蜂搜索蜜源算子
	std::uniform_real_distribution<> randr(-1., 1.);				// 随机数，用于跟随蜂搜索蜜源算子
	std::uniform_real_distribution<> randroulette(0., 1.);		// 随机数，用于轮盘赌
}
namespace rd = Random;

/* 有关优化的参数 */
namespace Opara
{
	bool GlobalFlag = false;
	bool LocalFlag = false;
	const short limit = Npara::maxit / 100;			// 连续无优化迭代的最大次数
	short dynlimit = limit;									// 连续无优化迭代的最大次数(动态)
	short time = 0;											// 当前无优化迭代的次数
}


VMD_ABCA::PEpara::~PEpara()
{
	delete[] IMF;
}

VMD_ABCA::FeasibleSolution&& VMD_ABCA::FeasibleSolution::operator*(double)
{
	return FeasibleSolution(K * rd::randr(rd::gen), Alpha * rd::randr(rd::gen));
}

VMD_ABCA::FeasibleSolution&& VMD_ABCA::FeasibleSolution::operator+(const FeasibleSolution& other)
{
	return FeasibleSolution(K + other.K, Alpha + other.Alpha);
}

VMD_ABCA::FeasibleSolution&& VMD_ABCA::FeasibleSolution::operator-(const FeasibleSolution& other)
{
	return FeasibleSolution(K - other.K, Alpha - other.Alpha);
}

inline void VMD_ABCA::FeasibleSolution::BoundaryTreatment()
{
	K = min(K, FSpara::Kmax);
	K = max(K, FSpara::Kmin);
	Alpha = min(Alpha, FSpara::Alphamax);
	Alpha = max(Alpha, FSpara::Alphamin);
}

bool VMD_ABCA::Swarm::operator>(const Swarm& comp)
{
	if (this->fitness > comp.fitness)
		return true;
	else
		return false;
}

bool VMD_ABCA::Swarm::operator==(const Swarm& comp)
{
	if (this->fitness == comp.fitness)
		return true;
	else
		return false;
}

VMD_ABCA::Swarm& VMD_ABCA::Swarm::operator=(const Swarm& other)
{
	choice = other.choice;
	fitness = other.fitness;
	count = 0;
	return *this;
}

VMD_ABCA::Fpara::~Fpara()
{
	mxDestroyArray(mAlpha);
	mxDestroyArray(mK);
	mxDestroyArray(mX);
}

inline void VMD_ABCA::Fpara::TransformNecter(const Nectar* n)
{
	memcpy(mxGetInt32s(mK), &n->K, sizeof(int));
	memcpy(mxGetInt32s(mAlpha), &n->Alpha, sizeof(int));
}


double VMD_ABCA::PE(const PEpara* para)
{
	/*
	* n					-> num
	* rest				-> restruction
	* c					-> component
	* index/in		-> index
	* r					-> relative
	* loca				-> location
	* s					-> sum
	* data				-> data
	* p				-> probability
	*/

	int i, j, k, m;													// 循环变量
	int t = para->T + 1;											// 采样增量
	int nrest = para->len - (para->M - 1) * t;				// 重构分量的数量
	double* crest = new double[para->M];					// 单个重构分量
	int nindex = factorial(para->M);							// 索引分量的数量
	rt::byte* cindex = new rt::byte[para->M];						// 单个索引分量
	rt::byte* rloca = new rt::byte[para->M];						// 索引分量中各元素的相对位置
	int loca;															// 索引分量在数组中的位置
	int* index = new int[nindex] {0};							// 索引分量的数组
	int& sdata = nrest;											// 数据总量
	double pcin;													// 索引分量出现的概率
	double ans = 0;												// 计算结果

	for (i = 0; i < nrest; i++)
	{
		/* 初始化 */
		loca = 0;
		for (j = i, k = 0; k < para->M; j += t, k++)
		{
			crest[k] = para->IMF[j];
			cindex[k] = rloca[k] = k;
		}
		/* 冒泡排序 */
		for (j = para->M - 1; j > 0; j--)
			for (k = 0; k < j; k++)
				if (crest[k] > crest[k + 1])
				{
					crest[k] += crest[k + 1];
					crest[k + 1] = crest[k] - crest[k + 1];
					crest[k] -= crest[k + 1];
					cindex[k] ^= cindex[k + 1];
					cindex[k + 1] ^= cindex[k];
					cindex[k] ^= cindex[k + 1];
				}
		/* 选择索引 */
		for (j = para->M - 1; j > 0; j--)
		{
			loca += rloca[cindex[j]] * factorial(j);
			rloca[cindex[j]] = -1;
			for (k = 0, m = 0; k < para->M; k++)
				if (rloca[k] >= 0)
					rloca[k] = m++;
		}
		index[loca]++;
	} // for (i = 0; i < nrest; i++)
	/* 计算熵 */
	for (j = 0; j < nindex; j++)
		if (index[j])
		{
			pcin = index[j] / (double)sdata;
			ans += pcin * log(pcin);
		}

	delete[] crest;
	delete[] cindex;
	delete[] rloca;
	delete[] index;
	return -ans;
}

inline void VMD_ABCA::InitNectar(Bee& b, Fpara& para)
{
	b.choice.K = rd::randK(rd::gen);
	b.choice.Alpha = rd::randAlpha(rd::gen);
	para.TransformNecter(&b.choice);
	b.fitness = Fitness(para, b.choice.K);
	UpdateGlobal(b, optimal);
}

inline double VMD_ABCA::Fitness(Fpara& para, const int K)
{
	mxArray* mIMF = mxCreateDoubleMatrix(para.len, K, mxREAL);
	mlfMvmd(1, &mIMF, para.mX, para.mK, para.mAlpha);
	for (int i = 0; i < K; i++)
	{
		memcpy(para.IMF, mxGetDoubles(mIMF) + para.len * i, para.len * sizeof(double));
		para.entropy[i] = PE(&para);
	}
	mxDestroyArray(mIMF);
	return var<double>(para.entropy, K) / mean<double>(para.entropy, K);
}

inline void VMD_ABCA::UpdateGlobal(Bee& comp1, Bee& comp2)
{
	if (comp1 > comp2 || (comp1 == comp2 && comp1.choice.K > comp2.choice.K))
	{
		comp2 = comp1;
		Opara::GlobalFlag = true;
	}
}

inline int VMD_ABCA::Recruit(double* arr)
{
	/*
	* 在引领蜂数量为 40 时，此函数在循环第 21 次之后，地址所对应的值不对
	* 猜测是因为内存不连续，但编译器知道怎么使它们看起来是连续的
	* double fitnessSum = sum<double, sizeof(VMD_ABCA)>(&looker[0].fitness, Npara::looker);
	*/
	if (!Opara::LocalFlag)
		goto next;
	static double fitnessSum;
	fitnessSum = 0.;
	for (int i = 0; i < Npara::looker; i++)
		fitnessSum += looker[i].fitness;
	arr[0] = looker[0].fitness / fitnessSum;
	for (int i = 1; i < Npara::looker; i++)
	{
		arr[i] = looker[i].fitness / fitnessSum;
		arr[i] += arr[i - 1];
	}
	next:
	double rand = rd::randroulette(rd::gen);
	return GetAddressOfArray<double>(arr, std::find_if(arr, arr + Npara::looker, [rand](double x)->bool {return x >= rand; }));
}

inline void VMD_ABCA::Search(Fpara& para, int rr)
{
	/*
	* rr	-> result roulette
	*/
	int t = rd::randt(rd::gen);
	while (t == rr)
		t = rd::randt(rd::gen);
	follow.choice = looker[rr].choice + (looker[rr].choice - looker[t].choice) * rd::rand;
	follow.choice.BoundaryTreatment();
	para.TransformNecter(&follow.choice);
	follow.fitness = Fitness(para, follow.choice.K);
	UpdateLocal(follow, looker[rr]);
}

inline void VMD_ABCA::UpdateLocal(Bee& comp1, Bee& comp2)
{
	if (comp1 > comp2 || (comp1 == comp2 && comp1.choice.K > comp2.choice.K))
	{
		comp2 = comp1;
		UpdateGlobal(comp2, optimal);
		Opara::LocalFlag = true;
	}
	else
	{
		comp2.count++;
		Opara::LocalFlag = false;
	}
}

inline void VMD_ABCA::Abandon(Bee& b, Fpara& para)
{
	if (b.count >= Npara::limit)
	{
		b.count = 0;
		InitNectar(b, para);
		UpdateGlobal(b, optimal);
	}
}

inline void VMD_ABCA::PrintAnsOfThisIter(int iteration)const
{
	printf("%02d:    %.7f\t[%2d %4d]\t%ld\n", iteration, optimal.fitness, optimal.choice.K, optimal.choice.Alpha, clock() / CLOCKS_PER_SEC);
}

inline void VMD_ABCA::PrintAns(Bee& b)
{
	cout << "K = " << b.choice.K << endl;
	cout << "Alpha = " << b.choice.Alpha << endl;
	cout << "fitness = " << b.fitness << endl;
}

void VMD_ABCA::Solution(const double* problem, const int prolen)
{
	int i, j, k;											// 循环变量
	double roulette[Npara::looker];			// 轮盘
	int resultroulette;								// 轮盘赌结果
	Fpara Fpa((double*)problem, prolen);

	/* 初始化蜜源 */
	for (i = 0; i < Npara::num; i++)
		InitNectar(looker[i], Fpa);
	Opara::GlobalFlag = false;
	Opara::LocalFlag = true;

	/* 种群迭代 */
#if ShowDetailOfIteration
	cout << "details:" << endl;
#endif // ShowDetailOfIteration
	for (i = 0; i < Npara::maxit; i++)
	{
		Opara::GlobalFlag = false;
		for (j = 0; j < Npara::follow; j++)
		{
			resultroulette = Recruit(roulette);
			Search(Fpa, resultroulette);
			Abandon(looker[resultroulette], Fpa);
		} // for (j = 0; j < Npara::follow; j++)
#if ShowDetailOfIteration
		PrintAnsOfThisIter(i);
#endif // ShowDetailOfIteration
		if (!(i % (Npara::maxit / 10)) && i && Opara::dynlimit > 2)
			Opara::dynlimit--;
		if (Opara::GlobalFlag)
			Opara::time = 0;
		else
			if (++Opara::time >= Opara::dynlimit)
				break;
	} // for (i = 0; i < Npara::maxit; i++)
	cout << "the answer is:" << endl;
	PrintAns(optimal);
}

void VMD_ABCA::ThreeToBest(const double* problem, const int prolen)
{
	Bee best;
	for (rt::byte i = 0; i < 3; i++)
	{
		/* 运行计算 */
		Solution(problem, prolen);
		UpdateGlobal(optimal, best);
		/* 初始化变量 */
		Opara::dynlimit = Opara::limit;
		Opara::time = 0;
		for (rt::byte j = 0; j < Npara::looker; j++)
			looker[j] = Bee();
		follow = Bee();
		optimal = Bee();
	}
	cout << "the best is:" << endl;
	PrintAns(best);
}
