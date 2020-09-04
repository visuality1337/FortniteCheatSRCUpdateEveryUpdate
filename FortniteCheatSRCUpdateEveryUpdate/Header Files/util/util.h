/*

Visual#9999
*/
#pragma once
#include "../includes.h"
#define PI (3.141592653589793f)
#define RELATIVE_ADDR(addr, size) ((PBYTE)((UINT_PTR)(addr) + *(PINT)((UINT_PTR)(addr) + ((size) - sizeof(INT))) + (size)))

#define ReadPointer(base, offset) (*(PVOID *)(((PBYTE)base + offset)))
#define ReadDWORD(base, offset) (*(PDWORD)(((PBYTE)base + offset)))
#define ReadBYTE(base, offset) (*(((PBYTE)base + offset)))

namespace Util {
	BOOLEAN Initialize();
	VOID CreateConsole();
	PBYTE FindPattern(LPCSTR pattern, LPCSTR mask);
	std::wstring GetObjectFirstName(UObject* object);
	std::wstring GetObjectName(UObject* object);
	PVOID FindObject(LPCWSTR name);
	BOOLEAN WorldToScreen(float width, float height, float inOutPosition[3]);
	VOID ToMatrixWithScale(float* in, float out[4][4]);
	VOID GetBoneLocation(float compMatrix[4][4], PVOID bones, DWORD index, float out[3]);
	BOOLEAN LineOfSightTo(PVOID PlayerController, PVOID Actor, FVector* ViewPoint);
	FMinimalViewInfo& GetViewInfo();
	FVector* GetPawnRootLocation(PVOID pawn);
	VOID CalcAngle(float* src, float* dst, float* angles);

	extern VOID(*FreeInternal)(PVOID);

	namespace _SpoofCallInternal {
		extern "C" PVOID RetSpoofStub();

		template <typename Ret, typename... Args>
		inline Ret Wrapper(PVOID shell, Args... args) {
			auto fn = (Ret(*)(Args...))(shell);
			return fn(args...);
		}

		template <std::size_t Argc, typename>
		struct Remapper {
			template<typename Ret, typename First, typename Second, typename Third, typename Fourth, typename... Pack>
			static Ret Call(PVOID shell, PVOID shell_param, First first, Second second, Third third, Fourth fourth, Pack... pack) {
				return Wrapper<Ret, First, Second, Third, Fourth, PVOID, PVOID, Pack...>(shell, first, second, third, fourth, shell_param, nullptr, pack...);
			}
		};

		template <std::size_t Argc>
		struct Remapper<Argc, std::enable_if_t<Argc <= 4>> {
			template<typename Ret, typename First = PVOID, typename Second = PVOID, typename Third = PVOID, typename Fourth = PVOID>
			static Ret Call(PVOID shell, PVOID shell_param, First first = First{}, Second second = Second{}, Third third = Third{}, Fourth fourth = Fourth{}) {
				return Wrapper<Ret, First, Second, Third, Fourth, PVOID, PVOID>(shell, first, second, third, fourth, shell_param, nullptr);
			}
		};
	}

	template <typename Ret, typename... Args>
	Ret SpoofCall(Ret(*fn)(Args...), Args... args) {
		static PVOID trampoline = nullptr;
		if (!trampoline) {
			trampoline = Util::FindPattern("\xFF\x27", "xx");
			if (!trampoline) {
				MessageBox(0, L"Failed to find valid trampoline", L"Failure", 0);
				ExitProcess(0);
			}
		}

		struct {
			PVOID Trampoline;
			PVOID Function;
			PVOID Reg;
		} params = {
			trampoline,
			reinterpret_cast<void*>(fn),
		};

		return _SpoofCallInternal::Remapper<sizeof...(Args), void>::template Call<Ret, Args...>(&_SpoofCallInternal::RetSpoofStub, &params, args...);
	}
}