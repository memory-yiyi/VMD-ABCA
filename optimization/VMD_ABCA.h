#pragma once
#include"rentype.h"
#include"mvmd.h"
#include<random>

#ifndef ShowDetailOfIteration
#define ShowDetailOfIteration 1
#endif // !ShowDetailOfIteration

/* �йؿ��н�߽�Ĳ��� */
namespace FSpara
{
	const int Kmin = 2;
	const int Kmax = 10;
	const int Alphamin = 200;
	const int Alphamax = 3000;
};
/* �й�ASI�Ĳ��� */
namespace Spara
{
	const short num = 5;															// ��Դ�ĸ������ֲ����Ž��������Ҳ��ζ�ſռ����ĵĶ���
	const short looker = num;													// �����ĸ���
	const short follow = 32;														// �����ĸ�������ζ�Ÿ���ȫ�����Ž��ٶȵĿ���
	const short maxit = (FSpara::Kmax - FSpara::Kmin) *
		(FSpara::Alphamax - FSpara::Alphamin) / follow;				// ����������
	const short limit = 16;															// ��󿪲ɴ���
};
/* ������Դ����ز��� */
namespace Npara = Spara;

class VMD_ABCA
{
	/* �������� */
private:
	/* �й������صĲ��� */
	struct PEpara
	{
		double* IMF;			// ���������
		int len;					// �����ܳ���
		rentype::byte M;		// Ƕ��ά��
		int T;						// �������

		PEpara(int prolen)
		{
			IMF = new double[prolen];
			len = prolen;
			M = 5;
			T = 0;
		}
		PEpara(int prolen, rentype::byte m, int t)
		{
			IMF = new double[prolen];
			len = prolen;
			M = m;
			T = t;
		}
		virtual ~PEpara();
		PEpara(PEpara&) = delete;
		PEpara& operator=(PEpara&) = delete;
		PEpara(PEpara&&) = delete;
		PEpara&& operator=(PEpara&&) = delete;
	};
protected:
	/* ���н⣬��Դ */
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

		/* �߽紦���� */
		void BoundaryTreatment();
	} Nectar;
	/* ASI�㷨�ĸ��壬�۷� */
	typedef struct Swarm
	{
		Nectar choice;			// �۷�ѡ�����Դ
		double fitness;			// ��Դ����Ӧ��
		short count;				// ��Դ�Ŀ��ɴ���

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
		Swarm& operator=(Swarm&&) = default;
	} Bee;
	/* �й���Ӧ�Ⱥ����Ĳ��� */
	struct Fpara :public PEpara
	{
		double entropy[FSpara::Kmax];
		mxArray* mX;
		mxArray* mK;
		mxArray* mAlpha;

		Fpara(double* problem, int prolen) :PEpara(prolen)
		{
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

		/* ����Դת��Ϊmatlab֧�ֵ���ʽ */
		void TransformNecter(const Nectar* n);
	};

	/* �������� */
private:
	Bee looker[Npara::looker];				// ���������λ�ã��ֲ����Ž�
	Bee follow;									// �����
	Bee optimal;								// ȫ�����Ž�

	/* �������� */
private:
	/* ������ */
	double PE(const PEpara* para);
protected:
	/* ��ʼ����Դ */
	void InitNectar(Bee& b, Fpara& para);
	/* ��Ӧ�Ⱥ��� */
	double Fitness(Fpara& para, const int K);
	/* ����ȫ�����Ž� */
	void UpdateGlobal(Bee& comp1, Bee& comp2);
	/* ��ļ��������� */
	int Recruit(double* arr);
	/* �����������Դ���� */
	void Search(Fpara& para, int rr);
	/* ���¾ֲ����Ž� */
	void UpdateLocal(Bee& comp1, Bee& comp2);
	/* ������Դ���� */
	void Abandon(Bee& b, Fpara& para);
	/* ��ӡ��ǰȫ�����Ž� */
	void PrintAnsOfThisIter(int iteration)const;
	/* ��ӡ������ */
	void PrintAns(Bee& b);
public:
	/* �����㷨 */
	void Solution(const double* problem, const int prolen);
	/* ����ȡ�� */
	void ThreeToBest(const double* problem, const int prolen);
};
