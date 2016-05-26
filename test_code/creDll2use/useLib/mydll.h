#ifndef _MY_DLL_H_
#define _MY_DLL_H_


extern "C" _declspec(dllexport)int sum(int a, int b);
extern "C" _declspec(dllexport)int GetVer();

#endif