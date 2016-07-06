# zlang
GVar lang system

# Requirements
- [GVar plugin](https://github.com/samp-incognito/samp-gvar-plugin)

# Defines
Directive | Default value | Can be redefined
----------|---------------|------------
MAX_MULTI_VAR_COUNT | 10 | yes
MAX_LANGS_COUNT | 2 | yes
ZLANG_GVAR_OFFSET | 1000 | yes
MAX_LANG_VAR_STRING | 64 | yes
MAX_LANG_VALUE_STRING | 144 | yes
ZLANG_FILENAME_VAR | "zlang_langfile" | yes
ZLANG_MAX_FILENAME_PATH | 256 | yes
INVALID_LANG_ID | -1 | no
MAX_LANG_MULTI_STRING | (MAX_LANG_VALUE_STRING * MAX_MULTI_VAR_COUNT) | no

# Functions
```Pawn
Lang_LoadText(filename[]);
Lang_SetText(langid, varname[], value[]);
Lang_GetText(langid, varname[], string[], size = sizeof(string));
Lang_ReturnText(langid, varname[]);
Lang_GetPlayerText(playerid, varname[], string[], size = sizeof(string)); // __(playerid, varname[], string[], size = sizeof(string))
Lang_ReturnPlayerText(playerid, varname[]); // _(playerid, varname[])
Lang_GetMultiText(langid, varname[], string[], size = sizeof(string));
Lang_ReturnMultiText(langid, varname[]);
Lang_GetPlayerMultiText(playerid, varname[], string[], size = sizeof(string)); // __m(playerid, varname[], string[], size = sizeof(string))
Lang_ReturnPlayerMultiText(playerid, varname[]); // _m(playerid, varname[])
Lang_IsTextExists(langid, varname[]);
Lang_DeleteText(langid, varname[]);
Lang_SetPlayerLanguage(playerid, langid);
Lang_GetPlayerLanguage(playerid);
```

# Example
```Pawn
#include <a_samp>
#include <gvar>
#include <zlang>

enum e_LANG_INFO {
	e_LANG_EN,
	e_LANG_RU,
}

static
	gLang[e_LANG_INFO];

main() {}

public OnGameModeInit()
{
	// load languages
	gLang[e_LANG_EN] = Lang_LoadText("en.ini");
	gLang[e_LANG_RU] = Lang_LoadText("ru.ini");
	return 1;
}

public OnPlayerConnect(playerid)
{
	// set default language to english
	Lang_SetPlayerLanguage(playerid, gLang[e_LANG_EN]);
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	if (strcmp(cmdtext, "/ru", true) == 0) {
		Lang_SetPlayerLanguage(playerid, gLang[e_LANG_RU]);
		SendClientMessage(playerid, -1, _(playerid, LANGUAGE_CHANGED));
		return 1;
	}

	if (strcmp(cmdtext, "/en", true) == 0) {
		Lang_SetPlayerLanguage(playerid, gLang[e_LANG_EN]);
		SendClientMessage(playerid, -1, _(playerid, LANGUAGE_CHANGED));
		return 1;
	}

	if (strcmp(cmdtext, "/help", true) == 0) {
		SendClientMessage(playerid, -1, _(playerid, HELLO_MSG));
		SendClientMessage(playerid, -1, _(playerid, COMMANDS_LIST));
		return 1;
	}

	return 0;
}
```

**scriptfiles/en.ini:**
```ini
LANGUAGE_CHANGED = Now you are using english language.
HELLO_MSG = Hello, World!
COMMANDS_LIST = Commands: /help, /level, /report
```

**scriptfiles/ru.ini:**
```ini
LANGUAGE_CHANGED = Теперь вы используете русский язык.
HELLO_MSG = Привет, Мир!
COMMANDS_LIST = Команды: /help, /level, /report
```
