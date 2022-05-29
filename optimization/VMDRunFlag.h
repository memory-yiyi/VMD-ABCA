#pragma once
#include"mvmd.h"

namespace VMDRunFlag
{
	bool flag = false;				// matlab������־
	bool VMDInit();
	bool VMDTerminate();
}

inline bool VMDRunFlag::VMDInit()
{
	if (!mclInitializeApplication(NULL, 0))
		return false;
	if (!mvmdInitialize())
		return false;
	flag = true;
	return true;
}

inline bool VMDRunFlag::VMDTerminate()
{
	mvmdTerminate();
	mclTerminateApplication();
	flag = false;
	return true;
}
