/*

Visual#9999
*/

#include "../../Header Files/xorstr.h"
#include "../../imgui/imgui_xorstr.h"
#include "../Header Files/includes.h"
namespace Offsets {
	PVOID* uWorld = 0;

	namespace Engine {
		namespace World {
			DWORD OwningGameInstance = 0;
			DWORD Levels = 0;
		}

		namespace Level {
			DWORD AActors = 0;
		}

		namespace GameInstance {
			DWORD LocalPlayers = 0;
		}

		namespace Player {
			DWORD PlayerController = 0;
		}

		namespace Controller {
			DWORD ControlRotation = 0;
			PVOID SetControlRotation = 0;
			PVOID ClientSetRotation;
		}

		namespace PlayerController {
			DWORD AcknowledgedPawn = 0;
			DWORD PlayerCameraManager = 0;
		}

		namespace Pawn {
			DWORD PlayerState = 0;
		}

		namespace PlayerState {
			PVOID GetPlayerName = 0;
		}

		namespace Actor {
			DWORD RootComponent = 0;
			DWORD CustomTimeDilation = 0;
		}

		namespace Character {
			DWORD Mesh = 0;
		}

		namespace SceneComponent {
			DWORD RelativeLocation = 0;
			DWORD ComponentVelocity = 0;
		}

		namespace StaticMeshComponent {
			DWORD ComponentToWorld = 0;
			DWORD StaticMesh = 0;
		}

		namespace SkinnedMeshComponent {
			DWORD CachedWorldSpaceBounds = 0;
		}
	}

	namespace FortniteGame {
		namespace FortPawn {
			DWORD bIsDBNO = 0;
			DWORD bIsDying = 0;
			DWORD CurrentWeapon = 0;
		}

		namespace FortPickup {
			DWORD PrimaryPickupItemEntry = 0;
		}

		namespace FortItemEntry {
			DWORD ItemDefinition = 0;
		}

		namespace FortItemDefinition {
			DWORD DisplayName = 0;
			DWORD Tier = 0;
		}

		namespace FortPlayerStateAthena {
			DWORD TeamIndex = 0;
		}

		namespace FortWeapon {
			DWORD WeaponData = 0;
			DWORD AmmoCount = 0;
		}

		namespace FortHoagieVehicle {
			DWORD BoostCooldown = 0;
		}

		namespace FortWeaponItemDefinition {
			DWORD WeaponStatHandle = 0;
		}

		namespace FortProjectileAthena {
			DWORD FireStartLoc = 0;
		}

		namespace FortBaseWeaponStats {
			DWORD ReloadTime = 0;
		}

		namespace BuildingContainer {
			DWORD bAlreadySearched = 0;
		}
	}

	namespace UI {
		namespace ItemCount {
			DWORD ItemDefinition = 0;
		}
	}

	BOOLEAN Initialize() {
		auto addr = Util::FindPattern(xorstr("\x48\x89\x05\x00\x00\x00\x00\x48\x8B\x4B\x78"), xorstr("xxx????xxxx"));
		if (!addr) {
			MessageBox(0, xorstr(L"Please contact an administrator and give the error code:\n0x08"), xorstr(L"Error"), 0);
			return FALSE;
		}

		uWorld = reinterpret_cast<decltype(uWorld)>(RELATIVE_ADDR(addr, 7));

		// SetControlRotation
		Engine::Controller::SetControlRotation = Util::FindObject(xorstr(L"/Script/Engine.Controller.SetControlRotation"));
		if (!Engine::Controller::SetControlRotation) {
			MessageBox(0, xorstr(L"Please contact an administrator and give the error code:\n0x09"), xorstr(L"Error"), 0);
			return FALSE;
		}

		// ClientSetRotation
		Engine::Controller::ClientSetRotation = Util::FindObject(xorstr(L"/Script/Engine.Controller.ClientSetRotation"));
		if (!Engine::Controller::ClientSetRotation) {
			MessageBox(0, xorstr(L"Please contact an administrator and give the error code:\n0x10"), xorstr(L"Error"), 0);
			return FALSE;
		}

		// GetPlayerName
		Engine::PlayerState::GetPlayerName = Util::FindObject(xorstr(L"/Script/Engine.PlayerState.GetPlayerName"));
		if (!Engine::PlayerState::GetPlayerName) {
			MessageBox(0, xorstr(L"Please contact an administrator and give the error code:\n0x11"), xorstr(L"Error"), 0);
			return FALSE;
		}

		Engine::World::Levels = 0x148;
		Engine::GameInstance::LocalPlayers = 0x38;
		Engine::World::OwningGameInstance = 0x190;
		Engine::Controller::ControlRotation = 0x290;
		Engine::PlayerController::PlayerCameraManager = 0x2C0;
		Engine::PlayerController::AcknowledgedPawn = 0x2B0;
		Engine::Pawn::PlayerState = 0x240;
		Engine::Actor::RootComponent = 0x138;
		Engine::Character::Mesh = 0x288;
		Engine::SceneComponent::RelativeLocation = 0x11C;
		Engine::SceneComponent::ComponentVelocity = 0x140;
		Engine::StaticMeshComponent::StaticMesh = 0x488;
		Engine::SkinnedMeshComponent::CachedWorldSpaceBounds = 0x638;
		Engine::Actor::CustomTimeDilation = 0x98;
		FortniteGame::FortPawn::bIsDBNO = 0x572;
		FortniteGame::FortPawn::bIsDying = 0x548;
		FortniteGame::FortPlayerStateAthena::TeamIndex = 0xF50;
		FortniteGame::FortPickup::PrimaryPickupItemEntry = 0x2A8;
		FortniteGame::FortItemDefinition::DisplayName = 0x88;
		FortniteGame::FortItemDefinition::Tier = 0x6C;
		FortniteGame::FortItemEntry::ItemDefinition = 0x18;
		FortniteGame::FortPawn::CurrentWeapon = 0x5F8;
		FortniteGame::FortWeapon::WeaponData = 0x380;
		FortniteGame::FortWeaponItemDefinition::WeaponStatHandle = 0x8B8;
		FortniteGame::FortProjectileAthena::FireStartLoc = 0x8C8;
		FortniteGame::FortBaseWeaponStats::ReloadTime = 0xFC;
		FortniteGame::BuildingContainer::bAlreadySearched = 0xE01; 


		return TRUE;



	}
}
