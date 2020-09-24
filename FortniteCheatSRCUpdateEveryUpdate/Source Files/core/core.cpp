/*

Visual#9999
*/
#include "../../Header Files/menu/menu.h"
#include "../../Header Files/Config/config.h"
#include "../../Header Files/includes.h"
#include "../../DiscordHook/Discord.h"
#include <iostream>

namespace Core {
	bool NoSpread = true;
	bool IsAirstuck = true;
	int Current = 1;
	PVOID LocalPlayerPawn = nullptr;
	PVOID LocalPlayerController = nullptr;
	PVOID TargetPawn = nullptr;
	BYTE FreeCamDirection[6] = { 0 };
	FRotator FreeCamRotation[6] = { 0 };
	FVector FreeCamPosition = { 0 };
	FVector PlayerPosition = { 0 };
	PVOID(*ProcessEvent)(PVOID, PVOID, PVOID, PVOID) = nullptr;
	PVOID(*CalculateSpread)(PVOID, float*, float*) = nullptr;
	float* (*CalculateShot)(PVOID, PVOID, PVOID) = nullptr;
	VOID(*ReloadOriginal)(PVOID, PVOID) = nullptr;
	PVOID(*GetWeaponStats)(PVOID) = nullptr;
	INT(*GetViewPoint)(PVOID, FMinimalViewInfo*, BYTE) = nullptr;
	auto CurrentLocation = Core::FreeCamPosition;
	auto CurrentYaw = 0;
	auto CurrentPitch = 0;
	auto CurrenRoll = 0;
	auto OriginalAitstuck = 0;

	PVOID calculateSpreadCaller = nullptr;
	float originalReloadTime = 0.0f;

	BOOLEAN GetTarget(FVector& out) {
		if (!Core::TargetPawn) {
			return FALSE;
		}

		auto mesh = ReadPointer(Core::TargetPawn, 0x280);
		if (!mesh) {
			return FALSE;
		}

		auto bones = ReadPointer(mesh, 0x478);
		if (!bones) bones = ReadPointer(mesh, 0x478 + 0x10);
		if (!bones) {
			return FALSE;
		}

		float compMatrix[4][4] = { 0 };
		Util::ToMatrixWithScale(reinterpret_cast<float*>(reinterpret_cast<PBYTE>(mesh) + 0x1C0), compMatrix);

		float AimPointer;
		if (config_system.item.AimPoint == 0) {
			AimPointer = BONE_HEAD_ID;
		}
		else if (config_system.item.AimPoint == 1) {
			AimPointer = BONE_NECK_ID;
		}
		else if (config_system.item.AimPoint == 2) {
			AimPointer = BONE_CHEST_ID;
		}
		else if (config_system.item.AimPoint == 3) {
			AimPointer = BONE_PELVIS_ID;
		}
		else if (config_system.item.AimPoint == 4) {
			AimPointer = BONE_RIGHTELBOW_ID;
		}
		else if (config_system.item.AimPoint == 5) {
			AimPointer = BONE_LEFTELBOW_ID;
		}
		else if (config_system.item.AimPoint == 6) {
			AimPointer = BONE_RIGHTTHIGH_ID;
		}
		else if (config_system.item.AimPoint == 7) {
			AimPointer = BONE_LEFTTHIGH_ID;
		}
		else if (config_system.item.AimPoint == 8) {
			AimPointer = BONE_PELVIS_ID;
		}
		Util::GetBoneLocation(compMatrix, bones, AimPointer, &out.X);

		return TRUE;
	}
	PVOID ProcessEventHook(UObject* object, UObject* func, PVOID params, PVOID result) {
		if (object && func) {
			auto objectName = Util::GetObjectFirstName(object);
			auto funcName = Util::GetObjectFirstName(func);
			


			do {
				if (Core::TargetPawn && Core::LocalPlayerController) {
					if (wcsstr(objectName.c_str(), L"B_Prj_Bullet_Sniper") && funcName == L"OnRep_FireStart") {
						FVector CurrentAimPointer = { 0 };
						if (!GetTarget(CurrentAimPointer)) {
							break;
						}

						*reinterpret_cast<FVector*>(reinterpret_cast<PBYTE>(object) + Offsets::FortniteGame::FortProjectileAthena::FireStartLoc) = CurrentAimPointer;

						auto root = reinterpret_cast<PBYTE>(ReadPointer(object, Offsets::Engine::Actor::RootComponent));
						*reinterpret_cast<FVector*>(root + Offsets::Engine::SceneComponent::RelativeLocation) = CurrentAimPointer;
						memset(root + Offsets::Engine::SceneComponent::ComponentVelocity, 0, sizeof(FVector));
					}
					else if (!config_system.item.SilentAimbot && wcsstr(funcName.c_str(), L"Tick")) {
						FVector CurrentAimPointer;
						if (!GetTarget(CurrentAimPointer)) { //head
							break;
						}

						float angles[2] = { 0 };
						Util::CalcAngle(&Util::GetViewInfo().Location.X, &CurrentAimPointer.X, angles); //head instead of neck.X

						if (config_system.item.AimbotSlow <= 0.0f) {
							if (config_system.item.TriggerAimbot) {
								FRotator args = { 0 };
								args.Pitch = angles[0];
								args.Yaw = angles[1];
								ProcessEvent(Core::LocalPlayerController, Offsets::Engine::Controller::ClientSetRotation, &args, 0);
								mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
								mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
								int x = 0;
								while (x < config_system.item.TriggerSpeed * 10) {
									x++;
								}
								x = 0;
							}
							else {
								auto scale = config_system.item.AimbotSlow + 1.0f;
								auto currentRotation = Util::GetViewInfo().Rotation;

								FRotator args = { 0 };
								args.Pitch = (angles[0] - currentRotation.Pitch) / scale + currentRotation.Pitch;
								args.Yaw = (angles[1] - currentRotation.Yaw) / scale + currentRotation.Yaw;
								ProcessEvent(Core::LocalPlayerController, Offsets::Engine::Controller::ClientSetRotation, &args, 0);
							}
						}
						else {
							if (config_system.item.TriggerAimbot) {
								auto scale = config_system.item.AimbotSlow + 1.0f;
								auto currentRotation = Util::GetViewInfo().Rotation;

								FRotator args = { 0 };
								args.Pitch = (angles[0] - currentRotation.Pitch) / scale + currentRotation.Pitch;
								args.Yaw = (angles[1] - currentRotation.Yaw) / scale + currentRotation.Yaw;
								ProcessEvent(Core::LocalPlayerController, Offsets::Engine::Controller::ClientSetRotation, &args, 0);
								mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
								mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
								int v = 0;
								while (v < config_system.item.TriggerSpeed * 10) {
									v++;
								}
								v = 0;
							}
							else {
								auto scale = config_system.item.AimbotSlow + 1.0f;
								auto currentRotation = Util::GetViewInfo().Rotation;

								FRotator args = { 0 };
								args.Pitch = (angles[0] - currentRotation.Pitch) / scale + currentRotation.Pitch;
								args.Yaw = (angles[1] - currentRotation.Yaw) / scale + currentRotation.Yaw;
								ProcessEvent(Core::LocalPlayerController, Offsets::Engine::Controller::ClientSetRotation, &args, 0);
							}

						}
					}
				}
			} while (FALSE);
		}

		return ProcessEvent(object, func, params, result);
	}
	
	PVOID CalculateSpreadHook(PVOID arg0, float* arg1, float* arg2) {
		if (originalReloadTime != 0.0f) {
			auto localPlayerWeapon = ReadPointer(Core::LocalPlayerPawn, Offsets::FortniteGame::FortPawn::CurrentWeapon);
			if (localPlayerWeapon) {
				auto stats = GetWeaponStats(localPlayerWeapon);
				if (stats) {
					*reinterpret_cast<float*>(reinterpret_cast<PBYTE>(stats) + Offsets::FortniteGame::FortBaseWeaponStats::ReloadTime) = originalReloadTime;
					originalReloadTime = 0.0f;
				}
			}
		}

		if (config_system.item.NoSpreadAimbot && Core::NoSpread && _ReturnAddress() == calculateSpreadCaller) {
			return 0;
		}

		return CalculateSpread(arg0, arg1, arg2);
	}

	float* CalculateShotHook(PVOID arg0, PVOID arg1, PVOID arg2) {
		auto ret = CalculateShot(arg0, arg1, arg2);
		if (ret && config_system.item.SilentAimbot && Core::TargetPawn && Core::LocalPlayerPawn) {
			auto mesh = ReadPointer(Core::TargetPawn, Offsets::Engine::Character::Mesh);
			if (!mesh) return ret;

			auto bones = ReadPointer(mesh, Offsets::Engine::StaticMeshComponent::StaticMesh);
			if (!bones) bones = ReadPointer(mesh, Offsets::Engine::StaticMeshComponent::StaticMesh + 0x10);
			if (!bones) return ret;

			float compMatrix[4][4] = { 0 };
			Util::ToMatrixWithScale(reinterpret_cast<float*>(reinterpret_cast<PBYTE>(mesh) + Offsets::Engine::StaticMeshComponent::ComponentToWorld), compMatrix);

			FVector CurrentAimPoint = { 0 };
			int CurrentAimPointer = BONE_HEAD_ID;
			if (config_system.item.AimPoint == 0) {
				CurrentAimPointer = BONE_HEAD_ID;
			}
			else if (config_system.item.AimPoint == 1) {
				CurrentAimPointer = BONE_NECK_ID;
			}
			else if (config_system.item.AimPoint == 2) {
				CurrentAimPointer = BONE_CHEST_ID;
			}
			else if (config_system.item.AimPoint == 3) {
				CurrentAimPointer = BONE_PELVIS_ID;
			}
			else if (config_system.item.AimPoint == 4) {
				CurrentAimPointer = BONE_RIGHTELBOW_ID;
			}
			else if (config_system.item.AimPoint == 5) {
				CurrentAimPointer = BONE_LEFTELBOW_ID;
			}
			else if (config_system.item.AimPoint == 6) {
				CurrentAimPointer = BONE_RIGHTTHIGH_ID;
			}
			else if (config_system.item.AimPoint == 7) {
				CurrentAimPointer = BONE_LEFTTHIGH_ID;
			}
			else if (config_system.item.AimPoint == 8) {
				CurrentAimPointer = BONE_PELVIS_ID;
			}
			Util::GetBoneLocation(compMatrix, bones, CurrentAimPointer, &CurrentAimPoint.X);

			auto rootPtr = Util::GetPawnRootLocation(Core::LocalPlayerPawn);
			if (!rootPtr) return ret;
			auto root = *rootPtr;

			auto dx = CurrentAimPoint.X - root.X;
			auto dy = CurrentAimPoint.Y - root.Y;
			auto dz = CurrentAimPoint.Z - root.Z;
			if (dx * dx + dy * dy + dz * dz < 125000.0f) {
				ret[4] = CurrentAimPoint.X;
				ret[5] = CurrentAimPoint.Y;
				ret[6] = CurrentAimPoint.Z;
			}
			else {
				CurrentAimPoint.Z -= 16.0f;
				root.Z += 45.0f;

				auto y = atan2f(CurrentAimPoint.Y - root.Y, CurrentAimPoint.X - root.X);

				root.X += cosf(y + 1.5708f) * 32.0f;
				root.Y += sinf(y + 1.5708f) * 32.0f;

				auto length = Util::SpoofCall(sqrtf, powf(CurrentAimPoint.X - root.X, 2) + powf(CurrentAimPoint.Y - root.Y, 2));
				auto x = -atan2f(CurrentAimPoint.Z - root.Z, length);
				y = atan2f(CurrentAimPoint.Y - root.Y, CurrentAimPoint.X - root.X);

				x /= 2.0f;
				y /= 2.0f;

				ret[0] = -(sinf(x) * sinf(y));
				ret[1] = sinf(x) * cosf(y);
				ret[2] = cosf(x) * sinf(y);
				ret[3] = cosf(x) * cosf(y);
			}
		}

		return ret;
	}

	INT GetViewPointHook(PVOID player, FMinimalViewInfo* viewInfo, BYTE stereoPass) 
	{
		static HWND TargetWindow = 0;
		float CurrentSpeed = config_system.item.FreeCamSpeed;
		TargetWindow = FindWindow((L"UnrealWindow"), (L"Fortnite  "));


		auto RotationYaw = viewInfo->Rotation.Yaw * PI / 180.0f;
		const float FlySpeed = 0.10f;
		const float HighFOV = 50.534008f;
		const float LowFOV = 40.0f;
		static FVector FreecamPoint = { 0 };
		static FRotator FreeCamRotation = { 0 };

		FVector DirectionPoint = {
				static_cast<float>(Core::FreeCamDirection[0] - Core::FreeCamDirection[1]),
				static_cast<float>(Core::FreeCamDirection[2] - Core::FreeCamDirection[3]),
				static_cast<float>(Core::FreeCamDirection[4] - Core::FreeCamDirection[5]),
		};

		auto CurrentViewPoint = GetViewPoint(player, viewInfo, stereoPass);

		auto CurrentFOV = viewInfo->FOV;
		auto NormalFOV = (((180.0f - HighFOV) / (180.0f - 80.0f)) * (config_system.item.FOV - 80.0f)) + HighFOV;

		if (CurrentFOV > HighFOV) {
			CurrentFOV = NormalFOV;
		}
		else if (CurrentFOV > LowFOV) {
			CurrentFOV = (((CurrentFOV - LowFOV) / (HighFOV - LowFOV)) * (NormalFOV - LowFOV)) + LowFOV;
		}

		auto master = Util::SpoofCall(sqrtf, DirectionPoint.X * DirectionPoint.X + DirectionPoint.Y * DirectionPoint.Y + DirectionPoint.Z * DirectionPoint.Z);
		if (master != 0) {
			DirectionPoint.X /= master;
			DirectionPoint.Y /= master;
			DirectionPoint.Z /= master;
		}

		if (config_system.item.FOVSlider) {
			viewInfo->FOV = CurrentFOV;
		}
		
		if (config_system.item.SpinBot) {
			if (Util::SpoofCall(GetAsyncKeyState, config_system.keybind.Spinbot) && Util::SpoofCall(GetForegroundWindow) == TargetWindow) {

				// Freeze Cam
				viewInfo->Location = CurrentLocation;
				viewInfo->Rotation.Yaw = CurrentYaw;
				viewInfo->Rotation.Pitch = CurrentPitch;
				viewInfo->Rotation.Roll = CurrenRoll;
				return CurrentViewPoint;
			}
			else {
				// 
				CurrentLocation = Util::GetViewInfo().Location;
				CurrentYaw = viewInfo->Rotation.Yaw;
				CurrentPitch = viewInfo->Rotation.Pitch;
				CurrenRoll = viewInfo->Rotation.Roll;
			}
		}

		return CurrentViewPoint;
	}

	VOID ReloadHook(PVOID arg0, PVOID arg1) {
		if (config_system.item.InstantReload && Core::LocalPlayerPawn) {
			auto localPlayerWeapon = ReadPointer(Core::LocalPlayerPawn, Offsets::FortniteGame::FortPawn::CurrentWeapon);
			if (localPlayerWeapon) {
				auto stats = GetWeaponStats(localPlayerWeapon);
				if (stats) {
					auto& reloadTime = *reinterpret_cast<float*>(reinterpret_cast<PBYTE>(stats) + Offsets::FortniteGame::FortBaseWeaponStats::ReloadTime);
					if (reloadTime != 0.01f) {
						originalReloadTime = reloadTime;
						reloadTime = 0.01f;
					}
				}
			}
		}

		ReloadOriginal(arg0, arg1);
	}

	BOOLEAN Initialize() {

		// GetWeaponStats
		auto addr = Util::FindPattern("\x48\x83\xEC\x58\x48\x8B\x91\x00\x00\x00\x00\x48\x85\xD2\x0F\x84\x00\x00\x00\x00\xF6\x81\x00\x00\x00\x00\x00\x74\x10\x48\x8B\x81\x00\x00\x00\x00\x48\x85\xC0\x0F\x85\x00\x00\x00\x00\x48\x8B\x8A\x00\x00\x00\x00\x48\x89\x5C\x24\x00\x48\x8D\x9A\x00\x00\x00\x00\x48\x85\xC9", "xxxxxxx????xxxxx????xx?????xxxxx????xxxxx????xxx????xxxx?xxx????xxx");
		if (!addr) {
			MessageBox(0, L"Failed to find GetWeaponStats", L"Failure", 0);
			return FALSE;
		}

		GetWeaponStats = reinterpret_cast<decltype(GetWeaponStats)>(addr);

		// ProcessEvent
		addr = Util::FindPattern("\x40\x55\x56\x57\x41\x54\x41\x55\x41\x56\x41\x57\x48\x81\xEC\x00\x00\x00\x00\x48\x8D\x6C\x24\x00\x48\x89\x9D\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC5\x48\x89\x85\x00\x00\x00\x00\x8B\x41\x0C\x45\x33\xF6\x3B\x05\x00\x00\x00\x00\x4D\x8B\xF8\x48\x8B\xF2\x4C\x8B\xE1\x41\xB8\x00\x00\x00\x00\x7D\x2A", "xxxxxxxxxxxxxxx????xxxx?xxx????xxx????xxxxxx????xxxxxxxx????xxxxxxxxxxx????xx");
		/*if (!addr) {
			MessageBox(0, L"Failed to find ProcessEvent", L"Failure", 0);
			return FALSE;
		}*/

		DISCORD.HookFunction((uintptr_t)addr, (uintptr_t)ProcessEventHook, (uintptr_t)&ProcessEvent);;

		//CalculateShot
		addr = Util::FindPattern("\x40\x55\x53\x56\x57\x48\x8D\x6C\x24\x00\x48\x81\xEC\x00\x00\x00\x00\x48\x8B\x05\x00\x00\x00\x00\x48\x33\xC4\x48\x89\x45\xE0\x49", "xxxxxxxxx?xxx????xxx????xxxxxxxx");
		if (!addr) {
			MessageBox(0, L"Failed to find CalculateShot", L"Failure", 0);
			return FALSE;
		}

		DISCORD.HookFunction((uintptr_t)addr, (uintptr_t)CalculateShotHook, (uintptr_t)&CalculateShot);

		// GetViewPoint
		addr = Util::FindPattern("\x48\x89\x5C\x24\x00\x48\x89\x74\x24\x00\x57\x48\x83\xEC\x20\x48\x8B\xD9\x41\x8B\xF0\x48\x8B\x49\x30\x48\x8B\xFA\xE8\x00\x00\x00\x00\xBA\x00\x00\x00\x00\x48\x8B\xC8", "xxxx?xxxx?xxxxxxxxxxxxxxxxxxx????x????xxx");
		if (!addr) {
			MessageBox(0, L"Failed to find GetViewPoint", L"Failure", 0);
			return FALSE;
		}

		DISCORD.HookFunction((uintptr_t)addr, (uintptr_t)GetViewPointHook, (uintptr_t)&GetViewPoint);



	}
}
