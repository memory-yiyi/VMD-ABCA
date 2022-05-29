/*
 * MATLAB Compiler: 8.2 (R2021a)
 * Date: Wed Apr 27 23:51:44 2022
 * Arguments:
 * "-B""macro_default""-W""lib:mvmd,version=1.0""-T""link:lib""-d""E:\project\fx
 * tolib\mvmd\for_testing""-v""E:\project\mvmd.m"
 */
#pragma once
#ifndef mvmd_h
#define mvmd_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#ifdef __cplusplus
extern "C" { // sbcheck:ok:extern_c
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_mvmd_C_API 
#define LIB_mvmd_C_API /* No special import/export declaration */
#endif

/* GENERAL LIBRARY FUNCTIONS -- START */

extern LIB_mvmd_C_API 
bool MW_CALL_CONV mvmdInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_mvmd_C_API 
bool MW_CALL_CONV mvmdInitialize(void);

extern LIB_mvmd_C_API 
void MW_CALL_CONV mvmdTerminate(void);

extern LIB_mvmd_C_API 
void MW_CALL_CONV mvmdPrintStackTrace(void);

/* GENERAL LIBRARY FUNCTIONS -- END */

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_mvmd_C_API 
bool MW_CALL_CONV mlxMvmd(int nlhs, mxArray *plhs[], int nrhs, mxArray *prhs[]);

/* C INTERFACE -- MLX WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

/* C INTERFACE -- MLF WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- START */

extern LIB_mvmd_C_API bool MW_CALL_CONV mlfMvmd(int nargout, mxArray** imf, mxArray* x, mxArray* K, mxArray* alpha);

#ifdef __cplusplus
}
#endif
/* C INTERFACE -- MLF WRAPPERS FOR USER-DEFINED MATLAB FUNCTIONS -- END */

#endif
