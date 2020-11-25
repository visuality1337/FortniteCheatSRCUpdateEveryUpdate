/*

Visual#9999
*/

#include "../../Header Files/menu/menu.h"
#include "../../Header Files/includes.h"
#include "../../Header Files/Config/config.h"
#include "../../DiscordHook/Discord.h"
#include "../../Helper/Helper.h"
#include <iostream>
ID3D11Device* device = nullptr;
ID3D11DeviceContext* immediateContext = nullptr;
ID3D11RenderTargetView* renderTargetView = nullptr;

HRESULT(*PresentOriginal)(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) = nullptr;
HRESULT(*ResizeOriginal)(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) = nullptr;
WNDPROC oWndProc;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
static bool ShowMenu = true;

void ToggleButton(const char* str_id, bool* v)
{
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float height = 16.0f;
	float width = height * 1.60f;
	float radius = height * 0.50f;

	ImGui::InvisibleButton(str_id, ImVec2(width, height));
	if (ImGui::IsItemClicked())
		*v = !*v;

	float t = *v ? 1.0f : 0.0f;

	ImGuiContext& g = *GImGui;
	float ANIM_SPEED = 0.20f;
	if (g.LastActiveId == g.CurrentWindow->GetID(str_id))
	{
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = *v ? (t_anim) : (1.0f - t_anim);
	}

	ImU32 col_bg;
	if (ImGui::IsItemHovered())
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.0f, 0.0f, 0.80f, 1.0f), ImVec4(0.0f, 0.0f, 0.80f, 1.0f), t));
	else
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.0f, 0.0f, 1.0f), ImVec4(0.0f, 0.85f, 0.0f, 1.0f), t));

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
	draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius + 0.80f, IM_COL32(255, 255, 255, 255));
}


VOID AddMarker(ImGuiWindow& window, float width, float height, float* start, PVOID pawn, LPCSTR text, ImU32 color) {
	auto root = Util::GetPawnRootLocation(pawn);
	if (root) {
		auto pos = *root;
		float dx = start[0] - pos.X;
		float dy = start[1] - pos.Y;
		float dz = start[2] - pos.Z;

		if (Util::WorldToScreen(width, height, &pos.X)) {
			float dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy + dz * dz) / 1000.0f;

			CHAR modified[0xFF] = { 0 };
			snprintf(modified, sizeof(modified), ("%s\n| %dm |"), text, static_cast<INT>(dist));

			auto size = ImGui::GetFont()->CalcTextSizeA(window.DrawList->_Data->FontSize, FLT_MAX, 0, modified);
			window.DrawList->AddText(ImVec2(pos.X - size.x / 2.0f, pos.Y - size.y / 2.0f), color, modified);
		}
	}
}

FLOAT GetDistance(ImGuiWindow& window, float width, float height, float* start, PVOID pawn) {
	auto root = Util::GetPawnRootLocation(pawn);
	float dist;
	if (root) {
		auto pos = *root;
		float dx = start[0] - pos.X;
		float dy = start[1] - pos.Y;
		float dz = start[2] - pos.Z;

		if (Util::WorldToScreen(width, height, &pos.X)) {
			dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy + dz * dz) / 1000.0f;
			return dist;
		}
	}

}

__declspec(dllexport) LRESULT CALLBACK WndProcHook(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_KEYUP && (wParam == config_system.keybind.Menu || (ShowMenu && wParam == VK_ESCAPE))) {
		ShowMenu = !ShowMenu;
		ImGui::GetIO().MouseDrawCursor = ShowMenu;

	}
	else if (msg == WM_QUIT && ShowMenu) {
		ExitProcess(0);
	}

	if (ShowMenu) {
		ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
		return TRUE;
	}

	return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
}

extern uint64_t base_address = 0;
DWORD processID;
const ImVec4 color = { 255.0,255.0,255.0,1 };
const ImVec4 red = { 0.65,0,0,1 };
const ImVec4 white = { 255.0,255.0,255.0,1 };
const ImVec4 green = { 0.03,0.81,0.14,1 };
const ImVec4 blue = { 0.21960784313,0.56470588235,0.90980392156,1.0 };

ImGuiWindow& BeginScene() {
	ImGui_ImplDX11_NewFrame();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin(("##scene"), nullptr, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);

	auto& io = ImGui::GetIO();
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);

	return *ImGui::GetCurrentWindow();
}

char streamsnipena[256] = "Username";

VOID EndScene(ImGuiWindow& window) {
	window.DrawList->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.17f, 0.18f, 0.2f, 1.0f));

	if (ShowMenu) {

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f);
		colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(1.00f, 0.00f, 0.00f, 0.50f);
		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.41f, 0.00f, 0.03f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.48f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_CheckMark] = ImVec4(0.75f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_Header] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.00f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
		colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);

		ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5, 0.5);
		ImGui::GetStyle().FramePadding = ImVec2(4.0, 2.0);
		ImGui::GetStyle().WindowRounding = 0;
		ImGui::GetStyle().GrabMinSize = 12;
		ImGui::GetStyle().ScrollbarSize = 14;
		ImGui::GetStyle().ScrollbarRounding = 0;
		ImGui::GetStyle().FrameRounding = 4.0f;


			ImGui::Begin("github.com/Visual9999", 0, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize); {
			ImGui::SetWindowSize(ImVec2(550, 450), ImGuiCond_FirstUseEver);

			ImVec2 size = ImGui::GetItemRectSize();

			if (ImGui::CollapsingHeader("Aimbot"))
			{
				ImGui::Text("Memory");
				ToggleButton("Memory", &config_system.item.Aimbot);
				if (config_system.item.Aimbot)
				{
					ImGui::Text("Silent Aimbot");
					ToggleButton("Silent", &config_system.item.SilentAimbot);
					ImGui::Text("Trigger Aimbot");
					ToggleButton("Trigger", &config_system.item.TriggerAimbot);
				}
				const char* Points[] = { " Head", " Chest", " Pelvis" };
				static int AimPoint = 0;
				ImGui::Combo("Aim Point", &AimPoint, Points, IM_ARRAYSIZE(Points));
				config_system.item.AimPoint = AimPoint;
				if (config_system.item.AutoAimbot) {
					config_system.item.AutoAimbot = 2000.0f;
				}
				ImGui::Text("Fov");
				ToggleButton("Fov", &config_system.item.DrawAimbotFOV);
				if (config_system.item.DrawAimbotFOV)
				{
					ImGui::Text("Filled FOV");
					ToggleButton("Filled", &config_system.item.DrawFilledAimbotFOV);
				}
				ImGui::SliderFloat(("Circle FOV"), &config_system.item.AimbotFOV, 100.0f, 1000.0f);
			}

			if (ImGui::CollapsingHeader("Misc"))
			{
				ImGui::Text("Spin Bot [CAPSLOCK]");
				ToggleButton("Spin", &config_system.item.SpinBot);

				ImGui::Text("Camera FOV");
				ToggleButton("Camera", &config_system.item.FOVSlider);

				if (config_system.item.FOVSlider)
				{
					ImGui::SliderFloat(("Camera FOV Slider"), &config_system.item.FOV, 0.0f, 150.0f, ("%.2f"));
				}
				ImGui::Text("Stream Snipe Player Name");
				ImGui::InputText("       ", streamsnipena, 256, ImGuiInputTextFlags_EnterReturnsTrue);
			}

			if (ImGui::CollapsingHeader("ESP"))
			{
				ImGui::Text("Chest ESP");
				ToggleButton("Chest", &config_system.item.Chest);
				ImGui::Text("Llama ESP");
				ToggleButton("Llama", &config_system.item.Llama);
				ImGui::Text("Ammo ESP");
				ToggleButton("Ammo", &config_system.item.Ammo);
				ImGui::Text("Boat ESP");
				ToggleButton("Boat", &config_system.item.boat);
				ImGui::Text("Chopper ESP");
				ToggleButton("Chopper", &config_system.item.chopper);
				ImGui::Text("Player Names ESP");
				ToggleButton("Names", &config_system.item.PlayerNames);
				ImGui::Text("Box ESP");
				ToggleButton("BOX", &config_system.item.PlayerBox);
				ImGui::Text("Player Lines");
				ToggleButton("Lines", &config_system.item.PlayerLines);
				ImGui::Text("Players Corner");
				ToggleButton("Corner", &config_system.item.PlayersCorner);
			}

			if (ImGui::CollapsingHeader("Colors"))
			{
				ImGui::PushItemWidth(100.0f);
				ImGui::Text("ESP");
				ImGui::SameLine();
				ImGui::ColorPicker3(("Box"), config_system.item.BoxESP, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				ImGui::ColorPicker3(("Lines"), config_system.item.LineESP, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				ImGui::ColorPicker3(("Aim\nFOV"), config_system.item.FOVCircleColor, ImGuiColorEditFlags_NoInputs);
				ImGui::Text("Skeleton ESP");
				ImGui::ColorPicker3(("Is\nVisible"), config_system.item.PlayerVisibleColor, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				ImGui::ColorPicker3(("Not\nVisible"), config_system.item.PlayerNotVisibleColor, ImGuiColorEditFlags_NoInputs);
				ImGui::SameLine();
				ImGui::ColorPicker3(("Teammates"), config_system.item.PlayerTeammate, ImGuiColorEditFlags_NoInputs);
			}
		}
		ImGui::End();
	}

	ImGui::PopStyleColor();

	ImGui::Render();
}

VOID AddLine(ImGuiWindow& window, float width, float height, float a[3], float b[3], ImU32 color, float& minX, float& maxX, float& minY, float& maxY) {
	float ac[3] = { a[0], a[1], a[2] };
	float bc[3] = { b[0], b[1], b[2] };
	if (Util::WorldToScreen(width, height, ac) && Util::WorldToScreen(width, height, bc)) {
		window.DrawList->AddLine(ImVec2(ac[0], ac[1]), ImVec2(bc[0], bc[1]), color, 2.0f);

		minX = min(ac[0], minX);
		minX = min(bc[0], minX);

		maxX = max(ac[0], maxX);
		maxX = max(bc[0], maxX);

		minY = min(ac[1], minY);
		minY = min(bc[1], minY);

		maxY = max(ac[1], maxY);
		maxY = max(bc[1], maxY);
	}
}


__declspec(dllexport) HRESULT PresentHook(IDXGISwapChain* swapChain, UINT syncInterval, UINT flags) {
	static float width = 0;
	static float height = 0;
	static HWND hWnd = 0;
	if (!device) {
		swapChain->GetDevice(__uuidof(device), reinterpret_cast<PVOID*>(&device));
		device->GetImmediateContext(&immediateContext);

		ID3D11Texture2D* renderTarget = nullptr;
		swapChain->GetBuffer(0, __uuidof(renderTarget), reinterpret_cast<PVOID*>(&renderTarget));
		device->CreateRenderTargetView(renderTarget, nullptr, &renderTargetView);
		renderTarget->Release();

		ID3D11Texture2D* backBuffer = 0;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (PVOID*)&backBuffer);
		D3D11_TEXTURE2D_DESC backBufferDesc = { 0 };
		backBuffer->GetDesc(&backBufferDesc);

		hWnd = FindWindow((L"UnrealWindow"), (L"Fortnite  "));
		if (!width) {
			oWndProc = reinterpret_cast<WNDPROC>(SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WndProcHook)));
		}

		width = (float)backBufferDesc.Width;
		height = (float)backBufferDesc.Height;
		backBuffer->Release();

		ImGui::GetIO().Fonts->AddFontFromFileTTF(("C:\\Windows\\Fonts\\trebucbd.ttf"), 13.0f);

		ImGui_ImplDX11_Init(hWnd, device, immediateContext);
		ImGui_ImplDX11_CreateDeviceObjects();
	}
	immediateContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
	////// reading
	auto& window = BeginScene();
	////// readin
	auto success = FALSE;
	do {
		float closestDistance = FLT_MAX;
		PVOID closestPawn = NULL;

		auto world = *Offsets::uWorld;
		if (!world) break;

		auto gameInstance = ReadPointer(world, Offsets::Engine::World::OwningGameInstance);
		if (!gameInstance) break;

		auto localPlayers = ReadPointer(gameInstance, Offsets::Engine::GameInstance::LocalPlayers);
		if (!localPlayers) break;

		auto localPlayer = ReadPointer(localPlayers, 0);
		if (!localPlayer) break;

		auto localPlayerController = ReadPointer(localPlayer, Offsets::Engine::Player::PlayerController);
		if (!localPlayerController) break;

		auto localPlayerPawn = reinterpret_cast<UObject*>(ReadPointer(localPlayerController, Offsets::Engine::PlayerController::AcknowledgedPawn));
		if (!localPlayerPawn) break;

		auto localPlayerWeapon = ReadPointer(localPlayerPawn, Offsets::FortniteGame::FortPawn::CurrentWeapon);
		if (!localPlayerWeapon) break;

		auto localPlayerRoot = ReadPointer(localPlayerPawn, Offsets::Engine::Actor::RootComponent);
		if (!localPlayerRoot) break;

		auto localPlayerState = ReadPointer(localPlayerPawn, Offsets::Engine::Pawn::PlayerState);
		if (!localPlayerState) break;

		auto localPlayerLocation = reinterpret_cast<float*>(reinterpret_cast<PBYTE>(localPlayerRoot) + Offsets::Engine::SceneComponent::RelativeLocation);
		auto localPlayerTeamIndex = ReadDWORD(localPlayerState, Offsets::FortniteGame::FortPlayerStateAthena::TeamIndex);

		auto weaponName = Util::GetObjectFirstName((UObject*)localPlayerWeapon);
		auto isProjectileWeapon = wcsstr(weaponName.c_str(), L"Rifle_Sniper");

		Core::LocalPlayerPawn = localPlayerPawn;
		Core::LocalPlayerController = localPlayerController;


		std::vector<PVOID> playerPawns;
		for (auto li = 0UL; li < ReadDWORD(world, Offsets::Engine::World::Levels + sizeof(PVOID)); ++li) {
			auto levels = ReadPointer(world, 0x138); //keks
			if (!levels) break;

			auto level = ReadPointer(levels, li * sizeof(PVOID));
			if (!level) continue;

			for (auto ai = 0UL; ai < ReadDWORD(level, Offsets::Engine::Level::AActors + sizeof(PVOID)); ++ai) {
				auto actors = ReadPointer(level, Offsets::Engine::Level::AActors);
				if (!actors) break;

				auto pawn = reinterpret_cast<UObject*>(ReadPointer(actors, ai * sizeof(PVOID)));
				if (!pawn || pawn == localPlayerPawn) continue;

				auto name = Util::GetObjectFirstName(pawn);
				if (wcsstr(name.c_str(), L"PlayerPawn_Athena_C") || wcsstr(name.c_str(), L"PlayerPawn_Athena_Phoebe_C") || wcsstr(name.c_str(), L"BP_MangPlayerPawn") || wcsstr(name.c_str(), L"HoagieVehicle_C")) {
					playerPawns.push_back(pawn);
				}
				
				else if (config_system.item.Llama && wcsstr(name.c_str(), L"AthenaSupplyDrop_Llama")) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "Llama", ImGui::GetColorU32({ 0.03f, 0.78f, 0.91f, 1.0f }));
				}
				if (config_system.item.Chest && wcsstr(name.c_str(), L"Tiered_Chest") && !((ReadBYTE(pawn, Offsets::FortniteGame::BuildingContainer::bAlreadySearched) >> 7) & 1)) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "Chest", ImGui::GetColorU32({ 255,255,0,255 }));
				}
				else if (config_system.item.Ammo && wcsstr(name.c_str(), L"Tiered_Ammo") && !((ReadBYTE(pawn, Offsets::FortniteGame::BuildingContainer::bAlreadySearched) >> 7) & 1)) {
					AddMarker(window, width, height, localPlayerLocation, pawn, "Ammo Box", ImGui::GetColorU32({ 0.75f, 0.75f, 0.75f, 1.0f }));
				}
				else if (config_system.item.chopper && wcsstr(name.c_str(), L"HoagieVehicle_C")) {

					AddMarker(window, width, height, localPlayerLocation, pawn, "Chopper", ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f }));
				}
				else if (config_system.item.boat && wcsstr(name.c_str(), L"MeatballVehicle_L")) {

					AddMarker(window, width, height, localPlayerLocation, pawn, "Boat", ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f }));
				}
			}
		}

		float CurrentAimPointer[3] = { 0 };
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
		else if (config_system.item.AimPoint == 8) { // automatic

		}

		for (auto pawn : playerPawns)
		{
			auto state = ReadPointer(pawn, Offsets::Engine::Pawn::PlayerState);
			if (!state) continue;

			auto mesh = ReadPointer(pawn, Offsets::Engine::Character::Mesh);
			if (!mesh) continue;

			auto bones = ReadPointer(mesh, Offsets::Engine::StaticMeshComponent::StaticMesh);
			if (!bones) bones = ReadPointer(mesh, Offsets::Engine::StaticMeshComponent::StaticMesh + 0x10);
			if (!bones) continue;
			float compMatrix[4][4] = { 0 };
			Util::ToMatrixWithScale(reinterpret_cast<float*>(reinterpret_cast<PBYTE>(mesh) + 0x1C0), compMatrix);

			// Top
			float head[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 66, head);

			float neck[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 65, neck);

			float chest[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 36, chest);

			float pelvis[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 2, pelvis);

			// Arms
			float leftShoulder[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 9, leftShoulder);

			float rightShoulder[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 62, rightShoulder);

			float leftElbow[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 10, leftElbow);

			float rightElbow[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 38, rightElbow);

			float leftHand[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 11, leftHand);

			float rightHand[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 39, rightHand);

			// Legs
			float leftLeg[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 67, leftLeg);

			float rightLeg[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 74, rightLeg);

			float leftThigh[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 73, leftThigh);

			float rightThigh[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 80, rightThigh);

			float leftFoot[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 68, leftFoot);

			float rightFoot[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 75, rightFoot);

			float leftFeet[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 71, leftFeet);

			float rightFeet[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 78, rightFeet);

			float leftFeetFinger[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 72, leftFeetFinger);

			float rightFeetFinger[3] = { 0 };
			Util::GetBoneLocation(compMatrix, bones, 79, rightFeetFinger);

			Util::GetBoneLocation(compMatrix, bones, AimPointer, CurrentAimPointer);

			auto color = ImGui::GetColorU32({ config_system.item.PlayerNotVisibleColor[0], config_system.item.PlayerNotVisibleColor[1], config_system.item.PlayerNotVisibleColor[2], 1.0f });
			FVector viewPoint = { 0 };


			if (ReadDWORD(state, 0xE60) == localPlayerTeamIndex) {
				color = ImGui::GetColorU32({ config_system.item.PlayerTeammate[0], config_system.item.PlayerTeammate[1], config_system.item.PlayerTeammate[2], 1.0f });
			}
			else if (!config_system.item.CheckVisible) {
				auto w2s = *reinterpret_cast<FVector*>(CurrentAimPointer);
				if (Util::WorldToScreen(width, height, &w2s.X)) {
					auto dx = w2s.X - (width / 2);
					auto dy = w2s.Y - (height / 2);
					auto dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy);
					if (dist < config_system.item.AimbotFOV && dist < closestDistance) {
						closestDistance = dist;
						closestPawn = pawn;
					}
				}
			}
			else if ((ReadBYTE(pawn, Offsets::FortniteGame::FortPawn::bIsDBNO) & 1) && (isProjectileWeapon || Util::LineOfSightTo(localPlayerController, pawn, &viewPoint))) {
				color = ImGui::GetColorU32({ config_system.item.PlayerVisibleColor[0], config_system.item.PlayerVisibleColor[1], config_system.item.PlayerVisibleColor[2], 1.0f });
				if (config_system.item.AutoAimbot) {
					if (config_system.item.AimPoint = 8) {
						Util::GetBoneLocation(compMatrix, bones, BONE_HEAD_ID, CurrentAimPointer);
						auto dx = CurrentAimPointer[0] - localPlayerLocation[0];
						auto dy = CurrentAimPointer[1] - localPlayerLocation[1];
						auto dz = CurrentAimPointer[2] - localPlayerLocation[2];
						auto dist = dx * dx + dy * dy + dz * dz;
						if (dist < closestDistance) {
							closestDistance = dist;
							closestPawn = pawn;
						}
						else {
							Util::GetBoneLocation(compMatrix, bones, BONE_NECK_ID, CurrentAimPointer);
							auto dx = CurrentAimPointer[0] - localPlayerLocation[0];
							auto dy = CurrentAimPointer[1] - localPlayerLocation[1];
							auto dz = CurrentAimPointer[2] - localPlayerLocation[2];
							auto dist = dx * dx + dy * dy + dz * dz;
							if (dist < closestDistance) {
								closestDistance = dist;
								closestPawn = pawn;
							}
							else {
								Util::GetBoneLocation(compMatrix, bones, BONE_CHEST_ID, CurrentAimPointer);
								auto dx = CurrentAimPointer[0] - localPlayerLocation[0];
								auto dy = CurrentAimPointer[1] - localPlayerLocation[1];
								auto dz = CurrentAimPointer[2] - localPlayerLocation[2];
								auto dist = dx * dx + dy * dy + dz * dz;
								if (dist < closestDistance) {
									closestDistance = dist;
									closestPawn = pawn;
								}
								else {
									Util::GetBoneLocation(compMatrix, bones, BONE_PELVIS_ID, CurrentAimPointer);
									auto dx = CurrentAimPointer[0] - localPlayerLocation[0];
									auto dy = CurrentAimPointer[1] - localPlayerLocation[1];
									auto dz = CurrentAimPointer[2] - localPlayerLocation[2];
									auto dist = dx * dx + dy * dy + dz * dz;
									if (dist < closestDistance) {
										closestDistance = dist;
										closestPawn = pawn;
									}
								}
							}
						}
					}
					else {
						auto dx = CurrentAimPointer[0] - localPlayerLocation[0];
						auto dy = CurrentAimPointer[1] - localPlayerLocation[1];
						auto dz = CurrentAimPointer[2] - localPlayerLocation[2];
						auto dist = dx * dx + dy * dy + dz * dz;
						if (dist < closestDistance) {
							closestDistance = dist;
							closestPawn = pawn;
						}
					}

				}
				else
				{
					auto w2s = *reinterpret_cast<FVector*>(CurrentAimPointer);
					if (Util::WorldToScreen(width, height, &w2s.X)) {
						auto dx = w2s.X - (width / 2);
						auto dy = w2s.Y - (height / 2);
						auto dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy);
						if (dist < config_system.item.AimbotFOV && dist < closestDistance) {
							closestDistance = dist;
							closestPawn = pawn;
						}
					}
				}
			}



			//if (!config_system.item.Players) continue;

			if (config_system.item.PlayerLines) {
				auto end = *reinterpret_cast<FVector*>(CurrentAimPointer);
				if (Util::WorldToScreen(width, height, &end.X)) {
					if (ReadDWORD(state, 0xE60) != localPlayerTeamIndex) {
						if (config_system.item.LineESP) {
							window.DrawList->AddLine(ImVec2(width / 2, height / 2), ImVec2(end.X, end.Y), ImGui::GetColorU32({ config_system.item.LineESP[0], config_system.item.LineESP[1], config_system.item.LineESP[2], 1.0f }));
						}
					}
				}
			}

			float minX = FLT_MAX;
			float maxX = -FLT_MAX;
			float minY = FLT_MAX;
			float maxY = -FLT_MAX;

			if (config_system.item.Players) {
				AddLine(window, width, height, head, neck, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, neck, pelvis, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, chest, leftShoulder, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, chest, rightShoulder, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, leftShoulder, leftElbow, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, rightShoulder, rightElbow, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, leftElbow, leftHand, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, rightElbow, rightHand, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, pelvis, leftLeg, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, pelvis, rightLeg, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, leftLeg, leftThigh, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, rightLeg, rightThigh, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, leftThigh, leftFoot, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, rightThigh, rightFoot, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, leftFoot, leftFeet, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, rightFoot, rightFeet, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, leftFeet, leftFeetFinger, color, minX, maxX, minY, maxY);
				AddLine(window, width, height, rightFeet, rightFeetFinger, color, minX, maxX, minY, maxY);
				
			}

			/*float dist;
			if (dist >= 100)
				dist = 75;*/

			if (minX < width && maxX > 0 && minY < height && maxY > 0) {

				//ALL CORRECT
				auto topLeft = ImVec2(minX - 3.0f, minY - 3.0f);
				auto bottomRight = ImVec2(maxX + 3.0f, maxY);
				auto topRight = ImVec2(maxX + 3.0f, minY - 3.0f);
				auto bottomLeft = ImVec2(minX - 3.0f, maxY);


				if (config_system.item.PlayerBox) {

					window.DrawList->AddRectFilled(topLeft, bottomRight, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 0.70f }));
					window.DrawList->AddRect(topLeft, bottomRight, color, 0.2, 0, 0.1f);
				}
				
				else if (config_system.item.PlayersCorner) {
					auto bottomRightLEFT = ImVec2(maxX - 7.f + 75, maxY + 2.5f);
					auto bottomRightUP = ImVec2(maxX + 3.0f, maxY - 7.f + 75);
					auto topRight = ImVec2(maxX + 3.0f, minY - 3.0f);
					auto topRightLEFT = ImVec2(maxX - 7.f + 75, minY - 3.0f);
					auto topRightDOWN = ImVec2(maxX + 3.0f, minY + 7.f - 75);

					auto bottomLeft = ImVec2(minX - 3.0f, maxY + 3.f);
					auto bottomLeftRIGHT = ImVec2(minX + 7.f - 75, maxY + 3.f);
					auto bottomLeftUP = ImVec2(minX - 3.0f, maxY - 7.f + 75);
					auto topLeftRIGHT = ImVec2(minX + 7.f - 75, minY - 3.0f);
					auto topLeftDOWN = ImVec2(minX - 3.0f, minY + 7.f - 75);


					window.DrawList->AddRectFilled(topLeft, bottomRight, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 0.70f }));
					ImU32 kek = ImGui::GetColorU32({ ImGui::GetColorU32({ 1.f, 0.f, 0.f, 1.0f }) });
					window.DrawList->AddLine(topLeft, topLeftRIGHT, color, 1.00f);
					window.DrawList->AddLine(topLeft, topLeftDOWN, color, 1.00f);

					window.DrawList->AddLine(bottomRight, bottomRightLEFT, color, 1.5f);
					window.DrawList->AddLine(bottomRight, bottomRightUP, color, 1.5f);

					window.DrawList->AddLine(topRight, topRightLEFT, color, 1.5f);
					window.DrawList->AddLine(topRight, topRightDOWN, color, 1.5f);

					window.DrawList->AddLine(bottomLeft, bottomLeftRIGHT, color, 1.5f);
					window.DrawList->AddLine(bottomLeft, bottomLeftUP, color, 1.5f);
				}
				if (config_system.item.PlayerNames) {
					FString playerName;
					Core::ProcessEvent(state, Offsets::Engine::PlayerState::GetPlayerName, &playerName, 0);
					if (playerName.c_str()) {
						CHAR copy[0xFF] = { 0 };
						auto w2s = *reinterpret_cast<FVector*>(head);
						float dist;
						if (Util::WorldToScreen(width, height, &w2s.X)) {
							auto dx = w2s.X;
							auto dy = w2s.Y;
							auto dz = w2s.Z;
							dist = Util::SpoofCall(sqrtf, dx * dx + dy * dy + dz * dz) / 100.0f;
						}
						CHAR lel[0xFF] = { 0 };
						wcstombs(lel, playerName.c_str(), sizeof(lel));
						Util::FreeInternal(playerName.c_str());
						snprintf(copy, sizeof(copy), ("%s [%dm]"), lel, static_cast<INT>(dist));
						auto centerTop = ImVec2((topLeft.x + bottomRight.x) / 2.0f, topLeft.y);
						auto size = ImGui::GetFont()->CalcTextSizeA(window.DrawList->_Data->FontSize, FLT_MAX, 0, copy);
						//	window.DrawList->AddRectFilled(ImVec2(centerTop.x - size.x / 2.0f, centerTop.y - size.y + 3.0f), ImVec2(centerTop.x + size.x / 2.0f, centerTop.y), ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 0.4f }));
						ImVec2 kek = ImVec2(centerTop.x - size.x / 2.0f + 10, centerTop.y - size.y);
						//	window.DrawList->AddRectFilled(kek, ImVec2(centerTop.y - size.y), ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, 0.20f }));
						std::string jsj = copy;
						if (jsj.find(streamsnipena) != std::string::npos) {
							window.DrawList->AddText(ImVec2(centerTop.x - size.x / 2.0f + 10, centerTop.y - size.y), ImGui::GetColorU32({ 1.0f, 0.0f, 1.0f, 1.0f }), copy);
						}
						else
						{
							window.DrawList->AddText(ImVec2(centerTop.x - size.x / 2.0f + 10, centerTop.y - size.y), color, copy);
						}
					}
				}
			}
		}

		if (config_system.item.Aimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, config_system.keybind.AimbotLock) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			Core::TargetPawn = closestPawn;
			Core::NoSpread = TRUE;
			if (config_system.item.Aimbot && config_system.item.AntiAim && Util::SpoofCall(GetAsyncKeyState, config_system.keybind.AntiAim)) {
				int rnd = rand();
				FRotator args = { 0 };
				args.Yaw = rnd;
				Core::ProcessEvent(Core::LocalPlayerController, Offsets::Engine::Controller::ClientSetRotation, &args, 0);
				//mouse_event(000001, rnd, NULL, NULL, NULL); old anti aim
			}
		}
		else {
			Core::TargetPawn = nullptr;
			Core::NoSpread = FALSE;
		}

		bool isSilent = config_system.item.SilentAimbot;
		bool isRage = config_system.item.AutoAimbot;
		if (config_system.item.SpinBot && Util::SpoofCall(GetAsyncKeyState, config_system.keybind.Spinbot) && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			int rnd = rand();
			FRotator args = { 0 };
			args.Yaw = rnd;
			if (closestPawn) {
				Core::TargetPawn = closestPawn;
				Core::NoSpread = TRUE;
			}
			else {
				Core::ProcessEvent(Core::LocalPlayerController, Offsets::Engine::Controller::ClientSetRotation, &args, 0);
			}
			config_system.item.AutoAimbot = true;
			config_system.item.SilentAimbot = true;
		}
		else {
			if (!isSilent) {
				config_system.item.SilentAimbot = false;
			}
			if (!isRage) {
				config_system.item.AutoAimbot = false;
			}

			if (config_system.item.SilentAimbot) {
				isSilent = true;
			}
			if (config_system.item.AutoAimbot) {
				isRage = true;
			}
		}

		if (config_system.item.FlickAimbot && closestPawn && Util::SpoofCall(GetAsyncKeyState, config_system.keybind.AimbotShoot) < 0 && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			Core::TargetPawn = closestPawn;
			Core::NoSpread = TRUE;
		}

		if (config_system.item.AutoAim && closestPawn && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			//mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			Core::TargetPawn = closestPawn;
			Core::NoSpread = TRUE;
		}

		if (config_system.item.SpamAutoAim && closestPawn && Util::SpoofCall(GetForegroundWindow) == hWnd) {
			mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			Core::TargetPawn = closestPawn;
			Core::NoSpread = TRUE;
			mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			Core::TargetPawn = nullptr;
			Core::NoSpread = FALSE;
		}

		if (config_system.item.DrawAimbotFOV) {
			window.DrawList->AddCircle(ImVec2(width / 2, height / 2), config_system.item.AimbotFOV, ImGui::GetColorU32({ config_system.item.FOVCircleColor[0], config_system.item.FOVCircleColor[1], config_system.item.FOVCircleColor[2], config_system.item.FOVCircleOpacity }), 128);
			if (config_system.item.DrawFilledAimbotFOV) {
				window.DrawList->AddCircleFilled(ImVec2(width / 2, height / 2), config_system.item.AimbotFOV, ImGui::GetColorU32({ 0.0f, 0.0f, 0.0f, config_system.item.FOVCircleFilledOpacity }), 128);
			}
		}

		if (&config_system.item.CrosshairSize) {
			//base crosshair
			window.DrawList->AddLine(ImVec2(width / 2 + config_system.item.CrosshairSize, height / 2), ImVec2(width / 2 - config_system.item.CrosshairSize, height / 2), ImGui::GetColorU32({ config_system.item.FOVCircleColor[0], config_system.item.FOVCircleColor[1], config_system.item.FOVCircleColor[2], 1.0f }), config_system.item.CrosshairThickness);
			window.DrawList->AddLine(ImVec2(width / 2, height / 2 + config_system.item.CrosshairSize), ImVec2(width / 2, height / 2 - config_system.item.CrosshairSize), ImGui::GetColorU32({ config_system.item.FOVCircleColor[0], config_system.item.FOVCircleColor[1], config_system.item.FOVCircleColor[2], 1.0f }), config_system.item.CrosshairThickness);

			//fancy crosshair
			window.DrawList->AddLine(ImVec2(width / 2 + config_system.item.CrosshairSize / 2, height / 2), ImVec2(width / 2 - config_system.item.CrosshairSize / 2, height / 2), ImGui::GetColorU32({ 1.0f, 0.0f, 0.0f, 1.0f }), config_system.item.CrosshairThickness);
			window.DrawList->AddLine(ImVec2(width / 2, height / 2 + config_system.item.CrosshairSize / 2), ImVec2(width / 2, height / 2 - config_system.item.CrosshairSize / 2), ImGui::GetColorU32({ 1.0f, 0.0f, 1.0f, 0.0f }), config_system.item.CrosshairThickness);
		}

		success = TRUE;
	} while (FALSE);

	if (!success) {
		Core::LocalPlayerController = Core::LocalPlayerPawn = Core::TargetPawn = nullptr;
	}
	EndScene(window);
	return PresentOriginal(swapChain, syncInterval, flags);
}



__declspec(dllexport) HRESULT ResizeHook(IDXGISwapChain* swapChain, UINT bufferCount, UINT width, UINT height, DXGI_FORMAT newFormat, UINT swapChainFlags) {
	ImGui_ImplDX11_Shutdown();
	renderTargetView->Release();
	immediateContext->Release();
	device->Release();
	device = nullptr;

	return ResizeOriginal(swapChain, bufferCount, width, height, newFormat, swapChainFlags);
}


bool Render::Initialize() {
	IDXGISwapChain* swapChain = nullptr;
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	auto featureLevel = D3D_FEATURE_LEVEL_11_0;

	DXGI_SWAP_CHAIN_DESC sd = { 0 };
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.OutputWindow = FindWindow((L"UnrealWindow"), (L"Fortnite  "));
	sd.SampleDesc.Count = 1;
	sd.Windowed = TRUE;

	if (FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, 0, &featureLevel, 1, D3D11_SDK_VERSION, &sd, &swapChain, &device, nullptr, &context))) {
		MessageBox(0, L"Critical error have happened\nPlease contact an admin with the error code:\n0x0001b", L"Error", MB_ICONERROR);
		return FALSE;
	}

	auto table = *reinterpret_cast<PVOID**>(swapChain);
	auto present = table[8];
	auto resize = table[13];

	context->Release();
	device->Release();
	swapChain->Release();

	const auto pcall_present_discord = Helper::PatternScan(Discord::GetDiscordModuleBase(), "FF 15 ? ? ? ? 8B D8 E8 ? ? ? ? E8 ? ? ? ? EB 10");
	auto presentSceneAdress = Helper::PatternScan(Discord::GetDiscordModuleBase(),
		"48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B D9 41 8B F8");

	DISCORD.HookFunction(presentSceneAdress, (uintptr_t)PresentHook, (uintptr_t)&PresentOriginal);

	DISCORD.HookFunction(presentSceneAdress, (uintptr_t)ResizeHook, (uintptr_t)&PresentOriginal);
	return TRUE;
}

