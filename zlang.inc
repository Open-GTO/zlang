/*

	About: GVar per player lang system
	Author: ziggi

*/

#if !defined _samp_included
	#error Please include a_samp or a_npc before zlang
#endif

#if defined _zlang_included
	#endinput
#endif

#define _zlang_included

/*
	SVar and GVar
*/

#if !defined LANG_USE_SVAR && !defined LANG_USE_GVAR
	#define LANG_USE_GVAR
#endif

#if defined LANG_USE_SVAR && defined LANG_USE_GVAR
	#error You cannot use LANG_USE_SVAR and LANG_USE_GVAR at the same time
#endif

#if defined LANG_USE_GVAR
	#if !defined SetGVarString
		#tryinclude "gvar"
	#endif

	#if !defined SetGVarString
		#tryinclude <gvar>
	#endif

	#if !defined SetGVarString
		#error Please include gvar before zlang
	#endif
#endif

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
	#define MAX_LANG_VAR_STRING (144)
#endif

#if !defined MAX_LANG_VALUE_STRING
	#define MAX_LANG_VALUE_STRING (144)
#endif

#if !defined MAX_LANG_SEPARATOR_STRING
	#define MAX_LANG_SEPARATOR_STRING (64)
#endif

#if !defined MAX_LANG_MVALUE_STRING
	#define MAX_LANG_MVALUE_STRING (MAX_LANG_VALUE_STRING * 25)
#endif

#if !defined MAX_LANG_FORMAT_STRING
	#define MAX_LANG_FORMAT_STRING (MAX_LANG_VALUE_STRING)
#endif

#if !defined MAX_LANG_MFORMAT_STRING
	#define MAX_LANG_MFORMAT_STRING (MAX_LANG_MVALUE_STRING)
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

#if defined LANG_USE_SVAR
	#if !defined MAX_LANG_PREFIX_SVAR_STRING
		#define MAX_LANG_PREFIX_SVAR_STRING (7 + MAX_LANG_VAR_STRING)
	#endif

	#if !defined LANG_SVAR_VARNAME_MASK
		#define LANG_SVAR_VARNAME_MASK "lng%d_%s"
	#endif
#endif

#define INVALID_LANG_ID (Lang:-1)
#define INVALID_LANG_FILE_ID (-1)

#define lang_args<%0> {Float, _}:...
#define lang_start<%0> (lang_:(%0))

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
	gLangCount,
	Lang:gDefaultLang,
	Lang:gPlayerLang[MAX_PLAYERS];

/*
	Forwards
*/

forward Lang:Lang_Get(const code[] = "", const name[] = "");
forward Lang:_Lang_GetFreeSlot();

/*
	OnGameModeInit
*/

#if defined _inc_y_hooks || defined _INC_y_hooks
	hook OnGameModeInit()
#else
	public OnGameModeInit()
#endif
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

#if !defined _inc_y_hooks && !defined _INC_y_hooks
	#if defined _ALS_OnGameModeInit
		#undef OnGameModeInit
	#else
		#define _ALS_OnGameModeInit
	#endif

	#define OnGameModeInit ZLANG_OnGameModeInit
	#if defined ZLANG_OnGameModeInit
		forward ZLANG_OnGameModeInit();
	#endif
#endif

/*
	OnGameModeExit
*/
#if defined _inc_y_hooks || defined _INC_y_hooks
	hook OnGameModeExit()
#else
	public OnGameModeExit()
#endif
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

#if !defined _inc_y_hooks && !defined _INC_y_hooks
	#if defined _ALS_OnGameModeExit
		#undef OnGameModeExit
	#else
		#define _ALS_OnGameModeExit
	#endif

	#define OnGameModeExit ZLANG_OnGameModeExit
	#if defined ZLANG_OnGameModeExit
		forward ZLANG_OnGameModeExit();
	#endif
#endif

/*
	Loading functions
*/

stock Lang:Lang_Add(const code[], const name[])
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

stock Lang_LoadFile(Lang:lang, const filename[])
{
	if (!Lang_IsValid(lang)) {
		return INVALID_LANG_FILE_ID;
	}

	if (isnull(filename)) {
		return INVALID_LANG_FILE_ID;
	}

	new fid = _Lang_FindFile(lang, filename);
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
		sep_found,
		sep_spos,
		sep_epos,
		varname[MAX_LANG_VAR_STRING + 1],
		string[MAX_LANG_VAR_STRING + MAX_LANG_SEPARATOR_STRING + MAX_LANG_VALUE_STRING + 1],
		idx,
		symbol,
		length,
		bool:is_process_var = false;

	while (fread(flang, string, sizeof(string))) {
		switch (string[0]) {
			case LANG_IGNORED_FIRST_SYMBOL: {
				continue;
			}
		}

		sep_found = _Lang_FindSeparatorPositions(string, sep_spos, sep_epos);
		if (!sep_found) {
			continue;
		}

		if ((sep_epos - sep_spos) > MAX_LANG_SEPARATOR_STRING) {
			printf("Error: length of lang separator more than %d.", MAX_LANG_SEPARATOR_STRING);
			printf("Error: %s", string);
		}

		if ((sep_spos - 1) > MAX_LANG_VAR_STRING) {
			printf("Error: length of lang variable more than %d.", MAX_LANG_VAR_STRING);
			printf("Error: %s", string);
		}

		strmid(varname, string, 0, sep_spos);

		if (_Lang_IsTextExists(lang, varname)) {
			printf("Error: duplicated variable name %s in language '%s'", varname, Lang_ReturnName(lang));
			break;
		}

		for (i = 0; string[i] >= ' ' || string[i] == '\t'; i++) {
			if (string[i] != '\\') {
				continue;
			}

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

					// gvar does not support symbols with negative codes
					if (string[i] < 0) {
						string[i] = ' ';
					}

					strdel(string, i + 1, i + length + 1);
				}
				case 'v': {
					is_process_var = true;
				}
			}
		}

		string[i] = '\0';
		_Lang_SetText(lang, varname, string[sep_epos]);

		if (is_process_var) {
			is_process_var = false;
			_Lang_ProcessVar(lang, varname);
		}
	}

	fclose(flang);
	return fid;
}

stock Lang_UnloadFile(Lang:lang, const filename[] = "", fid = INVALID_LANG_FILE_ID)
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
		sep_found,
		sep_spos,
		sep_epos,
		string[MAX_LANG_VAR_STRING + MAX_LANG_SEPARATOR_STRING + MAX_LANG_VALUE_STRING + 1];

	while (fread(flang, string, sizeof(string))) {
		switch (string[0]) {
			case LANG_IGNORED_FIRST_SYMBOL: {
				continue;
			}
		}

		sep_found = _Lang_FindSeparatorPositions(string, sep_spos, sep_epos);
		if (!sep_found) {
			continue;
		}

		strmid(string, string, 0, sep_spos);
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

stock Lang:Lang_Get(const code[] = "", const name[] = "")
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

stock Lang_SetName(Lang:lang, const name[])
{
	if (name{0} != 0) {
		return strunpack(gLangInfo[_:lang][e_zlName], name, MAX_LANG_NAME + 1);
	}
	return _Lang_strcpy(gLangInfo[_:lang][e_zlName], name, MAX_LANG_NAME + 1);
}

stock Lang_GetName(Lang:lang, name[], const size = sizeof(name))
{
	_Lang_strcpy(name, gLangInfo[_:lang][e_zlName], size);
}

stock Lang_ReturnName(Lang:lang)
{
	new name[MAX_LANG_NAME + 1];
	Lang_GetName(lang, name);
	return name;
}

stock Lang_SetCode(Lang:lang, const code[])
{
	if (code{0} != 0) {
		return strunpack(gLangInfo[_:lang][e_zlCode], code, MAX_LANG_CODE + 1);
	}
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

stock Lang:Lang_SetPlayerLangByCode(playerid, const code[])
{
	new Lang:lang = Lang_Get(code);
	if (lang == INVALID_LANG_ID) {
		return INVALID_LANG_ID;
	}

	gPlayerLang[playerid] = lang;
	return lang;
}

stock Lang:Lang_SetPlayerLangByName(playerid, const name[])
{
	new Lang:lang = Lang_Get(.name = name);
	if (lang == INVALID_LANG_ID) {
		return INVALID_LANG_ID;
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

stock Lang_SetText(Lang:lang, const var[], const text[])
{
	if (isnull(var)) {
		return 0;
	}

	if (!Lang_IsValid(lang)) {
		return 0;
	}

	static
		var_temp[MAX_LANG_VAR_STRING];

	if (var{0} != 0) {
		strunpack(var_temp, var);
	} else {
		_Lang_strcpy(var_temp, var);
	}

	new res = _Lang_SetText(lang, var_temp, text);

	if (strfind(text, "\\v(", true) != -1) {
		_Lang_ProcessVar(lang, var_temp);
	}

	return res;
}

stock Lang_GetText(Lang:lang, const var[], text[], const size = sizeof(text), lang_args<>)
{
	if (!Lang_IsValid(lang)) {
		_Lang_strcpy(text, "Error: lang is invalid.", size);
		return 0;
	}

	if (isnull(var)) {
		_Lang_strcpy(text, "Error: lang var is null.", size);
		return 0;
	}

	if (strlen(var) >= MAX_LANG_VAR_STRING) {
		_Lang_strcpy(text, var, size);
		return 2;
	}

	static
		var_temp[MAX_LANG_VAR_STRING],
		bool:is_colored,
		color[9];

	if (var{0} != 0) {
		strunpack(var_temp, var);
	} else {
		_Lang_strcpy(var_temp, var);
	}

	is_colored = false;
	if (var_temp[0] == '{' && var_temp[7] == '}') {
		strmid(color, var_temp, 0, 8);
		strdel(var_temp, 0, 8);
		is_colored = true;
	}

	static
		temp[MAX_LANG_MFORMAT_STRING];

	new success = _Lang_GetText(lang, var_temp, temp);
	if (!success) {
		text[0] = '\0';

		if (is_colored) {
			strcat(text, color, size);
		}

		strcat(text, var_temp, size);
		return 2;
	}

	if (numargs() > 4) {
		Lang_format(text, size, temp, lang_start<4>);

		if (is_colored) {
			format(text, size, "%s%s", color, text);
		}
	} else {
		text[0] = '\0';

		if (is_colored) {
			strcat(text, color, size);
		}

		strcat(text, temp, size);
	}

	return 1;
}

stock Lang_RemoveText(Lang:lang, const var[])
{
	return _Lang_RemoveText(lang, var);
}

stock Lang_IsTextExists(Lang:lang, const var[])
{
	if (isnull(var)) {
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

stock Lang_printf(const var[], lang_args<>)
{
	static
		Lang:lang,
		text[MAX_LANG_MFORMAT_STRING],
		success;

	lang = Lang_GetDefaultLang();

	success = Lang_GetText(lang, var, text);
	Lang_format(text, sizeof(text), text, lang_start<1>);
#if defined ENABLE_LANG_DYNAMIC_VARS
	Lang_ProcessDynamicVars(lang, text);
#endif

	print(text);

	return success;
}

stock Lang_printfex(Lang:lang, const var[], lang_args<>)
{
	static
		Lang:lang,
		text[MAX_LANG_MFORMAT_STRING],
		success;

	lang = Lang_GetDefaultLang();

	success = Lang_GetText(lang, var, text);
	Lang_format(text, sizeof(text), text, lang_start<2>);
#if defined ENABLE_LANG_DYNAMIC_VARS
	Lang_ProcessDynamicVars(lang, text);
#endif

	print(text);

	return success;
}

stock Lang_print(const var[])
{
	static
		text[MAX_LANG_MVALUE_STRING],
		success;

	success = Lang_GetText(Lang_GetDefaultLang(), var, text);
	print(text);

	return success;
}

stock Lang_printex(Lang:lang, const var[])
{
	static
		text[MAX_LANG_MVALUE_STRING],
		success;

	success = Lang_GetText(lang, var, text);
	print(text);
	return success;
}

/*
	Player text functions
*/

stock Lang_SendText(playerid, const var[], lang_args<>)
{
	static
		Lang:lang,
		text[MAX_LANG_FORMAT_STRING];

	lang = Lang_GetPlayerLang(playerid);

	Lang_GetText(lang, var, text);

#if !defined _INC_open_mp
	Lang_format(text, sizeof(text), text, lang_start<2>);
#endif

#if defined ENABLE_LANG_DYNAMIC_VARS
	Lang_ProcessDynamicVars(lang, text);
#endif

#if !defined _INC_open_mp
	return SendClientMessage(playerid, -1, text);
#else
	return Lang_SendClientMessage(playerid, text, lang_start<2>);
#endif
}

stock Lang_SendTextToAll(const var[], lang_args<>)
{
	static
		playerid,
		Lang:lang,
		text[MAX_LANG_FORMAT_STRING];

#if defined foreach
	foreach (playerid : Player) {
#else
	for (playerid = GetPlayerPoolSize(); playerid != -1; playerid--) {
		if (!IsPlayerConnected(playerid)) {
			continue;
		}
#endif
		lang = Lang_GetPlayerLang(playerid);

		Lang_GetText(lang, var, text);
		Lang_format(text, sizeof(text), text, lang_start<1>);
#if defined ENABLE_LANG_DYNAMIC_VARS
		Lang_ProcessDynamicVars(lang, text);
#endif

		SendClientMessage(playerid, -1, text);
	}
	return 1;
}

stock Lang_SendTextToPlayers(const players[], const var[], lang_args<>)
{
	static
		idx,
		Lang:lang,
		text[MAX_LANG_FORMAT_STRING];

	for (idx = 0; players[idx] != INVALID_PLAYER_ID; idx++) {
		lang = Lang_GetPlayerLang(players[idx]);

		Lang_GetText(lang, var, text);
		Lang_format(text, sizeof(text), text, lang_start<2>);
#if defined ENABLE_LANG_DYNAMIC_VARS
		Lang_ProcessDynamicVars(lang, text);
#endif

		SendClientMessage(players[idx], -1, text);
	}
	return 1;
}

stock Lang_ShowDialog(playerid, dialogid, style, const var_caption[], const var_info[], const var_button1[], const var_button2[], lang_args<>)
{
	static
		Lang:lang,
		caption[MAX_LANG_VALUE_STRING],
		button1[MAX_LANG_VALUE_STRING],
		button2[MAX_LANG_VALUE_STRING],
		info[MAX_LANG_MFORMAT_STRING];

	lang = Lang_GetPlayerLang(playerid);

	Lang_GetText(lang, var_caption, caption);
	Lang_GetText(lang, var_button1, button1);
	if (!isnull(var_button2)) {
		Lang_GetText(lang, var_button2, button2);
	}
	Lang_GetText(lang, var_info, info);
	Lang_format(info, sizeof(info), info, lang_start<7>);
#if defined ENABLE_LANG_DYNAMIC_VARS
	Lang_ProcessDynamicVars(lang, info);
#endif

	return ShowPlayerDialog(playerid, dialogid, style, caption, info, button1, button2);
}

stock Lang_GameText(playerid, time, style, const var[], lang_args<>)
{
	static
		Lang:lang,
		text[MAX_LANG_FORMAT_STRING];

	lang = Lang_GetPlayerLang(playerid);

	Lang_GetText(lang, var, text);
	Lang_format(text, sizeof(text), text, lang_start<4>);
#if defined ENABLE_LANG_DYNAMIC_VARS
	Lang_ProcessDynamicVars(lang, text);
#endif

	return GameTextForPlayer(playerid, text, time, style);
}

stock Lang_GameTextForAll(time, style, const var[], lang_args<>)
{
	static
		playerid,
		Lang:lang,
		text[MAX_LANG_FORMAT_STRING];

#if defined foreach
	foreach (playerid : Player) {
#else
	for (playerid = GetPlayerPoolSize(); playerid != -1; playerid--) {
		if (!IsPlayerConnected(playerid)) {
			continue;
		}
#endif
		lang = Lang_GetPlayerLang(playerid);

		Lang_GetText(lang, var, text);
		Lang_format(text, sizeof(text), text, lang_start<3>);
#if defined ENABLE_LANG_DYNAMIC_VARS
		Lang_ProcessDynamicVars(lang, text);
#endif

		GameTextForPlayer(playerid, text, time, style);
	}

	return GameTextForAll(text, time, style);
}

stock PlayerText:Lang_CreatePlayerTextDraw(playerid, Float:x, Float:y, const var[], lang_args<>)
{
	static
		Lang:lang,
		text[MAX_LANG_FORMAT_STRING];

	lang = Lang_GetPlayerLang(playerid);

	Lang_GetText(lang, var, text);
	Lang_format(text, sizeof(text), text, lang_start<4>);
#if defined ENABLE_LANG_DYNAMIC_VARS
	Lang_ProcessDynamicVars(lang, text);
#endif

	return CreatePlayerTextDraw(playerid, x, y, text);
}

stock Lang_PlayerTextDrawSetString(playerid, PlayerText:text, const var[], lang_args<>)
{
	static
		Lang:lang,
		string[MAX_LANG_FORMAT_STRING];

	lang = Lang_GetPlayerLang(playerid);

	Lang_GetText(lang, var, string);
	Lang_format(string, sizeof(string), string, lang_start<3>);
#if defined ENABLE_LANG_DYNAMIC_VARS
	Lang_ProcessDynamicVars(lang, string);
#endif

	return PlayerTextDrawSetString(playerid, text, string);
}

stock PlayerText3D:Lang_CreatePlayer3DTextLabel(playerid, const var[], color, Float:x, Float:y, Float:z, Float:DrawDistance, attachedplayer = INVALID_PLAYER_ID, attachedvehicle = INVALID_VEHICLE_ID, testLOS = 0, lang_args<>)
{
	static
		Lang:lang,
		text[MAX_LANG_FORMAT_STRING];

	lang = Lang_GetPlayerLang(playerid);

	Lang_GetText(lang, var, text);
	Lang_format(text, sizeof(text), text, lang_start<10>);
#if defined ENABLE_LANG_DYNAMIC_VARS
	Lang_ProcessDynamicVars(lang, text);
#endif

	return CreatePlayer3DTextLabel(playerid, text, color, x, y, z, DrawDistance, attachedplayer, attachedvehicle, testLOS);
}

stock Lang_UpdatePlayer3DTextLabel(playerid, PlayerText3D:id, color, const var[], lang_args<>)
{
	static
		Lang:lang,
		text[MAX_LANG_FORMAT_STRING];

	lang = Lang_GetPlayerLang(playerid);

	Lang_GetText(lang, var, text);
	Lang_format(text, sizeof(text), text, lang_start<4>);
#if defined ENABLE_LANG_DYNAMIC_VARS
	Lang_ProcessDynamicVars(lang, text);
#endif

	return UpdatePlayer3DTextLabelText(playerid, id, color, text);
}

/*
	Vars
*/

static stock _Lang_SetVarString(Lang:lang, const var[], const text[])
{
#if defined LANG_USE_GVAR
	return SetGVarString(var, text, LANG_VAR_OFFSET + _:lang);
#endif

#if defined LANG_USE_SVAR
	static
		varname[MAX_LANG_PREFIX_SVAR_STRING + 1];

	_Lang_ConvertVar(lang, var, varname);
	return SetSVarString(varname, text);
#endif
}

static stock _Lang_GetVarString(Lang:lang, const var[], text[], const size = sizeof(text))
{
#if defined LANG_USE_GVAR
	return GetGVarString(var, text, size, LANG_VAR_OFFSET + _:lang);
#endif

#if defined LANG_USE_SVAR
	static
		varname[MAX_LANG_PREFIX_SVAR_STRING + 1];

	_Lang_ConvertVar(lang, var, varname);
	return GetSVarString(varname, text, size);
#endif
}

static stock _Lang_DeleteVar(Lang:lang, const var[])
{
#if defined LANG_USE_GVAR
	return DeleteGVar(var, LANG_VAR_OFFSET + _:lang);
#endif

#if defined LANG_USE_SVAR
	static
		varname[MAX_LANG_PREFIX_SVAR_STRING + 1];

	_Lang_ConvertVar(lang, var, varname);
	return DeleteSVar(varname);
#endif
}

/*
	SVar functions
*/

#if defined LANG_USE_SVAR

static stock _Lang_ConvertVar(Lang:lang, const var[], varname[], const size = sizeof(varname))
{
	format(varname, size, LANG_SVAR_VARNAME_MASK, _:lang, var);
}

#endif

/*
	Private functions
*/

static stock _Lang_FindFile(Lang:lang, const filename[])
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

static stock _Lang_SetText(Lang:lang, const var[], const text[])
{
	return _Lang_SetVarString(lang, var, text);
}

static stock _Lang_GetText(Lang:lang, const var[], text[], const size = sizeof(text))
{
	static
		success,
		i,
		text_multi[MAX_LANG_VALUE_STRING],
		var_multi[MAX_LANG_VAR_STRING + 6];

	success = _Lang_GetVarString(lang, var, text, size);

	if (!success) {
		i = 0;

		do {
			format(var_multi, sizeof(var_multi), "%s_%d", var, i);
			success = _Lang_GetVarString(lang, var_multi, text_multi, sizeof(text_multi));

			if (success) {
				if (i == 0) {
					text[0] = '\0';
				}

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

static stock _Lang_RemoveText(Lang:lang, const var[])
{
	return _Lang_DeleteVar(lang, var);
}

static stock _Lang_IsTextExists(Lang:lang, const var[])
{
#if defined LANG_USE_GVAR
	return GetGVarType(var, LANG_VAR_OFFSET + _:lang) != GLOBAL_VARTYPE_NONE;
#endif

#if defined LANG_USE_SVAR
	static
		varname[MAX_LANG_PREFIX_SVAR_STRING + 1];

	_Lang_ConvertVar(lang, var, varname);
	return GetSVarType(varname) != SERVER_VARTYPE_NONE;
#endif
}

static stock _Lang_ProcessVar(Lang:lang, const var[])
{
	new
		spos,
		epos,
		varvalue[MAX_LANG_VALUE_STRING + 1],
		vartmp[MAX_LANG_VAR_STRING + 1],
		valtmp[MAX_LANG_VALUE_STRING + 1];

	_Lang_GetText(lang, var, varvalue);

	while ((spos = strfind(varvalue, "\\v(", true, spos)) != -1) {
		epos = strfind(varvalue, ")", true, spos);
		if (epos == -1) {
			printf("Error: error with using language variables, example of usage: \\v(VARIABLE)");
			return;
		}

		strmid(vartmp, varvalue, spos + 3, epos);

		if (vartmp[0] == '%' && vartmp[1] == 's') {
			spos += 1;
			_Lang_SetText(lang, var, varvalue);
			return;
		}

		_Lang_GetText(lang, vartmp, valtmp);

		strdel(varvalue, spos, epos + 1);
		format(varvalue, sizeof(varvalue), "%.*s%s%s", spos, varvalue, valtmp, varvalue[spos]);

		_Lang_SetText(lang, var, varvalue);
	}
}

stock Lang_ProcessDynamicVars(Lang:lang, text[], const size = sizeof(text))
{
	new
		epos,
		spos,
		vartmp[MAX_LANG_VAR_STRING + 1],
		valtmp[MAX_LANG_VALUE_STRING + 1];

	while ((spos = strfind(text, "\\v(", true)) != -1) {
		epos = strfind(text, ")", true, spos);
		if (epos == -1) {
			printf("Error: error with using language variables, example of usage: \\v(VARIABLE)");
			break;
		}

		strmid(vartmp, text, spos + 3, epos);

		_Lang_GetText(lang, vartmp, valtmp);

		strdel(text, spos, epos + 1);
		format(text, size, "%.*s%s%s", spos, text, valtmp, text[spos]);
	}
}

static stock _Lang_FindSeparatorPositions(const string[], &start_pos, &end_pos)
{
	new
		sep_spos = -1,
		sep_epos = -1;

	for (new i = 0; string[i] >= ' ' || string[i] == '\t'; i++) {
		if (sep_spos < 0 || sep_epos < 0) {
			// get the separator position
			if (sep_spos == -1) {
				if (string[i] == ' ' || string[i] == '\t') {
					sep_spos = i;
				}
			} else if (sep_epos == -1) {
				if (string[i] == '=') {
					sep_epos = -2;
				} else {
					sep_spos = -1;
				}
			} else if (sep_epos == -2) {
				if (string[i] != ' ' && string[i] != '\t') {
					sep_epos = i;
				}
			}

			// if a separator is found
			if (sep_spos > 0 && sep_epos > 0) {
				start_pos = sep_spos;
				end_pos = sep_epos;
				return 1;
			}
		}
	}
	return 0;
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

static stock _Lang_strcpy(result[], const source[], const size = sizeof(result))
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

forward _Lang_SysreqFix();
public _Lang_SysreqFix()
{
	new temp[2];
	format(temp, sizeof(temp), "");
}

// taken from YSI
stock Lang_format(out[], size, const fmat[], lang_:STATIC_ARGS)
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

// open.mp
#if defined _INC_open_mp

static stock Lang_SendClientMessage(playerid, const fmat[], lang_:STATIC_ARGS)
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
	#emit PUSH.C       0xFFFFFFFF
	#emit PUSH.S       playerid
	#emit PUSH         YSI_g_sArgCount // Push the parameter count.

	// Modify the heap to hold "locals".
	#emit HEAP         0
	#emit STOR.alt     YSI_g_sHeap
	#emit LCTRL        4
	#emit SCTRL        2

	// Call the function.
	#emit SYSREQ.C     SendClientMessage
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
