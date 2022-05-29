#pragma once
#include"rentype.h"
#include"mvmd.h"
#include<random>

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
	const short num = 40;				// ��Դ�ĸ���
	const short looker = num;			// �����ĸ���
	const short follow = 240;				// �����ĸ���
	const short maxit = 60;				// ����������
	const short limit = 40;				// ��󿪲ɴ���
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
		double* Series;			// ���������
		int len;					// �����ܳ���
		rentype::byte M;		// Ƕ��ά��
		int T;						// �������

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
		Swarm&& operator=(Swarm&&) = delete;
	} Bee;
	/* �й���Ӧ�Ⱥ����Ĳ��� */
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
	bool UpdateGlobal(Bee& comp1, Bee& comp2);
	/* ��ļ��������� */
	int Recruit(double* arr);
	/* �����������Դ���� */
	void Search(Fpara& para, int rr);
	/* ���¾ֲ����Ž� */
	bool UpdateLocal(Bee& comp1, Bee& comp2);
	/* ������Դ���� */
	void Abandon(Bee& b, Fpara& para);
public:
	/* �����㷨 */
	void Solution(const double* problem, const int prolen);
	/* ��ӡ��ǰȫ�����Ž� */
	void PrintOpt(int iteration)const;

	/* �����Ա���� */
};

