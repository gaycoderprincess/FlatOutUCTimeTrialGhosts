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

int TimeTrial_SetPropsEnabled(void* a1) {
	bool on = luaL_checknumber(a1, 1);
	if (on) EnableProps();
	else DisableProps();
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

int TimeTrial_WasLastRaceTimeTrial(void* a1) {
	lua_pushnumber(a1, bLastRaceWasTimeTrial);
	bLastRaceWasTimeTrial = false;
	return 1;
}

auto lua_pushcfunction_hooked = (void(*)(void*, void*, int))0x633750;
void CustomLUAFunctions(void* a1, void* a2, int a3) {
	lua_pushcfunction(a1, (void*)&TimeTrial_SetEnabled, 0);
	lua_setfield(a1, -10002, "TimeTrial_SetEnabled");
	lua_pushcfunction(a1, (void*)&TimeTrial_SetPropsEnabled, 0);
	lua_setfield(a1, -10002, "TimeTrial_SetPropsEnabled");
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
	lua_pushcfunction(a1, (void*)&TimeTrial_WasLastRaceTimeTrial, 0);
	lua_setfield(a1, -10002, "TimeTrial_WasLastRaceTimeTrial");
	return lua_pushcfunction_hooked(a1, a2, a3);
}

void ApplyLUAPatches() {
	lua_pushcfunction_hooked = (void(*)(void*, void*, int))NyaHookLib::PatchRelative(NyaHookLib::CALL, 0x4C9829, &CustomLUAFunctions);
}