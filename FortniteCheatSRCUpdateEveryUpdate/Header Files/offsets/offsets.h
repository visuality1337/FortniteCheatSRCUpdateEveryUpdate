/*

Visual#9999
*/
#pragma once
#include "../Header Files/includes.h"
namespace Offsets {
	typedef struct {
		LPCWSTR Name;
		DWORD& Offset;
	} OFFSET;

	extern PVOID* uWorld;

	namespace Engine {
		namespace World {
			extern DWORD OwningGameInstance;
			extern DWORD Levels;
		}

		namespace Level {
			extern DWORD AActors;
		}

		namespace GameInstance {
			extern DWORD LocalPlayers;
		}

		namespace Player {
			extern DWORD PlayerController;
		}

		namespace Controller {
			extern DWORD ControlRotation;
			extern PVOID SetControlRotation;
			extern PVOID ClientSetRotation;
		}

		namespace PlayerController {
			extern DWORD AcknowledgedPawn;
		}

		namespace Pawn {
			extern DWORD PlayerState;
		}

		namespace PlayerState {
			extern PVOID GetPlayerName;
		}

		namespace Actor {
			extern DWORD RootComponent;
		}

		namespace Character {
			extern DWORD Mesh;
		}

		namespace SceneComponent {
			extern DWORD RelativeLocation;
			extern DWORD ComponentVelocity;
		}

		namespace StaticMeshComponent {
			extern DWORD ComponentToWorld;
			extern DWORD StaticMesh;
		}

		namespace SkinnedMeshComponent {
			extern DWORD CachedWorldSpaceBounds;
		}
	}

	namespace FortniteGame {
		namespace FortPawn {
			extern DWORD bIsDBNO;
			extern DWORD bIsDying;
			extern DWORD CurrentWeapon;
		}

		namespace FortPickup {
			extern DWORD PrimaryPickupItemEntry;
		}

		namespace FortItemEntry {
			extern DWORD ItemDefinition;
		}

		namespace FortItemDefinition {
			extern DWORD DisplayName;
			extern DWORD Tier;
		}

		namespace FortPlayerStateAthena {
			extern DWORD TeamIndex;
		}

		namespace FortWeapon {
			extern DWORD WeaponData;
		}

		namespace FortWeaponItemDefinition {
			extern DWORD WeaponStatHandle;
		}

		namespace FortProjectileAthena {
			extern DWORD FireStartLoc;
		}

		namespace FortBaseWeaponStats {
			extern DWORD ReloadTime;
		}

		namespace BuildingContainer {
			extern DWORD bAlreadySearched;
		}
	}

	BOOLEAN Initialize();
}