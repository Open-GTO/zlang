#include <a_samp>
#include <gvar>
#include "zlang.inc"

enum e_LANG_INFO {
	Lang:e_LANG_EN,
	Lang:e_LANG_RU,
}

static
	gLang[e_LANG_INFO];

main()
{
	// load languages
	gLang[e_LANG_RU] = Lang_Add("ru", "Russian");
	gLang[e_LANG_EN] = Lang_Add("en", "English");

	Lang_LoadFile(gLang[e_LANG_RU], "ru.ini");
	Lang_LoadFile(gLang[e_LANG_EN], "en.ini");

	Lang_printf("COMMANDS_LIST");

	// set english as the default language
	Lang_SetDefaultLang(gLang[e_LANG_EN]);
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	if (strcmp(cmdtext, "/ru", true, 3) == 0) {
		Lang_SetPlayerLang(playerid, gLang[e_LANG_RU]);
		Lang_SendText(playerid, "LANGUAGE_CHANGED");
		return 1;
	}

	if (strcmp(cmdtext, "/en", true, 3) == 0) {
		Lang_SetPlayerLang(playerid, gLang[e_LANG_EN]);
		Lang_SendText(playerid, "LANGUAGE_CHANGED");
		return 1;
	}

	if (strcmp(cmdtext, "/help", true, 5) == 0) {
		Lang_SendText(playerid, "{FF0000}HELLO_MSG");
		Lang_SendText(playerid, "COMMANDS_LIST");
		return 1;
	}

	if (strcmp(cmdtext, "/help330", true, 5) == 0) {
		Lang_SendText(playerid, "Hello, World!");
		Lang_SendText(playerid, "Commands: /help, /en, /ru");
		return 1;
	}

	return 0;
}