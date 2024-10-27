int TimeTrial_SetEnabled(void* a1) {
	bool on = luaL_checknumber(a1, 1);
	if (on) {
		InitTimeTrials();
		bLastRaceWasTimeTrial = true;
	}
	else if (bTimeTrialsEnabled) {
		UninitTimeTrials();
		bLastRaceWasTimeTrial = false;
	}
	bTimeTrialsEnabled = on;
	return 0;
}

int TimeTrial_Set3LapMode(void* a1) {
	b3LapMode = luaL_checknumber(a1, 1);
	return 0;
}

int TimeTrial_Get3LapMode(void* a1) {
	lua_pushnumber(a1, b3LapMode);
	return 1;
}

int TimeTrial_SetPropsEnabled(void* a1) {
	bool on = luaL_checknumber(a1, 1);
	if (on) EnableProps();
	else DisableProps();
	bNoProps = !on;
	return 0;
}

int TimeTrial_SetPropsEnabledBool(void* a1) {
	bool on = luaL_checknumber(a1, 1);
	bNoProps = !on;
	return 0;
}

int TimeTrial_SetNitroType(void* a1) {
	nNitroType = luaL_checknumber(a1, 1);
	return 0;
}

int TimeTrial_SetUpgradeLevel(void* a1) {
	nUpgradeLevel = luaL_checknumber(a1, 1);
	return 0;
}

int TimeTrial_SetHandlingMode(void* a1) {
	nHandlingMode = luaL_checknumber(a1, 1);
	return 0;
}

int TimeTrial_GetPropsEnabled(void* a1) {
	lua_pushnumber(a1, !bNoProps);
	return 1;
}

int TimeTrial_GetNitroType(void* a1) {
	lua_pushnumber(a1, nNitroType);
	return 1;
}

int TimeTrial_SetCareerMode(void* a1) {
	bIsCareerMode = luaL_checknumber(a1, 1);
	return 0;
}

int TimeTrial_SetCareerSuperAuthors(void* a1) {
	bDisplaySuperAuthorTime = (int)luaL_checknumber(a1, 1) >= 1;
	bDisplayAuthorInCareer = (int)luaL_checknumber(a1, 1) >= 2;
	return 0;
}

int TimeTrial_WasLastRaceTimeTrial(void* a1) {
	lua_pushboolean(a1, bLastRaceWasTimeTrial);
	bLastRaceWasTimeTrial = false;
	return 1;
}

int TimeTrial_GetLastEventGhostTime(void* a1) {
	lua_pushnumber(a1, OpponentsCareer[(int)luaL_checknumber(a1, 1)].nLastRacePBTime);
	return 1;
}

int TimeTrial_IsLastEventGhostValid(void* a1) {
	lua_pushnumber(a1, OpponentsCareer[(int)luaL_checknumber(a1, 1)].nLastRacePBTime != UINT_MAX);
	return 1;
}

int TimeTrial_GetLastEventPlayerTime(void* a1) {
	lua_pushnumber(a1, nCareerLastRacePBTime);
	return 1;
}

void CustomLUAFunctions(void* a1) {
	lua_pushcfunction(a1, (void*)&TimeTrial_SetEnabled, 0);
	lua_setfield(a1, -10002, "TimeTrial_SetEnabled");
	lua_pushcfunction(a1, (void*)&TimeTrial_Set3LapMode, 0);
	lua_setfield(a1, -10002, "TimeTrial_Set3LapMode");
	lua_pushcfunction(a1, (void*)&TimeTrial_Get3LapMode, 0);
	lua_setfield(a1, -10002, "TimeTrial_Get3LapMode");
	lua_pushcfunction(a1, (void*)&TimeTrial_SetPropsEnabled, 0);
	lua_setfield(a1, -10002, "TimeTrial_SetPropsEnabled");
	lua_pushcfunction(a1, (void*)&TimeTrial_SetPropsEnabledBool, 0);
	lua_setfield(a1, -10002, "TimeTrial_SetPropsEnabledBool");
	lua_pushcfunction(a1, (void*)&TimeTrial_SetNitroType, 0);
	lua_setfield(a1, -10002, "TimeTrial_SetNitroType");
	lua_pushcfunction(a1, (void*)&TimeTrial_SetUpgradeLevel, 0);
	lua_setfield(a1, -10002, "TimeTrial_SetUpgradeLevel");
	lua_pushcfunction(a1, (void*)&TimeTrial_SetHandlingMode, 0);
	lua_setfield(a1, -10002, "TimeTrial_SetHandlingMode");
	lua_pushcfunction(a1, (void*)&TimeTrial_GetPropsEnabled, 0);
	lua_setfield(a1, -10002, "TimeTrial_GetPropsEnabled");
	lua_pushcfunction(a1, (void*)&TimeTrial_GetNitroType, 0);
	lua_setfield(a1, -10002, "TimeTrial_GetNitroType");
	lua_pushcfunction(a1, (void*)&TimeTrial_SetCareerMode, 0);
	lua_setfield(a1, -10002, "TimeTrial_SetCareerMode");
	lua_pushcfunction(a1, (void*)&TimeTrial_SetCareerSuperAuthors, 0);
	lua_setfield(a1, -10002, "TimeTrial_SetCareerSuperAuthors");
	lua_pushcfunction(a1, (void*)&TimeTrial_GetLastEventGhostTime, 0);
	lua_setfield(a1, -10002, "TimeTrial_GetLastEventGhostTime");
	lua_pushcfunction(a1, (void*)&TimeTrial_IsLastEventGhostValid, 0);
	lua_setfield(a1, -10002, "TimeTrial_IsLastEventGhostValid");
	lua_pushcfunction(a1, (void*)&TimeTrial_GetLastEventPlayerTime, 0);
	lua_setfield(a1, -10002, "TimeTrial_GetLastEventPlayerTime");
	lua_pushcfunction(a1, (void*)&TimeTrial_WasLastRaceTimeTrial, 0);
	lua_setfield(a1, -10002, "TimeTrial_WasLastRaceTimeTrial");
}

template<int id>
int __cdecl GetTimeTrialTime(wchar_t* str, size_t len, void* a3, void* a4) {
	auto timestr = GetTimeText(OpponentsCareer[id].nPBTime, true);
	mbstowcs(str, timestr.c_str(), len);
	return wcslen(str);
}

int __cdecl GetTotalTime(wchar_t* str, size_t len, void* a3, void* a4) {
	auto ply = *(Player**)a4;
	auto score = GetPlayerScore<PlayerScoreRace>(ply->nPlayerId);
	auto time = ply->nRaceTime;
	if (time < 0) time = 0;
	if (score->nCurrentLap >= pGameFlow->nNumLaps) {
		time = score->nLapTimes[score->nCurrentLap];
	}

	auto timestr = GetTimeText(time, true);
	mbstowcs(str, timestr.c_str(), len);
	return wcslen(str);
}

// spacing 62 from laptime, 38 between each other

void TimeTrialKeywords(void* a3) {
	AddHUDKeyword("TIMETRIAL_SAUTHOR", GetTimeTrialTime<4>, a3);
	AddHUDKeyword("TIMETRIAL_AUTHOR", GetTimeTrialTime<3>, a3);
	AddHUDKeyword("TIMETRIAL_GOLD", GetTimeTrialTime<0>, a3);
	AddHUDKeyword("TIMETRIAL_SILVER", GetTimeTrialTime<1>, a3);
	AddHUDKeyword("TIMETRIAL_BRONZE", GetTimeTrialTime<2>, a3);
	AddHUDKeyword("TOTAL_TIME", GetTotalTime, a3);
}

void ApplyHUDPatches() {
	NyaFO2Hooks::PlaceHUDKeywordHook();
	NyaFO2Hooks::aHUDKeywordFuncs.push_back(TimeTrialKeywords);
}

void ApplyLUAPatches() {
	NyaFO2Hooks::PlaceScriptHook();
	NyaFO2Hooks::aScriptFuncs.push_back(CustomLUAFunctions);
}