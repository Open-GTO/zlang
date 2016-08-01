/*

	About: GVar per player lang system
	Author: ziggi

*/

#if !defined _samp_included
	#error Please include a_samp or a_npc before zlang
#endif

#if !defined SetGVarString
	#tryinclude "gvar"

	#if !defined SetGVarString
		#tryinclude <gvar>

		#if !defined SetGVarString
			#error Please include gvar before zlang
		#endif
	#endif
#endif

#if defined _zlang_included
	#endinput
#endif

#define _zlang_included

/*
	Defines
*/

#if !defined MAX_LANGS
	#define MAX_LANGS (2)
#endif

#if !defined LANG_VAR_OFFSET
	#define LANG_VAR_OFFSET (1000)
#endif

#if !defined LANG_IGNORED_FIRST_SYMBOL
	#define LANG_IGNORED_FIRST_SYMBOL '\0', '#', ';'
#endif

#if !defined MAX_LANG_VAR_STRING
	#define MAX_LANG_VAR_STRING (64)
#endif

#if !defined MAX_LANG_VALUE_STRING
	#define MAX_LANG_VALUE_STRING (144)
#endif

#if !defined MAX_LANG_MVALUE_STRING
	#define MAX_LANG_MVALUE_STRING (MAX_LANG_VALUE_STRING * 25)
#endif

#if !defined MAX_LANG_CODE
	#define MAX_LANG_CODE (2)
#endif

#if !defined MAX_LANG_NAME
	#define MAX_LANG_NAME (16)
#endif

#if !defined MAX_LANG_FILES
	#define MAX_LANG_FILES (5)
#endif

#if !defined MAX_LANG_FILENAME
	#define MAX_LANG_FILENAME (256)
#endif

#define INVALID_LANG_ID (Lang:-1)
#define INVALID_LANG_FILE_ID (-1)

#if !defined _INC_y_va
	#define va_args<%0> {Float, _}:...
	#define va_start<%0> (va_:(%0))
#endif

/*
	Define functions
*/

#if !defined isnull
	#define isnull(%1) \
				((!(%1[0])) || (((%1[0]) == '\1') && (!(%1[1]))))
#endif

/*
	Enums
*/

enum e_ZLANG_INFO {
	e_zlCode[MAX_LANG_CODE + 1],
	e_zlName[MAX_LANG_NAME + 1],
}

enum (<<= 1) {
	LANG_SLOT_FREE = 1,
	LANG_SLOT_BUSY,
}

/*
	Vars
*/

static
	gLangInfo[MAX_LANGS][e_ZLANG_INFO],
	gLangFiles[MAX_LANGS][MAX_LANG_FILES][MAX_LANG_FILENAME],
	gLangSlots[MAX_LANGS char],
	Lang:gLangCount,
	Lang:gDefaultLang,
	Lang:gPlayerLang[MAX_PLAYERS];

/*
	Forwards
*/

forward Lang:Lang_Get(code[] = "", name[] = "");
forward Lang:_Lang_GetFreeSlot();

/*
	OnGameModeInit
*/

public OnGameModeInit()
{
	for (new Lang:lang; lang < Lang:MAX_LANGS; lang++) {
		_Lang_SetSlotStatus(lang, LANG_SLOT_FREE);
	}

	#if defined ZLANG_OnGameModeInit
		return ZLANG_OnGameModeInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit ZLANG_OnGameModeInit
#if defined ZLANG_OnGameModeInit
	forward ZLANG_OnGameModeInit();
#endif

/*
	OnGameModeExit
*/

public OnGameModeExit()
{
	for (new Lang:lang; lang < Lang:MAX_LANGS; lang++) {
		Lang_Remove(lang);
	}

	#if defined ZLANG_OnGameModeExit
		return ZLANG_OnGameModeExit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeExit
	#undef OnGameModeExit
#else
	#define _ALS_OnGameModeExit
#endif

#define OnGameModeExit ZLANG_OnGameModeExit
#if defined ZLANG_OnGameModeExit
	forward ZLANG_OnGameModeExit();
#endif

/*
	Loading functions
*/

stock Lang:Lang_Add(code[], name[])
{
	if (isnull(code) || isnull(name)) {
		return INVALID_LANG_ID;
	}

	new
		Lang:lang;

	lang = Lang_Get(code, name);

	if (lang == INVALID_LANG_ID) {
		lang = _Lang_GetFreeSlot();
	}

	if (lang == INVALID_LANG_ID) {
		return INVALID_LANG_ID;
	}

	Lang_SetCode(lang, code);
	Lang_SetName(lang, name);

	_Lang_SetSlotStatus(lang, LANG_SLOT_BUSY);
	gLangCount++;
	return lang;
}

stock Lang_Remove(Lang:lang)
{
	if (!Lang_IsValid(lang)) {
		return 0;
	}

	Lang_SetCode(lang, "");
	Lang_SetName(lang, "");

	for (new fid = 0; fid < MAX_LANG_FILES; fid++) {
		Lang_UnloadFile(lang, .fid = fid);
	}

	_Lang_SetSlotStatus(lang, LANG_SLOT_FREE);
	gLangCount--;
	return 1;
}

stock Lang_LoadFile(Lang:lang, filename[])
{
	if (!Lang_IsValid(lang)) {
		return INVALID_LANG_FILE_ID;
	}

	if (isnull(filename)) {
		return INVALID_LANG_FILE_ID;
	}

	new fid  = _Lang_FindFile(lang, filename);
	if (fid != INVALID_LANG_FILE_ID) {
		return fid;
	}

	new File:flang = fopen(filename, io_read);
	if (!flang) {
		printf("Error: no such language file '%s'", filename);
		return INVALID_LANG_FILE_ID;
	}

	fid = _Lang_GetFreeFileSlot(lang);
	_Lang_strcpy(gLangFiles[_:lang][fid], filename, MAX_LANG_FILENAME);

	new
		i,
		sep_pos,
		varname[MAX_LANG_VAR_STRING + 1],
		string[MAX_LANG_VAR_STRING + 3 + MAX_LANG_VALUE_STRING + 1],
		idx,
		symbol,
		length;

	while (fread(flang, string, sizeof(string))) {
		switch (string[0]) {
			case LANG_IGNORED_FIRST_SYMBOL: {
				continue;
			}
		}

		sep_pos = -1;
		for (i = 0; string[i] >= ' '; i++) {
			if (sep_pos == -1) {
				if (string[i] == ' ' && string[i + 1] == '=' && string[i + 2] == ' ') {
					if (i > MAX_LANG_VAR_STRING) {
						printf("Error: length of lang variable more than %d.", MAX_LANG_VAR_STRING);
						printf("Error: %s", string);
					}

					strmid(varname, string, 0, i);

					if (_Lang_IsTextExists(lang, varname)) {
						printf("Error: duplicated variable name %s in language '%s'", varname, Lang_ReturnName(lang));
						break;
					}

					sep_pos = i;
				}
			} else if (string[i] == '\\') {
				switch (string[i + 1]) {
					case 'n': {
						strdel(string, i, i + 1);
						string[i] = '\n';
					}
					case 't': {
						strdel(string, i, i + 1);
						string[i] = '\t';
					}
					case '%': {
						string[i] = '%';
					}
					case 's': {
						strdel(string, i, i + 1);
						string[i] = ' ';
					}
					case '\\': {
						strdel(string, i, i + 1);
						string[i] = '\\';
					}
					case '0'..'9': {
						length = 0;
						string[i] = 0;

						do {
							length++;
						} while ('0' <= string[i + length + 1] <= '9');

						for (idx = 1; idx <= length; idx++) {
							string[i] += (string[i + idx] - '0') * _Lang_pow(10, length - idx);
						}

						strdel(string, i + 1, i + length + 1);
					}
					case 'x': {
						length = 0;
						string[i] = 0;

						do {
							length++;
							symbol = string[i + length + 1];
						} while (   '0' <= symbol <= '9'
						         || 'A' <= symbol <= 'F'
						         || 'a' <= symbol <= 'f');

						for (idx = 1; idx <= length; idx++) {
							symbol = string[i + idx];

							if ('A' <= symbol <= 'F') {
								symbol = 10 + (symbol - 'A');
							} else if ('a' <= symbol <= 'f') {
								symbol = 10 + (symbol - 'a');
							} else if ('0' <= symbol <= '9') {
								symbol = symbol - '0';
							}

							string[i] += symbol * _Lang_pow(16, length - idx);
						}

						// gvar is not support symbols with negative code
						if (string[i] <= 0) {
							string[i] = ' ';
						}

						strdel(string, i + 1, i + length + 1);
					}
				}
			}
		}

		if (sep_pos != -1) {
			string[i] = '\0';
			_Lang_SetText(lang, varname, string[sep_pos + 3]);
		}
	}

	fclose(flang);
	return fid;
}

stock Lang_UnloadFile(Lang:lang, filename[] = "", fid = INVALID_LANG_FILE_ID)
{
	if (fid == INVALID_LANG_FILE_ID) {
		if (isnull(filename)) {
			return 0;
		}

		fid  = _Lang_FindFile(lang, filename);
		if (fid == INVALID_LANG_FILE_ID) {
			return 0;
		}
	}

	new File:flang = fopen(gLangFiles[_:lang][fid], io_read);
	if (!flang) {
		return INVALID_LANG_FILE_ID;
	}

	gLangFiles[_:lang][fid][0] = '\0';

	new
		sep_pos,
		string[MAX_LANG_VAR_STRING + 3 + MAX_LANG_VALUE_STRING + 1];

	while (fread(flang, string, sizeof(string))) {
		switch (string[0]) {
			case LANG_IGNORED_FIRST_SYMBOL: {
				continue;
			}
		}

		sep_pos = strfind(string, " = ");
		if (sep_pos == -1) {
			continue;
		}

		strmid(string, string, 0, sep_pos);
		Lang_RemoveText(lang, string);
	}

	fclose(flang);
	return 1;
}

stock Lang_Reload(Lang:lang)
{
	new
		filename[MAX_LANG_FILENAME];

	for (new fid = 0; fid < MAX_LANG_FILES; fid++) {
		if (isnull(gLangFiles[_:lang][fid])) {
			continue;
		}

		_Lang_strcpy(filename, gLangFiles[_:lang][fid]);

		Lang_UnloadFile(lang, filename);
		Lang_LoadFile(lang, filename);
	}
}

stock Lang:Lang_Get(code[] = "", name[] = "")
{
	new
		Lang:lang = INVALID_LANG_ID,
		is_code,
		is_name;

	is_code = strlen(code) != 0;
	is_name = strlen(name) != 0;

	for (new l; l < MAX_LANGS; l++) {
		if (is_code && !strcmp(code, gLangInfo[l][e_zlCode], true)) {
			return Lang:l;
		} else if (is_name && !strcmp(name, gLangInfo[l][e_zlName], true)) {
			return Lang:l;
		}
	}

	return lang;
}

stock Lang_SetName(Lang:lang, name[])
{
	return _Lang_strcpy(gLangInfo[_:lang][e_zlName], name, MAX_LANG_NAME);
}

stock Lang_GetName(Lang:lang, name[], const size = sizeof(name))
{
	_Lang_strcpy(name, gLangInfo[_:lang][e_zlName], size);
}

stock Lang_ReturnName(Lang:lang)
{
	new name[MAX_LANG_NAME];
	Lang_GetName(lang, name);
	return name;
}

stock Lang_SetCode(Lang:lang, code[])
{
	return _Lang_strcpy(gLangInfo[_:lang][e_zlCode], code, MAX_LANG_CODE + 1);
}

stock Lang_GetCode(Lang:lang, code[], const size = sizeof(code))
{
	strcat(code, gLangInfo[_:lang][e_zlCode], size);
}

stock Lang_ReturnCode(Lang:lang)
{
	new code[MAX_LANG_CODE + 1];
	Lang_GetCode(lang, code);
	return code;
}

stock Lang_GetCodes(result[], const size = sizeof(result), const separator = '/', const bool:isuppercase = false)
{
	new
		lang,
		i,
		length,
		sep[2];

	result[0] = '\0';
	sep[0] = separator;

	for (lang = 0; lang < MAX_LANGS; lang++) {
		strcat(result, gLangInfo[lang][e_zlCode], size);
		strcat(result, sep, size);

		for (i = (MAX_LANG_CODE + 1) * lang; i < (MAX_LANG_CODE + 1) * (lang + 1) - 1; i++) {
			if (isuppercase) {
				result[i] = toupper(result[i]);
			} else {
				result[i] = tolower(result[i]);
			}
		}
	}

	length = strlen(result);
	if (length != 0) {
		result[length - 1] = '\0';
	}
	return result;
}

stock Lang_ReturnCodes(const separator = '/', const bool:isuppercase = false)
{
	new result[(MAX_LANGS + 1) * (MAX_LANG_CODE + 1)];
	Lang_GetCodes(result, _, separator, isuppercase);
	return result;
}

stock Lang_IsValid(Lang:lang)
{
	if (_:lang < 0 || _:lang >= MAX_LANGS) {
		return 0;
	}
	return _Lang_IsSlotHaveStatus(lang, LANG_SLOT_BUSY);
}

stock Lang_GetCount()
{
	return gLangCount;
}

stock Lang_SetPlayerLang(playerid, Lang:lang)
{
	if (!Lang_IsValid(lang)) {
		return 0;
	}

	gPlayerLang[playerid] = lang;
	return 1;
}

stock Lang:Lang_GetPlayerLang(playerid)
{
	return gPlayerLang[playerid];
}

stock Lang:Lang_SetPlayerLangByCode(playerid, code[])
{
	new Lang:lang = Lang_Get(code);
	if (lang == INVALID_LANG_ID) {
		return 0;
	}

	gPlayerLang[playerid] = lang;
	return lang;
}

stock Lang:Lang_SetPlayerLangByName(playerid, name[])
{
	new Lang:lang = Lang_Get(.name = name);
	if (lang == INVALID_LANG_ID) {
		return 0;
	}

	gPlayerLang[playerid] = lang;
	return lang;
}

stock Lang_SetDefaultLang(Lang:lang)
{
	if (!Lang_IsValid(lang)) {
		return 0;
	}

	gDefaultLang = lang;
	return 1;
}

stock Lang:Lang_GetDefaultLang()
{
	return gDefaultLang;
}

/*
	Text functions
*/

stock Lang_SetText(Lang:lang, var[], text[])
{
	if (isnull(varname)) {
		return 0;
	}

	if (!Lang_IsValid(lang)) {
		return 0;
	}

	return _Lang_SetText(lang, var, text);
}

stock Lang_GetText(Lang:lang, var[], text[], const size = sizeof(text), va_args<>)
{
	text[0] = '\0';

	if (!Lang_IsValid(lang)) {
		strcat(text, "Error: lang is invalid. Var '", size);
		strcat(text, var, size);
		strcat(text, "' is not found.", size);
		return 0;
	}

	if (isnull(var)) {
		strcat(text, "Error: lang var is null", size);
		return 0;
	}

	new success = _Lang_GetText(lang, var, text, size);
	if (!success) {
		strcat(text, "Error: lang var '", size);
		strcat(text, var, size);
		strcat(text, "' is not found.", size);
		return 0;
	}

	if (numargs() > 4) {
		va_format(text, size, text, va_start<4>);
	}

	return 1;
}

stock Lang_RemoveText(Lang:lang, var[])
{
	return _Lang_RemoveText(lang, var);
}

stock Lang_IsTextExists(Lang:lang, var[])
{
	if (isnull(varname)) {
		return 0;
	}

	if (!Lang_IsValid(lang)) {
		return 0;
	}

	return _Lang_IsTextExists(lang, var);
}

#define Lang_GetPlayerText(%0, Lang_GetText(Lang_GetPlayerLang(%0),
#define Lang_GetDefaultText( Lang_GetText(Lang_GetDefaultLang(),

/*
	Print functions
*/

// printf
stock Lang_printf(var[], va_args<>)
{
	static
		text[MAX_LANG_VALUE_STRING],
		success;

	success = Lang_GetText(Lang_GetDefaultLang(), var, text);
	va_format(text, sizeof(text), text, va_start<1>);
	print(text);
	return success;
}

stock Lang_printfex(Lang:lang, var[], va_args<>)
{
	static
		text[MAX_LANG_VALUE_STRING],
		success;

	success = Lang_GetText(lang, var, text);
	va_format(text, sizeof(text), text, va_start<2>);
	print(text);
	return success;
}

// print
stock Lang_print(var[], va_args<>)
{
	static
		text[MAX_LANG_VALUE_STRING],
		success;

	success = Lang_GetText(Lang_GetDefaultLang(), var, text);
	print(text);
	return success;
}

stock Lang_printex(Lang:lang, var[])
{
	new
		text[MAX_LANG_VALUE_STRING],
		success;

	success = Lang_GetText(lang, var, text);
	print(text);
	return success;
}

/*
	Player text functions
*/

stock Lang_SendText(playerid, var[], va_args<>)
{
	static
		text[MAX_LANG_VALUE_STRING];

	Lang_GetText(Lang_GetPlayerLang(playerid), var, text);
	va_format(text, sizeof(text), text, va_start<2>);
	return SendClientMessage(playerid, -1, text);
}

stock Lang_SendTextToAll(var[], va_args<>)
{
	static
		playerid,
		text[MAX_LANG_VALUE_STRING];

#if defined _FOREACH_LOCAL_VERSION
	foreach (playerid : Player)
#else
	for (playerid = GetPlayerPoolSize(); playerid != -1; playerid--)
#endif
	{
		Lang_GetText(Lang_GetPlayerLang(playerid), var, text);
		va_format(text, sizeof(text), text, va_start<1>);
		SendClientMessage(playerid, -1, text);
	}
	return 1;
}

stock Lang_SendTextToPlayers(players[], var[], va_args<>)
{
	static
		idx,
		text[MAX_LANG_VALUE_STRING];

	for ( ; players[idx] != INVALID_PLAYER_ID; idx++) {
		Lang_GetText(Lang_GetPlayerLang(players[idx]), var, text);
		va_format(text, sizeof(text), text, va_start<2>);
		SendClientMessage(players[idx], -1, text);
	}
	return 1;
}

stock Lang_ShowDialog(playerid, dialogid, style, var_caption[], var_info[], var_button1[], var_button2[], va_args<>)
{
	static
		Lang:lang,
		caption[MAX_LANG_VALUE_STRING],
		button1[MAX_LANG_VALUE_STRING],
		button2[MAX_LANG_VALUE_STRING],
		info[MAX_LANG_VALUE_STRING];

	lang = Lang_GetPlayerLang(playerid);

	Lang_GetText(lang, var_caption, caption);
	Lang_GetText(lang, var_button1, button1);
	Lang_GetText(lang, var_button2, button2);
	Lang_GetText(lang, var_info, info);
	va_format(info, sizeof(info), info, va_start<7>);

	return ShowPlayerDialog(playerid, dialogid, style, caption, info, button1, button2);
}

stock Lang_GameText(playerid, time, style, var[], va_args<>)
{
	static
		text[MAX_LANG_VALUE_STRING];

	Lang_GetText(Lang_GetPlayerLang(playerid), var, text);
	va_format(text, sizeof(text), text, va_start<4>);
	return GameTextForPlayer(playerid, text, time, style);
}

stock Lang_GameTextForAll(time, style, var[], va_args<>)
{
	static
		playerid,
		text[MAX_LANG_VALUE_STRING];

#if defined _FOREACH_LOCAL_VERSION
	foreach (playerid : Player)
#else
	for (playerid = GetPlayerPoolSize(); playerid != -1; playerid--)
#endif
	{
		Lang_GetText(Lang_GetPlayerLang(playerid), var, text);
		va_format(text, sizeof(text), text, va_start<3>);
		SendClientMessage(playerid, -1, text);
	}
	return GameTextForAll(text, time, style);
}

/*
	Private functions
*/

static stock _Lang_FindFile(Lang:lang, filename[])
{
	for (new fid = 0; fid < MAX_LANG_FILES; fid++) {
		if (isnull(gLangFiles[_:lang][fid])) {
			continue;
		}

		if (!strcmp(filename, gLangFiles[_:lang][fid], true)) {
			return fid;
		}
	}

	return INVALID_LANG_FILE_ID;
}

static stock _Lang_GetFreeFileSlot(Lang:lang)
{
	for (new fid = 0; fid < MAX_LANG_FILES; fid++) {
		if (isnull(gLangFiles[_:lang][fid])) {
			return fid;
		}
	}

	return INVALID_LANG_FILE_ID;
}

/*
	Text functions
*/

static stock _Lang_SetText(Lang:lang, var[], text[])
{
	return SetGVarString(var, text, LANG_VAR_OFFSET + _:lang);
}

static stock _Lang_GetText(Lang:lang, var[], text[], const size = sizeof(text))
{
	static
		success,
		i,
		text_multi[MAX_LANG_VALUE_STRING],
		var_multi[MAX_LANG_VAR_STRING + 6];

	success = GetGVarString(var, text, size, LANG_VAR_OFFSET + _:lang);
	if (!success) {
		i = 0;

		do {
			format(var_multi, sizeof(var_multi), "%s_%d", var, i);
			success = GetGVarString(var_multi, text_multi, sizeof(text_multi), LANG_VAR_OFFSET + _:lang);

			if (success) {
				strcat(text, text_multi, size);
				i++;
			}
		} while (success);

		if (i != 0) {
			return 1;
		}
	}
	return success;
}

static stock _Lang_RemoveText(Lang:lang, var[])
{
	return DeleteGVar(var, LANG_VAR_OFFSET + _:lang);
}

static stock _Lang_IsTextExists(Lang:lang, var[])
{
	return GetGVarType(var, LANG_VAR_OFFSET + _:lang) != SERVER_VARTYPE_NONE;
}

/*
	Slot status
*/

static stock Lang:_Lang_GetFreeSlot()
{
	for (new Lang:l; l < Lang:MAX_LANGS; l++) {
		if (_Lang_IsSlotHaveStatus(l, LANG_SLOT_FREE)) {
			return l;
		}
	}

	return INVALID_LANG_ID;
}

static stock _Lang_GetSlotStatus(Lang:lang)
{
	return gLangSlots{_:lang};
}

static stock _Lang_SetSlotStatus(Lang:lang, status)
{
	gLangSlots{_:lang} = status;
}

static stock _Lang_IsSlotHaveStatus(Lang:lang, status)
{
	return !!(gLangSlots{_:lang} & status);
}

/*
	Other functions
*/

static stock _Lang_strcpy(result[], source[], const size = sizeof(result))
{
	result[0] = 0;
	return strcat(result, source, size);
}

static stock _Lang_pow(value, degree)
{
	new result = 1;
	while (degree--) {
		result *= value;
	}
	return result;
}

#if !defined _INC_y_va
stock va_format(out[], size, const fmat[], va_:STATIC_ARGS)
{
	static
		YSI_g_sHeap,
		YSI_g_sStack,
		YSI_g_sArgCount,
		YSI_g_sArgs[5];

	//P:C(if (_:STATIC_ARGS < 1) P:W("No static args found, please add a dummy local"););
	// Get the number of parameters.
	#emit LOAD.S.alt   STATIC_ARGS
	#emit DEC.alt      // 2 - n
	#emit SHL.C.alt    2
	// "alt" now contains the number of static arguments in bytes - 4.

	// Get the previous parameter count.
	#emit LOAD.S.pri   0
	#emit ADD.C        8
	#emit LOAD.I
	#emit SUB
	#emit ADD.C        8
	#emit STOR.pri     YSI_g_sArgCount // "format"s parameter count.

	// Get the address of the arguments we are replacing.
	#emit LOAD.S.pri   0
	#emit ADD

	// Copy them to our temporary buffer.
	#emit CONST.alt    YSI_g_sArgs
	#emit MOVS         16 // (n + 1) * 4

	// Go to the new "top" of the stack.
	#emit STACK        0
	#emit STOR.alt     YSI_g_sStack    // Store it.
	#emit ADD.C        16 // (n + 1) * 4
	#emit SCTRL        4

	// "frm" is still valid.
	#emit PUSH.S       fmat
	#emit PUSH.S       size
	#emit PUSH.S       out
	#emit PUSH         YSI_g_sArgCount // Push the parameter count.

	// Modify the heap to hold "locals".
	#emit HEAP         0
	#emit STOR.alt     YSI_g_sHeap
	#emit LCTRL        4
	#emit SCTRL        2

	// Call the function.
	#emit SYSREQ.C     format
	#emit STOR.pri     YSI_g_sArgCount

	// Copy the data back.
	#emit LOAD.pri     YSI_g_sHeap
	#emit SCTRL        2
	#emit STACK        0
	#emit CONST.pri    YSI_g_sArgs
	#emit MOVS         16
	#emit LOAD.pri     YSI_g_sStack
	#emit SCTRL        4

	#emit LOAD.pri     YSI_g_sArgCount
	#emit RETN

	return 0;
}
#endif
