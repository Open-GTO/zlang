/*

	About: GVar lang system
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

#if !defined MAX_MULTI_VAR_COUNT
	#define MAX_MULTI_VAR_COUNT 10
#endif

#if !defined MAX_LANGS_COUNT
	#define MAX_LANGS_COUNT 2
#endif

#if !defined ZLANG_GVAR_OFFSET
	#define ZLANG_GVAR_OFFSET 1000
#endif

#if !defined MAX_LANG_VAR_STRING
	#define MAX_LANG_VAR_STRING 64
#endif

#if !defined MAX_LANG_VALUE_STRING
	#define MAX_LANG_VALUE_STRING 144
#endif

#if !defined ZLANG_FILENAME_VAR
	#define ZLANG_FILENAME_VAR "zlang_langfile"
#endif

#if !defined ZLANG_MAX_FILENAME_PATH
	#define ZLANG_MAX_FILENAME_PATH 256
#endif

#define INVALID_LANG_ID -1
#define MAX_LANG_MULTI_STRING  (MAX_LANG_VALUE_STRING * MAX_MULTI_VAR_COUNT)

#define _(%0,%1) Lang_ReturnPlayerText(%0, #%1)
#define _m(%0,%1) Lang_ReturnPlayerMultiText(%0, #%1)

#define __(%0,%1,%2) Lang_GetPlayerText(%0, #%1, %2)
#define __m(%0,%1,%2) Lang_GetPlayerMultiText(%0, #%1, %2)

/*
	Vars
*/

static
	gLangSlots[MAX_LANGS_COUNT char],
	gPlayerLanguage[MAX_PLAYERS];

/*
	isnull
*/

#if !defined isnull
	#define isnull(%1) \
				((!(%1[0])) || (((%1[0]) == '\1') && (!(%1[1]))))
#endif

/*
	Loading functions
*/

stock Lang_LoadText(filename[])
{
	new langid = Lang_GetLoadedId(filename);
	if (langid != INVALID_LANG_ID) {
		Lang_SetActiveStatus(langid, true);
		return langid;
	}

	langid = Lang_Add();

	new File:flang = fopen(filename, io_read);
	if (!flang) {
		printf("Error: no such language file '%s'", filename);
		return INVALID_LANG_ID;
	}

	new
		i,
		sep_pos,
		varname[MAX_LANG_VAR_STRING + 1],
		string[MAX_LANG_VAR_STRING + 3 + MAX_LANG_VALUE_STRING + 1];

	while (fread(flang, string, sizeof(string))) {
		if (string[0] == '\0' || string[0] == '#' || string[0] == ';') {
			continue;
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

					if (Lang_IsTextExists(langid, varname)) {
						printf("Error: duplicated variable name %s in language %d", varname, langid);
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
						strdel(string, i, i + 1);
						strins(string, "%", i);
					}
					case 's': {
						strdel(string, i, i + 1);
						string[i] = ' ';
					}
					case '\\': {
						strdel(string, i, i + 1);
						string[i] = '\\';
					}
				}
			}
		}

		if (sep_pos != -1) {
			string[i] = '\0';
			Lang_SetText(langid, varname, string[sep_pos + 3]);
		}
	}

	fclose(flang);

	Lang_SetLoadedStatus(langid, filename, true);
	return langid;
}

static stock Lang_IsLoaded(value[])
{
	return Lang_GetLoadedId(value) != INVALID_LANG_ID;
}

static stock Lang_GetLoadedId(value[])
{
	if (isnull(value)) {
		return INVALID_LANG_ID;
	}

	new
		langid,
		langfile[ZLANG_MAX_FILENAME_PATH];

	for (langid = 0; langid < MAX_LANGS_COUNT; langid++) {
		if (Lang_IsActive(langid)) {
			continue;
		}

		GetGVarString(ZLANG_FILENAME_VAR, langfile, .id = langid + ZLANG_GVAR_OFFSET);

		if (isnull(langfile)) {
			continue;
		}

		if (strcmp(langfile, value, true) == 0) {
			return langid;
		}
	}
	return INVALID_LANG_ID;
}

static stock Lang_SetLoadedStatus(langid, value[], bool:status)
{
	if (status) {
		SetGVarString(ZLANG_FILENAME_VAR, value, langid + ZLANG_GVAR_OFFSET);
	} else {
		DeleteGVar(ZLANG_FILENAME_VAR, langid + ZLANG_GVAR_OFFSET);
	}
}

/*
	Text functions
*/

stock Lang_SetText(langid, varname[], value[])
{
	if (isnull(varname)) {
		return 0;
	}

	if (!Lang_IsActive(langid)) {
		return 0;
	}

	SetGVarString(varname, value, langid + ZLANG_GVAR_OFFSET);
	return 1;
}

stock Lang_GetText(langid, varname[], string[], size = sizeof(string))
{
	if (!Lang_IsActive(langid)) {
		return 0;
	}

	string[0] = '\0';

	if (!isnull(varname)) {
		new length = GetGVarString(varname, string, size, langid + ZLANG_GVAR_OFFSET);
		if (length == 0) {
			strcat(string, "Error: lang variable ", size);
			strcat(string, varname, size);
			strcat(string, " not found.", size);
		} else {
			return 1;
		}
	} else {
		strcat(string, "Error: lang varname is null", size);
	}
	return 0;
}

stock Lang_ReturnText(langid, varname[])
{
	static string[MAX_LANG_VALUE_STRING];
	Lang_GetText(langid, varname, string);
	return string;
}

/*
	Player text functions
*/

stock Lang_GetPlayerText(playerid, varname[], string[], size = sizeof(string))
{
	return Lang_GetText(Lang_GetPlayerLanguage(playerid), varname, string, size);
}

stock Lang_ReturnPlayerText(playerid, varname[])
{
	static string[MAX_LANG_VALUE_STRING];
	Lang_GetText(Lang_GetPlayerLanguage(playerid), varname, string);
	return string;
}

/*
	Multi text functions
*/

stock Lang_GetMultiText(langid, varname[], string[], size = sizeof(string))
{
	if (!Lang_IsActive(langid)) {
		return 0;
	}

	string[0] = '\0';

	if (!isnull(varname)) {
		new
			bool:is_success,
			length,
			var_value[MAX_LANG_VALUE_STRING],
			var_name[MAX_LANG_VAR_STRING + 6];

		for (new i = 0; i < MAX_MULTI_VAR_COUNT; i++) {
			format(var_name, sizeof(var_name), "%s_%d", varname, i);
			length = GetGVarString(var_name, var_value, sizeof(var_value), langid + ZLANG_GVAR_OFFSET);
			if (length != 0) {
				strcat(string, var_value, size);
				is_success = true;
			} else {
				if (i == 0) {
					strcat(string, "Error: multi lang value ", size);
					strcat(string, varname, size);
					strcat(string, " not found.", size);
				}
				break;
			}
		}
		return _:is_success;
	} else {
		strcat(string, "Error: multi lang varname is null", size);
	}
	return 0;
}

stock Lang_ReturnMultiText(langid, varname[])
{
	static string[MAX_LANG_MULTI_STRING];
	Lang_GetMultiText(langid, varname, string);
	return string;
}

/*
	Player multi text functions
*/

stock Lang_GetPlayerMultiText(playerid, varname[], string[], size = sizeof(string))
{
	return Lang_GetMultiText(Lang_GetPlayerLanguage(playerid), varname, string, size);
}

stock Lang_ReturnPlayerMultiText(playerid, varname[])
{
	static string[MAX_LANG_MULTI_STRING];
	Lang_GetMultiText(Lang_GetPlayerLanguage(playerid), varname, string);
	return string;
}

/*
	Other functions
*/

stock Lang_IsTextExists(langid, varname[])
{
	if (isnull(varname)) {
		return 0;
	}

	if (!Lang_IsActive(langid)) {
		return 0;
	}

	if (GetGVarType(varname, langid + ZLANG_GVAR_OFFSET) != SERVER_VARTYPE_NONE) {
		return 1;
	}

	return 0;
}

stock Lang_DeleteText(langid, varname[])
{
	if (isnull(varname)) {
		return 0;
	}

	if (!Lang_IsActive(langid)) {
		return 0;
	}

	DeleteGVar(varname, langid + ZLANG_GVAR_OFFSET);
	return 1;
}

/*
	Player language functions
*/

stock Lang_SetPlayerLanguage(playerid, langid)
{
	if (MAX_PLAYERS <= playerid < 0 || !IsPlayerConnected(playerid)) {
		return 0;
	}

	if (!Lang_IsActive(langid)) {
		return 0;
	}

	gPlayerLanguage[playerid] = langid;
	return 1;
}

stock Lang_GetPlayerLanguage(playerid)
{
	if (MAX_PLAYERS <= playerid < 0 || !IsPlayerConnected(playerid)) {
		return INVALID_LANG_ID;
	}

	return gPlayerLanguage[playerid];
}

/*
	Language slot functions
*/

static stock Lang_Add()
{
	new langid = Lang_GetFreeSlot();
	if (langid == INVALID_LANG_ID) {
		return INVALID_LANG_ID;
	}

	Lang_SetActiveStatus(langid, true);
	return langid;
}

static stock Lang_Remove(langid)
{
	if (!Lang_IsActive(langid)) {
		return 0;
	}

	Lang_SetActiveStatus(langid, false);
	return 1;
}

static stock Lang_GetFreeSlot()
{
	for (new langid = 0; langid < MAX_LANGS_COUNT; langid++) {
		if (!Lang_IsActive(langid)) {
			return langid;
		}
	}
	return INVALID_LANG_ID;
}

static stock Lang_SetActiveStatus(langid, bool:status)
{
	if (Lang_IsActive(langid)) {
		return 0;
	}

	gLangSlots{langid} = _:status;
	return 1;
}

static stock Lang_IsActive(langid)
{
	if (langid < 0 || langid >= MAX_LANGS_COUNT) {
		return 0;
	}
	return gLangSlots{langid} == 1;
}
