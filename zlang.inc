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
	#define MAX_LANGS 2
#endif

#if !defined MAX_LANG_CODE
	#define MAX_LANG_CODE 2
#endif

#if !defined MAX_LANG_NAME
	#define MAX_LANG_NAME 16
#endif

#if !defined MAX_LANG_FILENAME
	#define MAX_LANG_FILENAME 256
#endif

#define INVALID_LANG_ID -1

/*
	Enums
*/

enum e_LANG_INFO {
	e_lCode[MAX_LANG_CODE + 1],
	e_lName[MAX_LANG_NAME + 1],
}

/*
	Vars
*/

static
	gLangInfo[MAX_LANGS][E_LANG_INFO],
	gLangFiles[MAX_LANGS][MAX_LANG_FILES][MAX_LANG_FILENAME],
	bool:gLangIsValid[MAX_LANGS],
	Lang:gLangCount;

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

stock Lang:Lang_Add(code[], name[])
{
	if (isnull(code) || isnull(name)) {
		return INVALID_LANG_ID;
	}

	new
		Lang:lang;

	lang = Lang_Get(code);

	if (lang == INVALID_LANG_ID) {
		lang = _Lang_GetFreeSlot();
	}

	if (lang == INVALID_LANG_ID) {
		return lang;
	}

	Lang_SetCode(lang, code);
	Lang_SetName(lang, name);

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

	gLangIsValid[lang] = false;
	gLangCount--;
	return 1;
}

stock Lang_LoadFile(Lang:lang, filename[])
{
	if (!Lang_IsValid(lang)) {
		return 0;
	}

	for (new fid = 0; fid < sizeof(gLangFiles[]); fid++) {
		if (!strcmp(filename, gLangFiles[lang][fid], true)) {
			return 0;
		}
	}

	_Lang_strcpy(gLangFiles[lang][fid], filename, MAX_LANG_FILENAME);
	return 1;
}

stock Lang_UnloadFile(Lang:lang, filename[])
{

}

stock Lang_Reload(Lang:lang)
{

}

stock Lang:Lang_Get(code[])
{
	new
		Lang:lang = INVALID_LANG_ID;

	for (new Lang:l; l < MAX_LANGS; l++) {
		if (!strcmp(code, gLangInfo[l][e_lCode], true)) {
			return l;
		}
	}

	return lang;
}

stock Lang_SetName(lang, name[])
{
	return _Lang_strcpy(gLangInfo[lang][e_lName], name, MAX_LANG_NAME);
}

stock Lang_GetName(Lang:lang)
{
	return gLangInfo[lang][e_lName];
}

stock Lang_SetCode(Lang:lang, code[])
{
	return _Lang_strcpy(gLangInfo[lang][e_lCode], code, MAX_LANG_CODE);
}

stock Lang_GetCode(Lang:lang)
{
	return gLangInfo[lang][e_lCode];
}

stock bool:Lang_IsValid(Lang:lang)
{
	if (MAX_LANGS < _:lang < 0) {
		return false;
	}
	return gLangIsValid[lang];
}

stock Lang_GetCount()
{
	return gLangCount;
}

stock Lang_SetPlayerLang(playerid, Lang:lang)
{

}

stock Lang:Lang_GetPlayerLang(playerid)
{

}

stock Lang:Lang_SetPlayerLangByCode(playerid, code[])
{

}

/*
	Text functions
*/

stock _Lang_SetText(Lang:lang, var[], text[])
{

}

stock _Lang_GetText(Lang:lang, var[], text[], const size = sizeof(text))
{

}

stock _Lang_SendTextToPlayers(players[], var[], {Float, _}:...)
{

}

stock _Lang_SendTextToAll(var[], {Float, _}:...)
{

}

stock _Lang_SendText(playerid, var[], {Float, _}:...)
{

}

stock _Lang_ShowDialog(playerid, dialogid, style, caption[], info[], button1[], button2[], {Float, _}:...)
{

}

/*
	Private functions
*/

static stock _Lang_GetFreeSlot()
{
	new
		Lang:lang = INVALID_LANG_ID;

	for (new Lang:l; l < MAX_LANGS; l++) {
		if (!Lang_IsValid(l)) {
			return l;
		}
	}

	return lang;
}

static stock _Lang_strcpy(result[], source[], const size = sizeof(result))
{
	result[0] = 0;
	return strcat(result, source, size);
}
