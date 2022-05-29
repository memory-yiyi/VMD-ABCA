#pragma once
#include"rentype.h"
#include"mvmd.h"
#include<random>

/* 有关可行解边界的参数 */
namespace FSpara
{
	const int Kmin = 2;
	const int Kmax = 10;
	const int Alphamin = 200;
	const int Alphamax = 3000;
};
/* 有关ASI的参数 */
namespace Spara
{
	const short num = 40;				// 蜜源的个数
	const short looker = num;			// 引领蜂的个数
	const short follow = 240;				// 跟随蜂的个数
	const short maxit = 60;				// 最大迭代次数
	const short limit = 40;				// 最大开采次数
};
/* 控制蜜源的相关参数 */
namespace Npara = Spara;

class VMD_ABCA
{
	/* 基础类型 */
private:
	/* 有关排列熵的参数 */
	struct PEpara
	{
		double* Series;			// 待求解序列
		int len;					// 序列总长度
		rentype::byte M;		// 嵌入维度
		int T;						// 采样间隔

		PEpara(double* S, int l)
		{
			Series = S;
			len = l;
			M = 5;
			T = 0;
		}
		PEpara(double* S, int l, rentype::byte m, int t)
		{
			Series = S;
			len = l;
			M = m;
			T = t;
		}
	};
protected:
	/* 可行解，蜜源 */
	typedef struct FeasibleSolution
	{
		int K;					// NumIMF
		int Alpha;			// PenaltyFactor

		FeasibleSolution() = default;
		FeasibleSolution(int k, int a)
		{
			K = k;
			Alpha = a;
		}
		FeasibleSolution&& operator*(double);
		FeasibleSolution&& operator+(const FeasibleSolution& other);
		FeasibleSolution&& operator-(const FeasibleSolution& other);

		/* 边界处理函数 */
		void BoundaryTreatment();
	} Nectar;
	/* ASI算法的个体，蜜蜂 */
	typedef struct Swarm
	{
		Nectar choice;			// 蜜蜂选择的蜜源
		double fitness;			// 蜜源的适应度
		short count;				// 蜜源的开采次数

		Swarm()
		{
			fitness = 0;
			count = 0;
		}
		bool operator>(const Swarm& comp);
		bool operator==(const Swarm& comp);
		Swarm& operator=(const Swarm& other);
		virtual ~Swarm() = default;
		Swarm(Swarm&) = delete;
		Swarm(Swarm&&) = delete;
		Swarm&& operator=(Swarm&&) = delete;
	} Bee;
	/* 有关适应度函数的参数 */
	struct Fpara :public PEpara
	{
		double* IMF;
		double entropy[FSpara::Kmax];
		mxArray* mX;
		mxArray* mK;
		mxArray* mAlpha;

		Fpara(double* problem, int prolen) :PEpara(problem, prolen)
		{
			IMF = new double[prolen];
			mwSize columnsize = 1;
			mX = mxCreateDoubleMatrix(1, prolen, mxREAL);
			mK = mxCreateNumericArray(1, &columnsize, mxINT32_CLASS, mxREAL);
			mAlpha = mxCreateNumericArray(1, &columnsize, mxINT32_CLASS, mxREAL);
			memcpy(mxGetDoubles(mX), problem, prolen * sizeof(double));
		}
		virtual ~Fpara();
		Fpara(Fpara&) = delete;
		Fpara& operator=(Fpara&) = delete;
		Fpara(Fpara&&) = delete;
		Fpara&& operator=(Fpara&&) = delete;

		/* 将蜜源转化为matlab支持的形式 */
		void TransformNecter(const Nectar* n);
	};

	/* 基础变量 */
private:
	Bee looker[Npara::looker];				// 引领蜂所处位置，局部最优解
	Bee follow;									// 跟随蜂
	Bee optimal;								// 全局最优解

	/* 基础函数 */
private:
	/* 排列熵 */
	double PE(const PEpara* para);
protected:
	/* 初始化蜜源 */
	void InitNectar(Bee& b, Fpara& para);
	/* 适应度函数 */
	double Fitness(Fpara& para, const int K);
	/* 更新全局最优解 */
	bool UpdateGlobal(Bee& comp1, Bee& comp2);
	/* 招募跟随蜂算子 */
	int Recruit(double* arr);
	/* 跟随蜂搜索蜜源算子 */
	void Search(Fpara& para, int rr);
	/* 更新局部最优解 */
	bool UpdateLocal(Bee& comp1, Bee& comp2);
	/* 放弃蜜源算子 */
	void Abandon(Bee& b, Fpara& para);
public:
	/* 核心算法 */
	void Solution(const double* problem, const int prolen);
	/* 打印当前全局最优解 */
	void PrintOpt(int iteration)const;

	/* 特殊成员函数 */
};

