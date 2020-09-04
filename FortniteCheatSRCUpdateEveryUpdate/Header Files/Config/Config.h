/*

Visual#9999
*/
#pragma once

#include <filesystem>

class c_config final {
public:
	void run(const char*) noexcept;
	void load(size_t) noexcept;
	void save(size_t) const noexcept;
	void add(const char*) noexcept;
	void remove(size_t) noexcept;
	void rename(size_t, const char*) noexcept;
	void reset() noexcept;

	constexpr auto& get_configs() noexcept {
		return configs;
	}

	struct {
		// item

		bool Players;
		bool Chest;
		bool PlayerBox;
		bool PlayersCorner;
		bool Llama;
		bool PlayerLines;
		bool ClosestLineESP;
		int PlayerLinesLocation;
		bool PlayerNames;
		float PlayerVisibleColor[3];
		float PlayerNotVisibleColor[3];
		bool kazoisgay;
		float PlayerTeammate[3];
		float FOVCircleColor[3];
		float BoxESP[3];
		float BoxESPOpacity;
		float FOVCircleOpacity;
		float FOVCircleFilledOpacity;
		float LineESP[3];
		bool Ammo;
		bool boat;
		bool chopper;
		bool Weapons;
		bool Items;
		int MinWeaponTier;
		int Page;
		int AimPoint;
		bool Aimbot;
		bool AutoAimbot;
		bool SilentAimbot;
		bool FlickAimbot;
		bool SpinBot;
		bool AutoAim;
		bool SpamAutoAim;
		bool TriggerAimbot;
		int TriggerSpeed;
		bool NoSpreadAimbot;
		bool AntiAim;
		bool BetterMap;
		bool AirStuck;
		bool ThirdPerson;
		bool FreeCam;
		float FreeCamSpeed;
		bool CheckVisible;
		char StreamSnipeName;
		bool StreamSnipe;
		bool DrawAimbotFOV;
		bool DrawFilledAimbotFOV;
		float CrosshairSize;
		float MenuColor[3] = { 1.f, 0.f, 0.f };
		float CrosshairThickness;
		float AimbotFOV;
		float AimbotSlow;
		bool RapidFire;
		bool BulletTP;
		bool ARTP;
		bool fastmode420;
		bool BoatRocketTP;
		bool RocketTP;
		bool FreeCamRotationLock;
		bool InstantReload;
		bool FOVSlider;
		float FOV;
		bool Extra;

	} item;

	struct {
		//keybind

		int Menu = 0x2D;
		int AimbotLock = 0x02;
		int AimbotShoot = 0x01;
		int AntiAim = 0x01;
		int Spinbot = 0x14;
		int Freecam = 0;
		int StreamSnipe = 0;
		int ThirdPerson = 0;
		int Airstuck1 = 0;
		int Airstuck2 = 0;
		int BetterMap = 0;

	} keybind;

private:
	std::filesystem::path path;
	std::vector<std::string> configs;
};

extern c_config config_system;