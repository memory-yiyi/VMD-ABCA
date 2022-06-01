#define _CRT_SECURE_NO_WARNINGS 
#include"VMD_ABCA.h"
#include"VMDRunFlag.h"
#include<iostream>
using std::cout;
using std::endl;


int main(int argc, char* argv[])
{
	FILE* fp;
	double* data;
	int len;
	char clen[_MAX_ITOSTR_BASE10_COUNT + 1] = { 0 }, * str;
	str = strrchr(argv[1], '\\');
	strncpy(clen, str + 1, _MAX_ITOSTR_BASE10_COUNT);
	if ((str = strchr(clen, '(')) != NULL)
		*str = 0;
	else
	{
		str = strchr(clen, '.');
		*str = 0;
	}
	len = atoi(clen);
	data = new double[len];
	if ((fp = fopen(argv[1], "rb")) == NULL)
	{
		cout << "Cann't open file: " << argv[1] << endl;
		getchar();
		return 1;
	}
	fread((void*)data, sizeof(double), len, fp);
	fclose(fp);

	VMD_ABCA Ans;
	VMDRunFlag::VMDInit();
	Ans.ThreeToBest(data, len);
	VMDRunFlag::VMDTerminate();

	delete[] data;
	getchar();
	return 0;
}
