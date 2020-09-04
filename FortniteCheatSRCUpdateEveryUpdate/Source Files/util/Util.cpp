/*

Visual#9999
*/

#include "../../Header Files/includes.h"
#include "../../DiscordHook/Discord.h"
#include <corecrt_math.h>

namespace Util {
	GObjects* objects = nullptr;
	FString(*GetObjectNameInternal)(PVOID) = nullptr;
	VOID(*FreeInternal)(PVOID) = nullptr;
	BOOL(*LineOfSightToInternal)(PVOID PlayerController, PVOID Actor, FVector* ViewPoint) = nullptr;
	VOID(*CalculateProjectionMatrixGivenView)(FMinimalViewInfo* viewInfo, BYTE aspectRatioAxisConstraint, PBYTE viewport, FSceneViewProjectionData* inOutProjectionData) = nullptr;

	struct {
		FMinimalViewInfo Info;
		float ProjectionMatrix[4][4];
	} view = { 0 };

	VOID CreateConsole() {
		AllocConsole();
		static_cast<VOID>(freopen("CONIN$", "r", stdin));
		static_cast<VOID>(freopen("CONOUT$", "w", stdout));
		static_cast<VOID>(freopen("CONOUT$", "w", stderr));
	}

	BOOLEAN MaskCompare(PVOID buffer, LPCSTR pattern, LPCSTR mask) {
		for (auto b = reinterpret_cast<PBYTE>(buffer); *mask; ++pattern, ++mask, ++b) {
			if (*mask == 'x' && *reinterpret_cast<LPCBYTE>(pattern) != *b) {
				return FALSE;
			}
		}

		return TRUE;
	}

	PBYTE FindPattern(PVOID base, DWORD size, LPCSTR pattern, LPCSTR mask) {
		size -= static_cast<DWORD>(strlen(mask));

		for (auto i = 0UL; i < size; ++i) {
			auto addr = reinterpret_cast<PBYTE>(base) + i;
			if (MaskCompare(addr, pattern, mask)) {
				return addr;
			}
		}

		return NULL;
	}

	PBYTE FindPattern(LPCSTR pattern, LPCSTR mask) {
		MODULEINFO info = { 0 };
		GetModuleInformation(GetCurrentProcess(), GetModuleHandle(0), &info, sizeof(info));

		return FindPattern(info.lpBaseOfDll, info.SizeOfImage, pattern, mask);
	}

	VOID Free(PVOID buffer) {
		FreeInternal(buffer);
	}

	std::wstring GetObjectFirstName(UObject* object) {
		auto internalName = GetObjectNameInternal(object);
		if (!internalName.c_str()) {
			return L"";
		}

		std::wstring name(internalName.c_str());
		Free(internalName.c_str());

		return name;
	}

	std::wstring GetObjectName(UObject* object) {
		std::wstring name(L"");
		for (auto i = 0; object; object = object->Outer, ++i) {
			auto internalName = GetObjectNameInternal(object);
			if (!internalName.c_str()) {
				break;
			}

			name = internalName.c_str() + std::wstring(i > 0 ? L"." : L"") + name;
			Free(internalName.c_str());
		}

		return name;
	}

	BOOLEAN GetOffsets(std::vector<Offsets::OFFSET>& offsets) {
		auto current = 0ULL;
		auto size = offsets.size();

		for (auto array : objects->ObjectArray->Objects) {
			auto fuObject = array;
			for (auto i = 0; i < 0x10000 && fuObject->Object; ++i, ++fuObject) {
				auto object = fuObject->Object;
				if (object->ObjectFlags != 0x41) {
					continue;
				}

				auto name = GetObjectName(object);
				for (auto& o : offsets) {
					if (!o.Offset && name == o.Name) {
						o.Offset = *reinterpret_cast<PDWORD>(reinterpret_cast<PBYTE>(object) + 0x44);

						if (++current == size) {
							return TRUE;
						}

						break;
					}
				}
			}
		}

		for (auto& o : offsets) {
			if (!o.Offset) {
				WCHAR buffer[0xFF] = { 0 };
				wsprintf(buffer, L"Offset %ws not found", o.Name);
				MessageBox(0, buffer, L"Failure", 0);
			}
		}

		return FALSE;
	}

	PVOID FindObject(LPCWSTR name) {
		for (auto array : objects->ObjectArray->Objects) {
			auto fuObject = array;
			for (auto i = 0; i < 0x10000 && fuObject->Object; ++i, ++fuObject) {
				auto object = fuObject->Object;
				if (object->ObjectFlags != 0x41) {
					continue;
				}

				if (GetObjectName(object) == name) {
					return object;
				}
			}
		}

		return 0;
	}

	VOID ToMatrixWithScale(float* in, float out[4][4])
	{
		auto* rotation = &in[0];
		auto* translation = &in[4];
		auto* scale = &in[8];

		out[3][0] = translation[0];
		out[3][1] = translation[1];
		out[3][2] = translation[2];

		auto x2 = rotation[0] + rotation[0];
		auto y2 = rotation[1] + rotation[1];
		auto z2 = rotation[2] + rotation[2];

		auto xx2 = rotation[0] * x2;
		auto yy2 = rotation[1] * y2;
		auto zz2 = rotation[2] * z2;
		out[0][0] = (1.0f - (yy2 + zz2)) * scale[0];
		out[1][1] = (1.0f - (xx2 + zz2)) * scale[1];
		out[2][2] = (1.0f - (xx2 + yy2)) * scale[2];

		auto yz2 = rotation[1] * z2;
		auto wx2 = rotation[3] * x2;
		out[2][1] = (yz2 - wx2) * scale[2];
		out[1][2] = (yz2 + wx2) * scale[1];

		auto xy2 = rotation[0] * y2;
		auto wz2 = rotation[3] * z2;
		out[1][0] = (xy2 - wz2) * scale[1];
		out[0][1] = (xy2 + wz2) * scale[0];

		auto xz2 = rotation[0] * z2;
		auto wy2 = rotation[3] * y2;
		out[2][0] = (xz2 + wy2) * scale[2];
		out[0][2] = (xz2 - wy2) * scale[0];

		out[0][3] = 0.0f;
		out[1][3] = 0.0f;
		out[2][3] = 0.0f;
		out[3][3] = 1.0f;
	}

	VOID MultiplyMatrices(float a[4][4], float b[4][4], float out[4][4]) {
		for (auto r = 0; r < 4; ++r) {
			for (auto c = 0; c < 4; ++c) {
				auto sum = 0.0f;

				for (auto i = 0; i < 4; ++i) {
					sum += a[r][i] * b[i][c];
				}

				out[r][c] = sum;
			}
		}
	}

	VOID GetBoneLocation(float compMatrix[4][4], PVOID bones, DWORD index, float out[3]) {
		float boneMatrix[4][4];
		ToMatrixWithScale((float*)((PBYTE)bones + (index * 0x30)), boneMatrix);

		float result[4][4];
		MultiplyMatrices(boneMatrix, compMatrix, result);

		out[0] = result[3][0];
		out[1] = result[3][1];
		out[2] = result[3][2];
	}

	VOID GetViewProjectionMatrix(FSceneViewProjectionData* projectionData, float out[4][4]) {
		auto loc = &projectionData->ViewOrigin;

		float translation[4][4] = {
			{ 1.0f, 0.0f, 0.0f, 0.0f, },
			{ 0.0f, 1.0f, 0.0f, 0.0f, },
			{ 0.0f, 0.0f, 1.0f, 0.0f, },
			{ -loc->X, -loc->Y, -loc->Z, 0.0f, },
		};

		float temp[4][4];
		MultiplyMatrices(translation, projectionData->ViewRotationMatrix.M, temp);
		MultiplyMatrices(temp, projectionData->ProjectionMatrix.M, out);
	}

	BOOLEAN ProjectWorldToScreen(float viewProjection[4][4], float width, float height, float inOutPosition[3]) {
		float res[4] = {
			viewProjection[0][0] * inOutPosition[0] + viewProjection[1][0] * inOutPosition[1] + viewProjection[2][0] * inOutPosition[2] + viewProjection[3][0],
			viewProjection[0][1] * inOutPosition[0] + viewProjection[1][1] * inOutPosition[1] + viewProjection[2][1] * inOutPosition[2] + viewProjection[3][1],
			viewProjection[0][2] * inOutPosition[0] + viewProjection[1][2] * inOutPosition[1] + viewProjection[2][2] * inOutPosition[2] + viewProjection[3][2],
			viewProjection[0][3] * inOutPosition[0] + viewProjection[1][3] * inOutPosition[1] + viewProjection[2][3] * inOutPosition[2] + viewProjection[3][3],
		};

		auto r = res[3];
		if (r > 0) {
			auto rhw = 1.0f / r;

			inOutPosition[0] = (((res[0] * rhw) / 2.0f) + 0.5f) * width;
			inOutPosition[1] = (0.5f - ((res[1] * rhw) / 2.0f)) * height;
			inOutPosition[2] = r;

			return TRUE;
		}

		return FALSE;
	}

	VOID CalculateProjectionMatrixGivenViewHook(FMinimalViewInfo* viewInfo, BYTE aspectRatioAxisConstraint, PBYTE viewport, FSceneViewProjectionData* inOutProjectionData) {
		CalculateProjectionMatrixGivenView(viewInfo, aspectRatioAxisConstraint, viewport, inOutProjectionData);

		view.Info = *viewInfo;
		GetViewProjectionMatrix(inOutProjectionData, view.ProjectionMatrix);
	}

	BOOLEAN WorldToScreen(float width, float height, float inOutPosition[3]) {
		return ProjectWorldToScreen(view.ProjectionMatrix, width, height, inOutPosition);
	}

	BOOLEAN LineOfSightTo(PVOID PlayerController, PVOID Actor, FVector* ViewPoint) {
		return SpoofCall(LineOfSightToInternal, PlayerController, Actor, ViewPoint);
	}

	FMinimalViewInfo& GetViewInfo() {
		return view.Info;
	}

	FVector* GetPawnRootLocation(PVOID pawn) {
		auto root = ReadPointer(pawn, Offsets::Engine::Actor::RootComponent);
		if (!root) {
			return nullptr;
		}

		return reinterpret_cast<FVector*>(reinterpret_cast<PBYTE>(root) + Offsets::Engine::SceneComponent::RelativeLocation);
	}

	float Normalize(float angle) {
		float a = (float)fmod(fmod(angle, 360.0) + 360.0, 360.0);
		if (a > 180.0f) {
			a -= 360.0f;
		}
		return a;
	}

	VOID CalcAngle(float* src, float* dst, float* angles) {
		float rel[3] = {
			dst[0] - src[0],
			dst[1] - src[1],
			dst[2] - src[2],
		};

		auto dist = sqrtf(rel[0] * rel[0] + rel[1] * rel[1] + rel[2] * rel[2]);
		auto yaw = atan2f(rel[1], rel[0]) * (180.0f / PI);
		auto pitch = (-((acosf((rel[2] / dist)) * 180.0f / PI) - 90.0f));

		angles[0] = Normalize(pitch);
		angles[1] = Normalize(yaw);
	}

	BOOLEAN Initialize() {
			// GObjects
			auto addr = FindPattern("\x48\x8B\x05\x7E\x38\x79\x05\x4C\x8D\x34\xCD", "xxx????xxxx");
			if (!addr) {
				MessageBox(0, L"Failed to find GObjects", L"Failure", 0);
				return FALSE;
			}

		objects = reinterpret_cast<decltype(objects)>(RELATIVE_ADDR(addr, 7));

		// GetObjectName
		addr = FindPattern("\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\x48\x85\xD2\x75\x45\x33\xC0\x48\x89\x01\x48\x89\x41\x08\x8D\x50\x05\xE8\x00\x00\x00\x00\x8B\x53\x08\x8D\x42\x05\x89\x43\x08\x3B\x43\x0C\x7E\x08\x48\x8B\xCB\xE8\x00\x00\x00\x00\x48\x8B\x0B\x48\x8D\x15\x00\x00\x00\x00\x41\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\xC3\x48\x83\xC4\x20\x5B\xC3\x48\x8B\x42\x18", "xxxxxxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxxxxxxxx????xxxxxx????xx????x????xxxxxxxxxxxxx");
		if (!addr) {
			MessageBox(0, L"Failed to find GetObjectNameInternal", L"Failure", 0);
			return FALSE;
		}

		GetObjectNameInternal = reinterpret_cast<decltype(GetObjectNameInternal)>(addr);

		// Free
		addr = FindPattern("\x48\x85\xC9\x74\x2E\x53\x48\x83\xEC\x20\x48\x8B\xD9\x48\x8B\x0D\x00\x00\x00\x00\x48\x85\xC9\x75\x0C", "xxxxxxxxxxxxxxxx????xxxxx");
		if (!addr) {
			MessageBox(0, L"Failed to find FreeInternal", L"Failure", 0);
			return FALSE;
		}

		FreeInternal = reinterpret_cast<decltype(FreeInternal)>(addr);

		// CalculateProjectionMatrixGivenView
		addr = FindPattern("\xF3\x0F\x5F\x0D\x00\x00\x00\x00\x41\x8B\x41\x08", "xxxx????xxxx");
		if (!addr) {
			MessageBox(0, L"Failed to find CalculateProjectionMatrixGivenView", L"Failure", 0);
			return FALSE;
		}

		addr -= 0x280;
		DISCORD.HookFunction((uintptr_t)addr, (uintptr_t)CalculateProjectionMatrixGivenViewHook, (uintptr_t)&CalculateProjectionMatrixGivenView);
		/*MH_CreateHook(addr, CalculateProjectionMatrixGivenViewHook, (PVOID*)&CalculateProjectionMatrixGivenView);
		MH_EnableHook(addr);*/

		// LineOfSightTo    40 55 53 56 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8D 05 ? ? ? ? 48 33 C4 48 89 45 E0 49
		addr = FindPattern("\x40\x55\x53\x56\x57\x48\x8D\x6C\x24\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\xE0\x49", "xxxxxxxxx?xxx????xxx????xxxxxxxx");
		if (!addr) {
			MessageBox(0, L"Failed to find LineOfSightTo", L"Failure", 0);
			return FALSE;
		}

		LineOfSightToInternal = reinterpret_cast<decltype(LineOfSightToInternal)>(addr);

		return TRUE;
	}
}
