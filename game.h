enum eGameMode {
	GM_NONE = 0,
	GM_CAREER = 1,
	GM_SINGLE_RACE = 2,
	GM_INSTANT_ACTION = 3,
	GM_CRASHTEST_DUMMY = 4,
	GM_ONLINE_MULTIPLAYER = 5,
	GM_PARTY = 6,
	GM_DEVELOPER = 7,
	GM_TOURNAMENT = 8,
	GM_SPLITSCREEN = 9,
	GM_TEST = 10,
	GM_ARCADE_CAREER = 11,
};

enum eGameRules {
	GR_DEFAULT = 0,
	GR_RACE = 1,
	GR_STUNT = 2,
	GR_DERBY = 3,
	GR_HUNTER_PREY = 4,
	GR_ARCADE_ADVENTURE = 5,
	GR_PONGRACE = 6,
	GR_TIMEATTACK = 7,
	GR_BEAT_THE_BOMB = 8,
	GR_ARCADE_RACE = 9,
	GR_TIME_TRIAL = 10,
	GR_TEST = 11,
};

enum eDerbyType {
	DERBY_NONE = 0,
	DERBY_LMS = 1,
	DERBY_WRECKING = 2,
	DERBY_FRAG = 3,
	DERBY_END = 4,
};

struct tMatrix {
	NyaVec3 a1;
	float w1;
	NyaVec3 a2;
	float w2;
	NyaVec3 a3;
	float w3;
	NyaVec3 a4;
	float w4;
};

class Car {
public:
	uint8_t _0[0x1C0];
	tMatrix mMatrix; // +1C0
	uint8_t _200[0x80];
	float qQuaternion[4]; // +280
	NyaVec3 vVelocity; // +290
	uint8_t _29C[0x4];
	NyaVec3 vAngVelocity; // +2A0
	uint8_t _2AC[0x2C];
	uint32_t nIsWrecked; // +2D8
	uint8_t _2DC[0x310];
	float fNitro; // +5EC
	uint8_t _5F0[0x1920];
	float fGasPedal; // +1F10
	float fBrakePedal; // +1F14
	float fNitroButton; // +1F18
	float fHandbrake; // +1F1C
	float fSteerAngle; // +1F20
	uint8_t _1F24[0x5B94];
	float fDamage; // +7AB8
};

enum eControllerInput {
	INPUT_HANDBRAKE = 0,
	INPUT_NITRO = 1,
	INPUT_CAMERA = 3,
	INPUT_LOOK_BEHIND = 4,
	INPUT_RESET = 5,
	INPUT_PLAYERLIST = 7,
	INPUT_MENU_ACCEPT = 8,
	INPUT_PAUSE = 9,
	INPUT_ACCELERATE = 10,
	INPUT_BRAKE = 11,
	INPUT_STEER_LEFT = 12,
	INPUT_STEER_RIGHT = 13,
	INPUT_GEAR_DOWN = 18,
	INPUT_GEAR_UP = 19,
};

enum ePlayerEvent {
	PLAYEREVENT_EJECT_WRECKED = 6009,
	PLAYEREVENT_WRECKED = 6032,
	PLAYEREVENT_DERBY_OUT_OF_TIME = 6033,
	PLAYEREVENT_RESPAWN_GHOST = 6040,
	PLAYEREVENT_WRECKED_2 = 6053,
};

class Player {
public:
	uint8_t _4[0x220];
	uint32_t nArrowColor; // +224
	uint32_t nArrowColorId; // +228
	uint8_t _22C[0x68];
	Car* pCar; // +294
	uint32_t nCarId; // +298
	uint32_t nCarSkinId; // +29C
	uint8_t _2A0[0x4];
	uint32_t nCharacterTypeId; // +2A4
	uint32_t nCharacterSkinId; // +2A8
	uint8_t _2AC[0x18];
	uint32_t nPlayerId; // +2C4
	uint8_t _2C8[0x178];
	uint32_t nGhosting; // +440
	uint8_t _444[0x4A0];
	uint32_t nSomeFlags; // +8E4

	virtual void _vf0() = 0;
	virtual void _vf1() = 0;
	virtual void _vf2() = 0;
	virtual void _vf3() = 0;
	virtual void _vf4() = 0;
	virtual void _vf5() = 0;
	virtual void _vf6() = 0;
	virtual void _vf7() = 0;
	virtual void _vf8() = 0;
	virtual void _vf9() = 0;
	virtual void _vf10() = 0;
	virtual void _vf11() = 0;
	virtual void _vf12() = 0;
	virtual void _vf13() = 0;
	virtual void _vf14() = 0;
	virtual void _vf15() = 0;
	virtual void _vf16() = 0;
	virtual void _vf17() = 0;
	virtual void TriggerEvent(int* properties) = 0;
};

class PlayerHost {
public:
	uint8_t _0[0x14];
	Player** aPlayers;
};

enum eGameState {
	GAME_STATE_NONE,
	GAME_STATE_MENU,
	GAME_STATE_RACE
};

class Game {
public:
	uint8_t _0[0x4A4];
	int nGameState; // +4A4
	uint8_t _4A8[0x24];
	int nLevelId; // +4CC
	uint8_t _4D0[0x4];
	int nNumLaps; // +4D4
	uint8_t _4D8[0x20];
	float fNitroMultiplier; // +4F8
	uint8_t _4FC[0x2324];
	PlayerHost* pHost; // +2820
};
auto& pGame = *(Game**)0x9298FAC;

class PlayerScoreRace {
public:
	uint8_t _0[0x4];
	uint32_t nPlayerId; // 4
	uint8_t _8[0x3C];
	uint32_t nCurrentLap; // 44
};

class ScoreManager {
public:
	uint8_t _0[0x8];
	void** pScoresStart;
	void** pScoresEnd;
};
auto& pScoreManager = *(ScoreManager**)0x846514;

auto& pLoadingScreen = *(void**)0x929902C;

class Font {
public:
	uint8_t _0[0xC];
	NyaDrawing::CNyaRGBA32 nColor; // +C
	uint8_t _10[0x2C];
	struct
	{
		bool bXCenterAlign : 1;
		bool bXRightAlign : 1;
	}; // +34
	uint8_t _38[0x14];
	float fScaleX; // +4C
	float fScaleY; // +50

	static inline auto GetFont = (Font*(__stdcall*)(void*, const char*))0x457D00;
	static inline auto Display = (void(*)(Font*, float, float, const wchar_t*, ...))0x5A8BE0;
};

Player* GetPlayer(int id) {
	auto host = pGame->pHost;
	if (!host) return nullptr;
	auto players = host->aPlayers;
	if (!players) return nullptr;
	auto ply = players[id];
	if (!ply || !ply->pCar) return nullptr;
	return ply;
}

void DrawStringFO2(tNyaStringData data, const wchar_t* string, const char* font) {
	auto pFont = Font::GetFont(*(void**)(0x9298FD8), font);
	pFont->fScaleX = data.size * nResX / 20.0;
	pFont->fScaleY = data.size * nResY / 20.0;
	pFont->bXRightAlign = data.XRightAlign;
	pFont->bXCenterAlign = data.XCenterAlign;
	pFont->nColor.r = data.b;
	pFont->nColor.g = data.g;
	pFont->nColor.b = data.r;
	pFont->nColor.a = data.a;
	pFont->fScaleX *= GetAspectRatioInv();
	if (data.YCenterAlign) {
		data.y -= data.size * 0.5;
	}
	Font::Display(pFont, data.x * nResX, data.y * nResY, string);
}

template<typename T>
T* GetPlayerScore(int playerId) {
	if (!pScoreManager) return nullptr;

	auto score = (T**)pScoreManager->pScoresStart;
	auto end = (T**)pScoreManager->pScoresEnd;
	while (score < end) {
		if ((*score)->nPlayerId + 1 == playerId) {
			return *score;
		}
		score++;
	}
	return nullptr;
}

auto luaL_checktype = (void(*)(void*, int, int))0x634C70;
auto luaL_checkudata = (void*(*)(void*, int, const char*))0x634BB0;
auto luaL_typerror = (void(*)(void*, int, const char*))0x634900;
auto lua_pushnumber = (int(*)(void*, float))0x633550;
auto luaL_checknumber = (float(*)(void*, int))0x634DD0;
auto lua_setfield = (void(*)(void*, int, const char*))0x633D20;
auto lua_pushcfunction = (void(*)(void*, void*, int))0x633750;
auto lua_pushboolean = (int(*)(void*, int))0x633870;
auto lua_pushlstring = (int(*)(void*, const wchar_t*, size_t))0x6335D0;