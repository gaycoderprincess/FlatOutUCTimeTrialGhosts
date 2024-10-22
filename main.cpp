#include <windows.h>
#include <d3d9.h>
#include "toml++/toml.hpp"

#include "nya_dx9_hookbase.h"
#include "nya_commonmath.h"
#include "nya_commonhooklib.h"

#include "fouc.h"
#include "chloemenulib.h"

uintptr_t pControllerVTable = 0x6F403C;
void SetPlayerControl(bool on) {
	NyaHookLib::Patch<uint32_t>(0x6E1AA8, on ? 0x47CCF0 : 0x47D2A3);
}

void SetGhostVisuals(bool on) {
	NyaHookLib::Patch<uint64_t>(0x43F9E7, on ? 0xD28500000113840F : 0xD2859000000114E9);
}

void UninitTimeTrials();

#include "timetrialshared.h"

uintptr_t ProcessGhostCarsASM_call = 0x478CF0;
void __attribute__((naked)) ProcessGhostCarsASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, esi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (ProcessGhostCarsASM_call), "i" (ProcessGhostCar)
	);
}

uintptr_t ProcessPlayerCarsASM_call = 0x478CF0;
void __attribute__((naked)) ProcessPlayerCarsASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, esi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (ProcessPlayerCarsASM_call), "i" (ProcessGhostCar)
	);
}

uintptr_t FinishLapASM_jmp = 0x492AA5;
void __attribute__((naked)) FinishLapASM() {
	__asm__ (
		"mov ecx, [esp+0x10]\n\t"
		"pushad\n\t"
		"call %1\n\t"
		"popad\n\t"
		"mov ecx, 0x9298FAC\n\t"
		"mov ecx, [ecx]\n\t"
		"jmp %0\n\t"
			:
			:  "m" (FinishLapASM_jmp), "i" (OnFinishLap)
	);
}

int nPlayerCarID = 0;

uintptr_t GetPlayerCarASM_jmp = 0x4696D1;
void __attribute__((naked)) GetPlayerCarASM() {
	__asm__ (
		"mov edx, [edi-0x14]\n\t"
		"mov %1, edx\n\t"
		"mov [ebx+0x298], edx\n\t"
		"jmp %0\n\t"
			:
			:  "m" (GetPlayerCarASM_jmp), "m" (nPlayerCarID)
	);
}

int GetNumSkinsForCar(int dbCar) {
	int tmp[2];
	auto path = "data/cars/car_" + std::to_string(GetCarDataPath(dbCar, false)) + "/skin";
	int i;
	for (i = 0; i < 255; i++) {
		auto file = (path + std::to_string(i+1) + ".dds");
		if (!BFSManager_DoesFileExist(*(void**)0x846688, file.c_str(), tmp)) {
			break;
		}
	}
	return i;
}

void __fastcall SetAISameCar(Player* pPlayer) {
	pPlayer->nCarId = nPlayerCarID;

	if (pPlayer->nPlayerId <= 2) return;
	auto tmp = LoadTemporaryGhostForSpawning(pPlayer->nCarId);
	if (!tmp->IsValid()) return;
	if (tmp->sPlayerName[0] && pOpponentPlayerInfo) {
		wcscpy_s(pOpponentPlayerInfo->sPlayerName, 16, tmp->sPlayerName.c_str());
	}
	if (tmp->nCarSkinId < 1 || tmp->nCarSkinId > GetNumSkinsForCar(pPlayer->nCarId)) return;
	pPlayer->nCarSkinId = tmp->nCarSkinId;
}

uintptr_t AISameCarASM_jmp = 0x409308;
void __attribute__((naked)) AISameCarASM() {
	__asm__ (
		"pushad\n\t"
		"mov ecx, esi\n\t"
		"call %1\n\t"
		"popad\n\t"
		"jmp %0\n\t"
			:
			:  "m" (AISameCarASM_jmp), "i" (SetAISameCar)
	);
}

void __attribute__((naked)) GetAINameASM() {
	__asm__ (
		"mov ecx, eax\n\t"
		"mov edx, esi\n\t"
		"push ecx\n\t"
		"push edx\n\t"
		"push ebx\n\t"
		"push ebp\n\t"
		"push esi\n\t"
		"push edi\n\t"
		"call %0\n\t"
		"pop edi\n\t"
		"pop esi\n\t"
		"pop ebp\n\t"
		"pop ebx\n\t"
		"pop edx\n\t"
		"pop ecx\n\t"
		"ret\n\t"
			:
			:  "i" (GetAIName)
	);
}

void UpdateD3DProperties() {
	g_pd3dDevice = pDeviceD3d->pD3DDevice;
	ghWnd = pDeviceD3d->hWnd;
	nResX = nGameResolutionX;
	nResY = nGameResolutionY;
}

bool bDeviceJustReset = false;
void D3DHookMain() {
	if (!g_pd3dDevice) {
		UpdateD3DProperties();
		InitHookBase();
	}

	if (bDeviceJustReset) {
		ImGui_ImplDX9_CreateDeviceObjects();
		bDeviceJustReset = false;
	}
	HookBaseLoop();
}

void OnEndScene() {
	*(float*)0x716034 = 480.1f; // hack to fix font scale in chloe collection
	D3DHookMain();
	*(float*)0x716034 = 480.0f;
}

void OnD3DReset() {
	if (g_pd3dDevice) {
		UpdateD3DProperties();
		ImGui_ImplDX9_InvalidateDeviceObjects();
		bDeviceJustReset = true;
	}
}

void InitTimeTrials() {
	static bool bOnce = false;
	if (!bOnce) {
		ProcessGhostCarsASM_call = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x409C7D, &ProcessGhostCarsASM);
		ProcessPlayerCarsASM_call = NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x47A010, &ProcessPlayerCarsASM);
		bOnce = true;
	}

	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x469459, &GetAINameASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x492A9F, &FinishLapASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x409302, &AISameCarASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4696C8, &GetPlayerCarASM);
	NyaHookLib::Patch<uint8_t>(0x46828E + 1, bIsCareerMode ? 3 : 2); // only spawn two ai
	NyaHookLib::Patch<uint8_t>(0x46829F + 1, bIsCareerMode ? 3 : 2); // only spawn two ai
	NyaHookLib::Patch<uint8_t>(0x43407E, 0xEB); // use regular skins for ai
	NyaHookLib::Patch<uint8_t>(0x432CF5, 0xEB); // use regular skins for ai
	NyaHookLib::Patch<uint8_t>(0x432D6E, 0xEB); // use regular skins for ai
	NyaHookLib::Patch<uint8_t>(0x433EA2, 0xEB); // use regular skins for ai
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x409D90, 0x40A6BE); // disable ai control

	SetGhostVisuals(nGhostVisuals);
}

void DisableProps() {
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x58C1FD, 0x58C28C);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x58B9FC, 0x58BAA0);
}

void EnableProps() {
	NyaHookLib::Patch<uint64_t>(0x58C1FD, 0x558B00000089840F);
	NyaHookLib::Patch<uint64_t>(0x58B9FC, 0x4C8B0000009E840F);
}

void UninitTimeTrials() {
	// revert ai name
	if (!bChloeCollectionIntegration) NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x469459, 0x467770);

	// revert assembly patches
	NyaHookLib::Patch<uint64_t>(0x492A9F, 0xA6E809298FAC0D8B);
	if (!bChloeCollectionIntegration) NyaHookLib::Patch<uint64_t>(0x409302, 0x86C7000002989689);
	NyaHookLib::Patch<uint64_t>(0x4696C8, 0x0002989389EC578B);

	// revert ai count
	NyaHookLib::Patch<uint8_t>(0x46828E + 1, 11);
	NyaHookLib::Patch<uint8_t>(0x46829F + 1, 11);

	// revert ai skins
	NyaHookLib::Patch<uint8_t>(0x43407E, 0x75);
	NyaHookLib::Patch<uint8_t>(0x432CF5, 0x75);
	NyaHookLib::Patch<uint8_t>(0x432D6E, 0x75);
	NyaHookLib::Patch<uint8_t>(0x433EA2, 0x75);

	// revert ai control
	NyaHookLib::Patch<uint64_t>(0x409D90, 0xEC83F0E483EC8B55);

	// revert prop patches
	EnableProps();

	// revert ghost visuals
	SetGhostVisuals(true);
}

#include "luafunctions.h"

void InitStandalone() {
	InitAndReadConfigFile();

	if (bTimeTrialsEnabled) {
		InitTimeTrials();
		if (nGhostVisuals == 0) SetGhostVisuals(false);
		if (bNoProps) {
			DisableProps();
		}
	}
	else {
		bPBTimeDisplayEnabled = false;
		bCurrentSessionPBTimeDisplayEnabled = false;
		bViewReplayMode = false;
	}
}

void TimeTrialMenu() {
	ChloeMenuLib::BeginMenu();

	if (DrawMenuOption(std::format("PB Display - {}", bPBTimeDisplayEnabled), "", false, false)) {
		bPBTimeDisplayEnabled = !bPBTimeDisplayEnabled;
	}

	if (DrawMenuOption(std::format("Session PB Display - {}", bCurrentSessionPBTimeDisplayEnabled), "", false, false)) {
		bCurrentSessionPBTimeDisplayEnabled = !bCurrentSessionPBTimeDisplayEnabled;
	}

	if (DrawMenuOption(std::format("Load Mismatched Replays - {}", bReplayIgnoreMismatches), "", false, false)) {
		bReplayIgnoreMismatches = !bReplayIgnoreMismatches;
	}

	const char* aGhostVisualNames[] = {
			"Off",
			"On",
			"Proximity"
	};
	if (DrawMenuOption(std::format("Ghost Visuals < {} >", aGhostVisualNames[nGhostVisuals]), "", false, false, true)) {
		if (auto lr = ChloeMenuLib::GetMoveLR()) {
			nGhostVisuals += lr;
			if (nGhostVisuals < 0) nGhostVisuals = 2;
			if (nGhostVisuals > 2) nGhostVisuals = 0;
			if (pGameFlow->nGameState == GAME_STATE_RACE && bTimeTrialsEnabled) {
				SetGhostVisuals(nGhostVisuals);
			}
		}
	}

	if (pGameFlow->nGameState != GAME_STATE_RACE) {
		if (DrawMenuOption(std::format("Replay Viewer - {}", bViewReplayMode), "", false, false)) {
			bViewReplayMode = !bViewReplayMode;
		}

		if (!bChloeCollectionIntegration) {
			const char* aNitroNames[] = {
					"0x",
					"1x",
					"2x",
					"Infinite"
			};
			if (DrawMenuOption(std::format("Nitro < {} >", aNitroNames[nNitroType]), "", false, false, true)) {
				if (auto lr = ChloeMenuLib::GetMoveLR()) {
					nNitroType += lr;
					if (nNitroType < 0) nNitroType = NITRO_INFINITE;
					if (nNitroType > NITRO_INFINITE) nNitroType = 0;
				}
			}

			if (DrawMenuOption(std::format("Props - {}", bNoProps), "", false, false)) {
				if (bNoProps = !bNoProps) {
					DisableProps();
				}
				else {
					EnableProps();
				}
			}

			if (DrawMenuOption(std::format("Three Lap Mode - {}", b3LapMode), "", false, false)) {
				b3LapMode = !b3LapMode;
			}
		}
	}

	ChloeMenuLib::EndMenu();
}

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			if (NyaHookLib::GetEntryPoint() != 0x24CEF7) {
				MessageBoxA(nullptr, aFOUCVersionFail, "nya?!~", MB_ICONERROR);
				exit(0);
				return TRUE;
			}

			if (bShowInputsWhileDriving || bViewReplayMode || bPBTimeDisplayEnabled || bCurrentSessionPBTimeDisplayEnabled) {
				NyaFO2Hooks::PlaceD3DHooks();
				NyaFO2Hooks::aEndSceneFuncs.push_back(OnEndScene);
				NyaFO2Hooks::aD3DResetFuncs.push_back(OnD3DReset);
			}

			ChloeMenuLib::RegisterMenu("Time Trial Ghosts - gaycoderprincess", &TimeTrialMenu);

			if (std::filesystem::exists("FlatOutUCChloeCollection_gcp.asi") || std::filesystem::exists("FlatOutUCCustomMP_gcp.asi") || std::filesystem::exists("foucpack_gcp_misc.bfs")) {
				ApplyLUAPatches();
				bChloeCollectionIntegration = true;
				bTimeTrialsEnabled = false;

				auto config = toml::parse_file("FlatOutUCTimeTrialGhosts_gcp.toml");
				bReplayIgnoreMismatches = config["main"]["load_mismatched_replays"].value_or(false);
			}
			else {
				InitStandalone();
			}
		} break;
		default:
			break;
	}
	return TRUE;
}