#include <windows.h>
#include <d3d9.h>
#include "toml++/toml.hpp"

#include "nya_dx9_hookbase.h"
#include "nya_commonmath.h"
#include "nya_commonhooklib.h"

#include "game.h"

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

uintptr_t AISameCarASM_jmp = 0x409308;
void __attribute__((naked)) AISameCarASM() {
	__asm__ (
		"mov edx, %1\n\t"
		"mov [esi+0x298], edx\n\t"
		"jmp %0\n\t"
			:
			:  "m" (AISameCarASM_jmp), "m" (nPlayerCarID)
	);
}

void UpdateD3DProperties() {
	g_pd3dDevice = *(IDirect3DDevice9**)(0x7242B0 + 0x60);
	ghWnd = *(HWND*)(0x7242B0 + 0x7C);
	nResX = *(int*)0x764A84;
	nResY = *(int*)0x764A88;
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

auto EndSceneOrig = (HRESULT(__thiscall*)(void*))nullptr;
HRESULT __fastcall EndSceneHook(void* a1) {
	D3DHookMain();
	return EndSceneOrig(a1);
}

auto D3DResetOrig = (void(__thiscall*)(void*))nullptr;
void __fastcall D3DResetHook(void* a1) {
	if (g_pd3dDevice) {
		UpdateD3DProperties();
		ImGui_ImplDX9_InvalidateDeviceObjects();
		bDeviceJustReset = true;
	}
	return D3DResetOrig(a1);
}

void InitTimeTrials() {
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x409C7D, &ProcessGhostCarsASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x47A010, &ProcessPlayerCarsASM);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x469459, &GetAIName);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x492A9F, &FinishLapASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x409302, &AISameCarASM);
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x4696C8, &GetPlayerCarASM);
	NyaHookLib::Patch<uint8_t>(0x46828E + 1, 1); // only spawn one ai
	NyaHookLib::Patch<uint8_t>(0x46829F + 1, 1); // only spawn one ai
	NyaHookLib::Patch<uint8_t>(0x43407E, 0xEB); // use regular skins for ai
	NyaHookLib::Patch<uint8_t>(0x432CF5, 0xEB); // use regular skins for ai
	NyaHookLib::Patch<uint8_t>(0x432D6E, 0xEB); // use regular skins for ai
	NyaHookLib::Patch<uint8_t>(0x433EA2, 0xEB); // use regular skins for ai
	NyaHookLib::PatchRelative(NyaHookLib::JMP, 0x409D90, 0x40A6BE); // disable ai control
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
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x409C7D, ProcessGhostCarsASM_call);
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x47A010, ProcessPlayerCarsASM_call);

	// revert ai name
	NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x469459, 0x467770);

	// revert assembly patches
	NyaHookLib::Patch<uint64_t>(0x492A9F, 0xA6E809298FAC0D8B);
	NyaHookLib::Patch<uint64_t>(0x409302, 0x86C7000002989689);
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

BOOL WINAPI DllMain(HINSTANCE, DWORD fdwReason, LPVOID) {
	switch( fdwReason ) {
		case DLL_PROCESS_ATTACH: {
			if (NyaHookLib::GetEntryPoint() != 0x24CEF7) {
				MessageBoxA(nullptr, aFOUCVersionFail, "nya?!~", MB_ICONERROR);
				exit(0);
				return TRUE;
			}

			if (bShowInputsWhileDriving || bViewReplayMode || bPBTimeDisplayEnabled || bCurrentSessionPBTimeDisplayEnabled) {
				EndSceneOrig = (HRESULT(__thiscall*)(void*))(*(uintptr_t*)0x677448);
				NyaHookLib::Patch(0x677448, &EndSceneHook);
				D3DResetOrig = (void(__thiscall*)(void*))NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x60F744, &D3DResetHook);
			}

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