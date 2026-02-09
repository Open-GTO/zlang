# zlang

Per-player language system using GVar or SVar.

The [GVar plugin](https://github.com/samp-incognito/samp-gvar-plugin) is optional, but recommended when using SA-MP.
If the plugin is not used (i.e. `gvar` is not included before `zlang`), **SVar** will be used instead.

You can explicitly control this behavior using:

- `#define LANG_USE_SVAR` - force SVar mode.
- `#define LANG_USE_GVAR` - force GVar mode.

The [foreach](https://github.com/Open-GTO/foreach) library is optional, but recommended.

Also take a look at the [mdialog](https://github.com/Open-GTO/mdialog) library, which provides updated versions of `Dialog_Open`, `Dialog_Message`, and `Dialog_MessageEx` with zlang support.


# Basic usage

```Pawn
#include <a_samp>
#include <gvar> // optional
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
```

**scriptfiles/en.ini:**
```ini
COLOR_GRAY = {CCCCCC}
LANGUAGE_CHANGED = \v(COLOR_GRAY)Now you are using english language.
HELLO_MSG = Hello, {00FF00}World!
COMMANDS_LIST = Commands: /help, /en, /ru
```

**scriptfiles/ru.ini:**
```ini
COLOR_GRAY = {CCCCCC}
LANGUAGE_CHANGED = \v(COLOR_GRAY)Теперь вы используете русский язык.
HELLO_MSG = Привет, {00FF00}Мир!
COMMANDS_LIST = Команды: /help, /en, /ru
# since zlang 3.3.0
Hello, World! = \v(HELLO_MSG)
Commands: /help, /en, /ru = \v(COMMANDS_LIST)
```

# Language file format

Language file format is a standard INI file format (without sections). It supports a variety of special characters, such as `\n`, `\t`, `\%`, `\s`, `\\`, `\v(<value>)`, `\x<hex>`.

# Variables

You can use variables in your text files. This supports any nesting level.
```ini
HELLO_MSG = Hello
COLOR_RED = {FF0000}
COLOR_GREEN = {00FF00}
COLOR_WHITE = {FFFFFF}
NAME_ONE = \v(COLOR_RED)Alex\v(COLOR_WHITE)
NAME_TWO = \v(COLOR_GREEN)Peter\v(COLOR_WHITE)
MESSAGE_HELLO = \v(COLOR_WHITE)\v(HELLO_MSG) \v(NAME_ONE) and \v(NAME_TWO)
dynamic_var_test = Hello, \v(%s) <- this is dynamic variable
```

Example of usage dynamic variables:

```Pawn
// initialize dynamic variables
#define ENABLE_LANG_DYNAMIC_VARS
#include <zlang>

public OnGameModeInit()
{
	// ...init zlang...

	Lang_printf("dynamic_var_test", "MESSAGE_HELLO");

	// result:
	// Hello, {FFFFFF}Hello {FF0000}Alex{FFFFFF} and {00FF00}Peter{FFFFFF} <- this is dynamic variable

	return 1;
}
```

# Defines

Directive | Default value | Can be redefined
----------|---------------|------------
MAX_LANGS | 2 | yes
LANG_VAR_OFFSET | 1000 | yes
LANG_IGNORED_FIRST_SYMBOL | '\0', '#', ';' | yes
MAX_LANG_VAR_STRING | 144 | yes
MAX_LANG_VALUE_STRING | 144 | yes
MAX_LANG_SEPARATOR_STRING | 64 | yes
MAX_LANG_MVALUE_STRING | MAX_LANG_VALUE_STRING * 25 | yes
MAX_LANG_FORMAT_STRING | MAX_LANG_VALUE_STRING | yes
MAX_LANG_MFORMAT_STRING | MAX_LANG_MVALUE_STRING | yes
MAX_LANG_CODE | 2 | yes
MAX_LANG_NAME | 16 | yes
MAX_LANG_FILES | 5 | yes
MAX_LANG_FILENAME | 256 | yes
ENABLE_LANG_DYNAMIC_VARS | (disabled) | yes
LANG_SVAR_VARNAME_MASK | "lng%d_%s" | yes
MAX_LANG_PREFIX_SVAR_STRING | (7 + MAX_LANG_VAR_STRING) | yes
INVALID_LANG_ID | Lang:-1 | no
INVALID_LANG_FILE_ID | -1 | no

# Functions

#### Add language
```Pawn
Lang:Lang_Add(const code[], const name[])
```

#### Remove language
```Pawn
Lang_Remove(Lang:lang)
```

#### Load language file
```Pawn
Lang_LoadFile(Lang:lang, const filename[])
```

#### Unload language file
```Pawn
Lang_UnloadFile(Lang:lang, const filename[] = "", fid = INVALID_LANG_FILE_ID)
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
Lang_SetName(Lang:lang, const name[])
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
Lang_SetCode(Lang:lang, const code[])
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
Lang:Lang_SetPlayerLangByCode(playerid, const code[])
```

#### Set player language by name
```Pawn
Lang:Lang_SetPlayerLangByName(playerid, const name[])
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
Lang_GetText(Lang:lang, const var[], text[], const size = sizeof(text), {Float, _}:...)
```

#### Get player language text
```Pawn
Lang_GetPlayerText(playerid, const var[], text[], const size = sizeof(text), {Float, _}:...)
```

#### Get default language text
```Pawn
Lang_GetDefaultText(const var[], text[], const size = sizeof(text), {Float, _}:...)
```

#### Remove language text
```Pawn
Lang_RemoveText(Lang:lang, const var[])
```

#### Is language text exists
```Pawn
Lang_IsTextExists(Lang:lang, const var[])
```

#### Language printf function with default language
```Pawn
Lang_printf(const var[], {Float, _}:...)
```

#### Language printf function with specific language
```Pawn
Lang_printfex(Lang:lang, const var[], {Float, _}:...)
```

#### Language print function with default language
```Pawn
Lang_print(const var[], {Float, _}:...)
```

#### Language print function with specific language
```Pawn
Lang_printex(Lang:lang, const var[])
```

#### Send language text to player
```Pawn
Lang_SendText(playerid, const var[], {Float, _}:...)
```

#### Send language text to all players
```Pawn
Lang_SendTextToAll(const var[], {Float, _}:...)
```

#### Send language text to all players in array
```Pawn
Lang_SendTextToPlayers(players[], const var[], {Float, _}:...)
```

#### Show language dialog to player
```Pawn
Lang_ShowDialog(playerid, dialogid, style, const var_caption[], const var_info[], const var_button1[], const var_button2[], {Float, _}:...)
```

#### Show language game text to player
```Pawn
Lang_GameText(playerid, time, style, const var[], {Float, _}:...)
```

#### Show language game text to all players
```Pawn
Lang_GameTextForAll(time, style, const var[], {Float, _}:...)
```

#### Create player TextDraw with language var
```Pawn
Lang_CreatePlayerTextDraw(playerid, Float:x, Float:y, const var[], {Float, _}:...)
```

#### Sets the language text to the player TextDraw
```Pawn
Lang_PlayerTextDrawSetString(playerid, PlayerText:text, const var[], {Float, _}:...)
```

#### Create player 3D TextLabel with language var
```Pawn
Lang_CreatePlayer3DTextLabel(playerid, const var[], color, Float:x, Float:y, Float:z, Float:DrawDistance, attachedplayer = INVALID_PLAYER_ID, attachedvehicle = INVALID_VEHICLE_ID, testLOS = 0, {Float, _}:...)
```

#### Update player 3D TextLabel with language var
```Pawn
Lang_UpdatePlayer3DTextLabel(playerid, PlayerText3D:id, color, const var[], {Float, _}:...)
```
