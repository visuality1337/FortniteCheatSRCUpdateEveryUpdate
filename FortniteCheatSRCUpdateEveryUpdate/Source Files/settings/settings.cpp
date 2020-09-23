/*

Visual#9999
*/

#include "../../Header Files/includes.h"
#include "../../Header Files/Config/config.h"

//config_system Settings = { 0 };

namespace SettingsHelper {
	VOID SaveSettings() {
		CHAR path[0xFF];
		GetTempPathA(sizeof(path) / sizeof(path[0]), path);
		strcat(path, ("fnambt.settings"));

		auto file = fopen(path, ("wb"));
		if (file) {
			fwrite(&config_system.item, sizeof(config_system.item), 1, file);
			fclose(file);
		}
	}

	VOID ResetSettings() {
		config_system.item = { 0 };
		config_system.item.Page = 1;
		config_system.item.AimPoint = 0;
		config_system.item.Aimbot = true;
		config_system.item.SilentAimbot = false;
		config_system.item.FlickAimbot = false;
		config_system.item.SpinBot = false;
		config_system.item.AutoAim = false;
		config_system.item.SpamAutoAim = false;
		config_system.item.TriggerAimbot = false;
		config_system.item.TriggerSpeed = 0;
		config_system.item.AutoAimbot = false;
		config_system.item.NoSpreadAimbot = false;
		config_system.item.RapidFire = false;
		config_system.item.BulletTP = false;
		config_system.item.ARTP = false;
		config_system.item.BoatRocketTP = false;
		config_system.item.RocketTP = false;
		config_system.item.BetterMap = false;
		config_system.item.ThirdPerson = true;
		config_system.item.AirStuck = false;
		config_system.item.AntiAim = false;
		config_system.item.FreeCamRotationLock = false;
		config_system.item.FreeCam = false;
		config_system.item.FreeCamSpeed = 1.00f;
		config_system.item.CheckVisible = false;
		config_system.item.AimbotFOV = 200.0f;
		config_system.item.AimbotSlow = 0.0f;
		config_system.item.InstantReload = false;
		config_system.item.StreamSnipe = false;
		config_system.item.FOVSlider = true;
		config_system.item.FOV = 100.0f;
		config_system.item.DrawAimbotFOV = true;
		config_system.item.DrawFilledAimbotFOV = true;
		config_system.item.CrosshairSize = 0.0f;
		config_system.item.CrosshairThickness = 0.0f;
		config_system.item.AimbotFOV = true;
		config_system.item.Players = true;
		config_system.item.PlayerBox = true;
		config_system.item.PlayersCorner = false;
		config_system.item.PlayerLines = true;
		config_system.item.ClosestLineESP = true;
		config_system.item.PlayerLinesLocation = 1;
		config_system.item.PlayerNames = true;
		config_system.item.BoxESPOpacity = 0.20f;
		config_system.item.FOVCircleOpacity = 1.00f;
		config_system.item.FOVCircleFilledOpacity = 0.20f;
		config_system.item.PlayerVisibleColor[0] = 0.0f;
		config_system.item.PlayerVisibleColor[1] = 0.0f;
		config_system.item.PlayerVisibleColor[2] = 1.0f;
		config_system.item.PlayerNotVisibleColor[0] = 0.0f;
		config_system.item.PlayerNotVisibleColor[1] = 0.0f;
		config_system.item.PlayerNotVisibleColor[2] = 0.0f;
		config_system.item.PlayerTeammate[0] = 0.0f;
		config_system.item.PlayerTeammate[1] = 0.75f;
		config_system.item.PlayerTeammate[2] = 0.0f;
		config_system.item.FOVCircleColor[0] = 0.0f;
		config_system.item.FOVCircleColor[1] = 0.20f;
		config_system.item.FOVCircleColor[2] = 0.0f;
		config_system.item.BoxESP[0] = 0.0f;
		config_system.item.BoxESP[1] = 0.0f;
		config_system.item.BoxESP[2] = 0.0f;
		config_system.item.LineESP[0] = 0.46f;
		config_system.item.LineESP[1] = 0.0f;
		config_system.item.LineESP[2] = 0.0f;
		config_system.item.Ammo = true;
		config_system.item.Chest = true;
		config_system.item.Llama = true;
		config_system.item.boat = true;
		config_system.item.chopper = true;
		config_system.item.Extra = true;

		// keybinds
		config_system.keybind.Menu = 0x2D;
		config_system.keybind.AimbotLock = 0x02;
		config_system.keybind.AimbotShoot = 0x01;
		config_system.keybind.AntiAim = 0x01;
		config_system.keybind.Spinbot = 0x14;
		config_system.keybind.Freecam = 0;
		config_system.keybind.StreamSnipe = 0;
		config_system.keybind.ThirdPerson = 0;
		config_system.keybind.Airstuck1 = 0;
		config_system.keybind.Airstuck2 = 0;

		SaveSettings();
	}

	VOID Initialize() {
		CHAR path[0xFF] = { 0 };
		GetTempPathA(sizeof(path) / sizeof(path[0]), path);
		strcat(path, ("fnambt.settings"));

		auto file = fopen(path, ("rb"));
		if (file) {
			fseek(file, 0, SEEK_END);
			auto size = ftell(file);

			if (size == sizeof(config_system.item)) {
				fseek(file, 0, SEEK_SET);
				fread(&config_system.item, sizeof(config_system.item), 1, file);
				fclose(file);
			}
			else {
				fclose(file);
				ResetSettings();
			}
		}
		else {
			ResetSettings();
		}
	}
}
