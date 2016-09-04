# zlang
GVar per player lang system

# Requirements
- [GVar plugin](https://github.com/samp-incognito/samp-gvar-plugin)
- [foreach](https://github.com/karimcambridge/SAMP-foreach) - not necessary but recommended

# Libraries compatible with zlang
- [mdialog](https://github.com/Open-GTO/mdialog) - new look for Dialog_Open, Dialog_Message and Dialog_MessageEx functions.

# Defines
Directive | Default value | Can be redefined
----------|---------------|------------
MAX_LANGS | 2 | yes
LANG_VAR_OFFSET | 1000 | yes
LANG_IGNORED_FIRST_SYMBOL | '\0', '#', ';' | yes
MAX_LANG_VAR_STRING | 64 | yes
MAX_LANG_VALUE_STRING | 144 | yes
MAX_LANG_SEPARATOR_STRING | 64 | yes
MAX_LANG_MVALUE_STRING | MAX_LANG_VALUE_STRING * 25 | yes
MAX_LANG_CODE | 2 | yes
MAX_LANG_NAME | 16 | yes
MAX_LANG_FILES | 5 | yes
MAX_LANG_FILENAME | 256 | yes
INVALID_LANG_ID | Lang:-1 | no
INVALID_LANG_FILE_ID | -1 | no

# Functions
#### Add language
```Pawn
Lang:Lang_Add(code[], name[])
```

#### Remove language
```Pawn
Lang_Remove(Lang:lang)
```

#### Load language file
```Pawn
Lang_LoadFile(Lang:lang, filename[])
```

#### Unload language file
```Pawn
Lang_UnloadFile(Lang:lang, filename[] = "", fid = INVALID_LANG_FILE_ID)
```

#### Reload all language files
```Pawn
Lang_Reload(Lang:lang)
```

#### Get language id by *code* or *name*
```Pawn
Lang:Lang_Get(code[] = "", name[] = "")
```

#### Set language name
```Pawn
Lang_SetName(Lang:lang, name[])
```

#### Get language name
```Pawn
Lang_GetName(Lang:lang, name[], const size = sizeof(name))
```

#### Get language name and return it
```Pawn
Lang_ReturnName(Lang:lang)
```

#### Set language code
```Pawn
Lang_SetCode(Lang:lang, code[])
```

#### Get language code
```Pawn
Lang_GetCode(Lang:lang, code[], const size = sizeof(code))
```

#### Get language code and return it
```Pawn
Lang_ReturnCode(Lang:lang)
```

#### Get language codes string
```Pawn
Lang_GetCodes(result[], const size = sizeof(result), const separator = '/', const bool:isuppercase = false)
```

#### Get language codes string and return it
```Pawn
Lang_ReturnCodes(const separator = '/', const bool:isuppercase = false)
```

#### Check language on valid
```Pawn
Lang_IsValid(Lang:lang)
```

#### Get languages count
```Pawn
Lang_GetCount()
```

#### Set player language
```Pawn
Lang_SetPlayerLang(playerid, Lang:lang)
```

#### Get player language
```Pawn
Lang:Lang_GetPlayerLang(playerid)
```

#### Set player language by code
```Pawn
Lang:Lang_SetPlayerLangByCode(playerid, code[])
```

#### Set player language by name
```Pawn
Lang:Lang_SetPlayerLangByName(playerid, name[])
```

#### Set default server language
```Pawn
Lang_SetDefaultLang(Lang:lang)
```

#### Get default server language
```Pawn
Lang_GetDefaultLang()
```

#### Get language text
```Pawn
Lang_GetText(Lang:lang, var[], text[], const size = sizeof(text), {Float, _}:...)
```

#### Get player language text
```Pawn
Lang_GetPlayerText(playerid, var[], text[], const size = sizeof(text), {Float, _}:...)
```

#### Get default language text
```Pawn
Lang_GetDefaultText(var[], text[], const size = sizeof(text), {Float, _}:...)
```

#### Remove language text
```Pawn
Lang_RemoveText(Lang:lang, var[])
```

#### Is language text exists
```Pawn
Lang_IsTextExists(Lang:lang, var[])
```

#### Language printf function with default language
```Pawn
Lang_printf(var[], {Float, _}:...)
```

#### Language printf function with specific language
```Pawn
Lang_printfex(Lang:lang, var[], {Float, _}:...)
```

#### Language print function with default language
```Pawn
Lang_print(var[], {Float, _}:...)
```

#### Language print function with specific language
```Pawn
Lang_printex(Lang:lang, var[])
```

#### Send language text to player
```Pawn
Lang_SendText(playerid, var[], {Float, _}:...)
```

#### Send language text to all players
```Pawn
Lang_SendTextToAll(var[], {Float, _}:...)
```

#### Send language text to all players in array
```Pawn
Lang_SendTextToPlayers(players[], var[], {Float, _}:...)
```

#### Show language dialog to player
```Pawn
Lang_ShowDialog(playerid, dialogid, style, var_caption[], var_info[], var_button1[], var_button2[], {Float, _}:...)
```

#### Show language game text to player
```Pawn
Lang_GameText(playerid, time, style, var[], {Float, _}:...)
```

#### Show language game text to all players
```Pawn
Lang_GameTextForAll(time, style, var[], {Float, _}:...)
```

# Language file format
Language file format is a standard INI file format (without sections). It supports a variety of special characters, such as \n, \t, \%, \s, \\, \<value>, \x<hex>.

# Example
```Pawn
#include <a_samp>
#include <gvar>
#include <zlang>

enum e_LANG_INFO {
	Lang:e_LANG_EN,
	Lang:e_LANG_RU,
}

static
	gLang[e_LANG_INFO];

main() {}

public OnGameModeInit()
{
	// load languages
	gLang[e_LANG_RU] = Lang_Add("ru", "Russian");
	gLang[e_LANG_EN] = Lang_Add("en", "English");

	Lang_LoadFile(gLang[e_LANG_RU], "ru.ini");
	Lang_LoadFile(gLang[e_LANG_EN], "en.ini");

	// set english as the default language
	Lang_SetDefaultLanguage(gLang[e_LANG_EN]);
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
		Lang_SendText(playerid, "HELLO_MSG");
		Lang_SendText(playerid, "COMMANDS_LIST");
		return 1;
	}

	return 0;
}
```

**scriptfiles/en.ini:**
```ini
LANGUAGE_CHANGED = Now you are using english language.
HELLO_MSG = Hello, World!
COMMANDS_LIST = Commands: /help, /en, /ru
```

**scriptfiles/ru.ini:**
```ini
LANGUAGE_CHANGED = Теперь вы используете русский язык.
HELLO_MSG = Привет, Мир!
COMMANDS_LIST = Команды: /help, /en, /ru
```
