/*
lovefn is pasted shit
lovefn saying its not pasted here is proof kek
cracked

Visual#9999
*/

#include "../../Header Files/includes.h"
namespace Offsets {
	PVOID* uWorld = 0;

	namespace Engine {
		namespace World {
			DWORD OwningGameInstance = 0;
			DWORD Levels = 0;
		}

		namespace Level {
			DWORD AActors = 0x98;
		}

		namespace GameInstance {
			DWORD LocalPlayers = 0;
		}

		namespace Player {
			DWORD PlayerController = 0x30;
		}

		namespace Controller {
			DWORD ControlRotation = 0x280;
			PVOID SetControlRotation = 0;
			PVOID ClientSetRotation;
		}

		namespace PlayerController {
			DWORD AcknowledgedPawn = 0x298;
			DWORD PlayerCameraManager = 0x2B0;
		}

		namespace Pawn {
			DWORD PlayerState = 0x238;
		}

		namespace PlayerState {
			PVOID GetPlayerName = 0;
		}

		namespace Actor {
			DWORD RootComponent = 0x130;
		}

		namespace Character {
			DWORD Mesh = 0x278;
		}

		namespace SceneComponent {
			DWORD RelativeLocation = 0x11C;
			DWORD ComponentVelocity = 0x140;
		}

		namespace StaticMeshComponent {
			DWORD ComponentToWorld = 0x1C0;
			DWORD StaticMesh = 0x420;
		}

		namespace SkinnedMeshComponent {
			DWORD CachedWorldSpaceBounds = 0x5A0;
		}
	}

	namespace FortniteGame {
		namespace FortPawn {
			DWORD bIsDBNO = 0x53A;
			DWORD bIsDying = 0x520;
			DWORD CurrentWeapon = 0x588;
		}

		namespace FortPickup {
			DWORD PrimaryPickupItemEntry = 0x280;
		}

		namespace FortItemEntry {
			DWORD ItemDefinition = 0x18;
		}

		namespace FortItemDefinition {
			DWORD DisplayName = 0x70;
			DWORD Tier = 0x54;
		}

		namespace FortPlayerStateAthena {
			DWORD TeamIndex = 0xE60;
		}

		namespace FortWeapon {
			DWORD WeaponData = 0x358;
		}

		namespace FortHoagieVehicle {
			DWORD BoostCooldown = 0x105C;
		}

		namespace FortWeaponItemDefinition {
			DWORD WeaponStatHandle = 0x7B8;
		}

		namespace FortProjectileAthena {
			DWORD FireStartLoc = 0x850;
		}

		namespace FortBaseWeaponStats {
			DWORD ReloadTime = 0;
		}

		namespace BuildingContainer {
			DWORD bAlreadySearched = 0xC59;
		}
	}

	namespace UI {
		namespace ItemCount {
			DWORD ItemDefinition = 0;
		}
	}

	BOOLEAN Initialize() {
		auto addr = Util::FindPattern("\x48\x8B\x1D\x00\x00\x00\x00\x48\x85\xDB\x74\x3B\x41", "xxx????xxxxxx");
		if (!addr) {
			MessageBox(0, L"Please contact an administrator and give the error code:\n0x08", L"Error", 0);
			return FALSE;
		}

		uWorld = reinterpret_cast<decltype(uWorld)>(RELATIVE_ADDR(addr, 7));

		// SetControlRotation
		Engine::Controller::SetControlRotation = Util::FindObject(L"/Script/Engine.Controller.SetControlRotation");
		if (!Engine::Controller::SetControlRotation) {
			MessageBox(0, L"Please contact an administrator and give the error code:\n0x09", L"Error", 0);
			return FALSE;
		}

		// ClientSetRotation
		Engine::Controller::ClientSetRotation = Util::FindObject(L"/Script/Engine.Controller.ClientSetRotation");
		if (!Engine::Controller::ClientSetRotation) {
			MessageBox(0, L"Please contact an administrator and give the error code:\n0x10", L"Error", 0);
			return FALSE;
		}

		// GetPlayerName
		Engine::PlayerState::GetPlayerName = Util::FindObject(L"/Script/Engine.PlayerState.GetPlayerName");
		if (!Engine::PlayerState::GetPlayerName) {
			MessageBox(0, L"Please contact an administrator and give the error code:\n0x11", L"Error", 0);
			return FALSE;
		}


		Engine::World::OwningGameInstance = 0x190;			 // leaked by YTMcGamer#1337
		Engine::World::Levels = 0x148;						 // leaked by YTMcGamer#1337
		Engine::GameInstance::LocalPlayers = 0x38;			 // leaked by YTMcGamer#1337
		Engine::Player::PlayerController = 0x30;			 // leaked by YTMcGamer#1337
		Engine::PlayerController::AcknowledgedPawn = 0x2A0;  // leaked by YTMcGamer#1337
		Engine::Controller::ControlRotation = 0x288;		 // leaked by YTMcGamer#1337
		Engine::Pawn::PlayerState = 0x240;					 // leaked by YTMcGamer#1337
		Engine::Actor::RootComponent = 0x130;				 // leaked by YTMcGamer#1337
		Engine::Character::Mesh = 0x280;					 // leaked by YTMcGamer#1337
		Engine::SceneComponent::RelativeLocation = 0x11C;    // leaked by YTMcGamer#1337
		Engine::SceneComponent::ComponentVelocity = 0x140;   // leaked by YTMcGamer#1337
		Engine::StaticMeshComponent::StaticMesh = 0x468;     //leaked by YTMcGamer#1337
		Engine::SkinnedMeshComponent::CachedWorldSpaceBounds = 0x600;// leaked by YTMcGamer#1337
		FortniteGame::FortPawn::bIsDBNO = 0x552;			// leaked by YTMcGamer#1337
		FortniteGame::FortPawn::bIsDying = 0x538;           // leaked by YTMcGamer#1337
		FortniteGame::FortPlayerStateAthena::TeamIndex = 0xe68; //0xe68
		FortniteGame::FortPickup::PrimaryPickupItemEntry = 0x298; // leaked by YTMcGamer#1337
		FortniteGame::FortItemDefinition::DisplayName = 0x70; // leaked by YTMcGamer#1337
		FortniteGame::FortItemDefinition::Tier = 0x54; // leaked by YTMcGamer#1337
		FortniteGame::FortItemEntry::ItemDefinition = 0x18;// leaked by YTMcGamer#1337
		FortniteGame::FortPawn::CurrentWeapon = 0x5A0;// leaked by YTMcGamer#1337
		FortniteGame::FortWeapon::WeaponData = 0x378;// leaked by YTMcGamer#1337
		FortniteGame::FortWeaponItemDefinition::WeaponStatHandle = 0x7B8;// leaked by YTMcGamer#1337
		FortniteGame::FortProjectileAthena::FireStartLoc = 0x880;// leaked by YTMcGamer#1337
		FortniteGame::FortBaseWeaponStats::ReloadTime = 0xfc;// leaked by YTMcGamer#1337
		FortniteGame::BuildingContainer::bAlreadySearched = 0xc71;// leaked by YTMcGamer#1337



		return TRUE;



	}
}
