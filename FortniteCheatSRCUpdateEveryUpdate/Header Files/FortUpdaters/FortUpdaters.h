/*

Visual#9999
*/
#pragma once
#include <Windows.h>
#include <iostream>
#include <string>

class FortUpdater
{
public:
	bool Init(uintptr_t UObjectArray, uintptr_t GetObjectName, uintptr_t GetNameByIndex, uintptr_t FnFree);
	DWORD FindOffset(const char* Class, const char* var);

private:
	uintptr_t UObjectArray = NULL;
	uintptr_t GetObjectName = NULL;
	uintptr_t GetNameByIndex = NULL;
	uintptr_t FnFree = NULL;

	void cFixName(char* Name);
	void FreeObjName(uintptr_t Address);
	char* fGetObjectName(uintptr_t Object);
	char* fGetNameByIndex(int Index);
};