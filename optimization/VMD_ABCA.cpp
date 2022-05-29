#include"VMD_ABCA.h"
#include"mathexpand.h"
#include<cmath>
#include<iostream>
using namespace math;
namespace rt = rentype;
using std::cout;
using std::endl;

/* ������������������Ķ��� */
namespace Random
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> randK(FSpara::Kmin, FSpara::Kmax);							// �����K
	std::uniform_int_distribution<> randAlpha(FSpara::Alphamin, FSpara::Alphamax);		// �����Alpha
	std::uniform_int_distribution<> randt(0, Npara::num - 1);											// �������ָ����������Դ
	double rand = 0.;													// �����(����)�����ڸ����������Դ����
	std::uniform_real_distribution<> randr(-1., 1.);				// ����������ڸ����������Դ����
	std::uniform_real_distribution<> randroulette(0., 1.);		// ��������������̶�
}
namespace rd = Random;


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
	delete[] IMF;
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

	int i, j, k, m;													// ѭ������
	int t = para->T + 1;											// ��������
	int nrest = para->len - (para->M - 1) * t;				// �ع�����������
	double* crest = new double[para->M];					// �����ع�����
	int nindex = factorial(para->M);							// ��������������
	rt::byte* cindex = new rt::byte[para->M];						// ������������
	rt::byte* rloca = new rt::byte[para->M];						// ���������и�Ԫ�ص����λ��
	int loca;															// ���������������е�λ��
	int* index = new int[nindex] {0};							// ��������������
	int& sdata = nrest;											// ��������
	double pcin;													// �����������ֵĸ���
	double ans = 0;												// ������

	for (i = 0; i < nrest; i++)
	{
		/* ��ʼ�� */
		loca = 0;
		for (j = i, k = 0; k < para->M; j += t, k++)
		{
			crest[k] = para->Series[j];
			cindex[k] = rloca[k] = k;
		}
		/* ð������ */
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
		/* ѡ������ */
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
	/* ������ */
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

inline bool VMD_ABCA::UpdateGlobal(Bee& comp1, Bee& comp2)
{
	if (comp1 > comp2 || (comp1 == comp2 && comp1.choice.K > comp2.choice.K))
	{
		comp2 = comp1;
		return true;
	}
	else
		return false;
}

inline int VMD_ABCA::Recruit(double* arr)
{
	double fitnessSum = sum<double, sizeof(VMD_ABCA)>(&looker[0].fitness, Npara::looker);
	arr[0] = looker[0].fitness / fitnessSum;
	for (int i = 1; i < Npara::looker; i++)
	{
		arr[i] = looker[i].fitness / fitnessSum;
		arr[i] += arr[i - 1];
	}
	double rand = rd::randroulette(rd::gen);
	return GetAddressOfArray<double>(arr, std::find_if(arr, arr + Npara::looker, [&rand](int x)->bool {return x >= rand; }));
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

inline bool VMD_ABCA::UpdateLocal(Bee& comp1, Bee& comp2)
{
	if (comp1 > comp2 || (comp1 == comp2 && comp1.choice.K > comp2.choice.K))
	{
		comp2 = comp1;
		UpdateGlobal(comp2, optimal);
		return true;
	}
	else
	{
		comp2.count++;
		return false;
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

void VMD_ABCA::Solution(const double* problem, const int prolen)
{
	int i, j, k;											// ѭ������
	double roulette[Npara::looker];			// ����
	int resultroulette;								// ���̶Ľ��
	Fpara Fpa((double*)problem, prolen);

	/* ��ʼ����Դ */
	for (i = 0; i < Npara::num; i++)
		InitNectar(looker[i], Fpa);

	/* ��Ⱥ���� */
	for (i = 0; i < Npara::maxit; i++)
	{
		for (j = 0; j < Npara::follow; j++)
		{
			resultroulette = Recruit(roulette);
			Search(Fpa, resultroulette);
			Abandon(looker[resultroulette], Fpa);
		} // for (j = 0; j < Npara::follow; j++)
		PrintOpt(i);
	} // for (i = 0; i < Npara::maxit; i++)
}

inline void VMD_ABCA::PrintOpt(int iteration)const
{
	printf("%d: [%2d %4d] %.7f", iteration, optimal.choice.K, optimal.choice.Alpha, optimal.fitness);
}
