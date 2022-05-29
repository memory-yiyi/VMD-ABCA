#pragma once
#include"mvmd.h"

namespace VMDRunFlag
{
	bool Flag = false;				// matlab∆Ù∂Ø±Í÷æ
	bool VMDInit();
	bool VMDTerminate();
}

inline bool VMDRunFlag::VMDInit()
{
	if (!mclInitializeApplication(NULL, 0))
		return false;
	if (!mvmdInitialize())
		return false;
	Flag = true;
	return true;
}

inline bool VMDRunFlag::VMDTerminate()
{
	mvmdTerminate();
	mclTerminateApplication();
	Flag = false;
	return true;
}
