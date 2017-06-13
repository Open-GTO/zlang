static
	langText[MAX_LANG_FORMAT_STRING];

/*
 * SendClientMessageToAll
 */
stock zLang_SendClientMessageToAll(color, message[], va_args<>) {
	if(Lang_IsTextExists(Lang_GetDefaultLang(), message)) {
		static langsText[MAX_LANGS][MAX_LANG_FORMAT_STRING];
		for(new lang = Lang_GetCount(); lang != -1; lang--)
		{
			Lang_GetText(Lang:lang, message, langsText[lang]);
			if(numargs() > 3)
			{
				va_format(langsText[lang], sizeof(langsText[]), langsText[lang], va_start<3>);	
			}
		}
	#if defined foreach
		foreach (new playerid : Player) {
	#else
		for (new playerid = GetPlayerPoolSize(); playerid != -1; playerid--) {
			if (!IsPlayerConnected(playerid)) {
				continue;
			}
	#endif
			SendClientMessage(playerid, color, langsText[_:Lang_GetPlayerLang(playerid)]);
		}
		return 1;
	}
	else {
		if(numargs() > 2)
		{
			va_format(langText, sizeof(langText), message, va_start<2>);
			return SendClientMessageToAll(color, langText);
		}
		return SendClientMessageToAll(color, message);
	}
}

#if defined _ALS_SendClientMessageToAll
	#undef SendClientMessageToAll
#else
	#define _ALS_SendClientMessageToAll
#endif
#define SendClientMessageToAll zLang_SendClientMessageToAll

/*
 * SendClientMessage
 */
stock zLang_SendClientMessage(playerid, color, message[], va_args<>) {
	if(Lang_IsTextExists(Lang_GetPlayerLang(playerid), message)) {
		Lang_GetText(Lang_GetPlayerLang(playerid), message, langText);
		if(numargs() > 3)
		{
			va_format(langText, sizeof(langText), langText, va_start<3>);	
		}
		return SendClientMessage(playerid, color, langText);
	}
	else {
		if(numargs() > 3)
		{
			va_format(langText, sizeof(langText), message, va_start<3>);
			return SendClientMessage(playerid, color, langText);
		}
		return SendClientMessage(playerid, color, message);
	}
}

#if defined _ALS_SendClientMessage
	#undef SendClientMessage
#else
	#define _ALS_SendClientMessage
#endif
#define SendClientMessage zLang_SendClientMessage

/*
 * CreatePlayerTextDraw
 */
stock PlayerText:zLang_CreatePlayerTextDraw(playerid, Float:x, Float:y, text[], va_args<>) {
	if(Lang_IsTextExists(Lang_GetPlayerLang(playerid), text)) {
		Lang_GetText(Lang_GetPlayerLang(playerid), text, langText);
		if(numargs() > 4)
		{
			va_format(langText, sizeof(langText), langText, va_start<4>);	
		}
		return CreatePlayerTextDraw(playerid, x, y, langText);
	}
	else {
		if(numargs() > 4)
		{
			va_format(langText, sizeof(langText), text, va_start<4>);
			return CreatePlayerTextDraw(playerid, x, y, langText);
		}
		return CreatePlayerTextDraw(playerid, x, y, text);
	}
}

#if defined _ALS_CreatePlayerTextDraw
	#undef CreatePlayerTextDraw
#else
	#define _ALS_CreatePlayerTextDraw
#endif
#define CreatePlayerTextDraw zLang_CreatePlayerTextDraw

/*
 * PlayerTextDrawSetString
 */
stock zLang_PlayerTextDrawSetString(playerid, PlayerText:text, string[], va_args<>) {
	if(Lang_IsTextExists(Lang_GetPlayerLang(playerid), string)) {
		Lang_GetText(Lang_GetPlayerLang(playerid), string, langText);
		if(numargs() > 3)
		{
			va_format(langText, sizeof(langText), langText, va_start<3>);	
		}
		return PlayerTextDrawSetString(playerid, text, langText);
	}
	else {
		if(numargs() > 3)
		{
			va_format(langText, sizeof(langText), string, va_start<3>);
			return PlayerTextDrawSetString(playerid, text, langText);
		}
		return PlayerTextDrawSetString(playerid, text, string);
	}
}

#if defined _ALS_PlayerTextDrawSetString
	#undef PlayerTextDrawSetString
#else
	#define _ALS_PlayerTextDrawSetString
#endif
#define PlayerTextDrawSetString zLang_PlayerTextDrawSetString

/*
 * GameTextForAll
 */
stock zLang_GameTextForAll(string[], time, style, va_args<>) {
	if(Lang_IsTextExists(Lang_GetDefaultLang(), string)) {
		static langsText[MAX_LANGS][MAX_LANG_FORMAT_STRING];
		for(new lang = Lang_GetCount(); lang != -1; lang--)
		{
			Lang_GetText(Lang:lang, string, langsText[lang]);
			if(numargs() > 3)
			{
				va_format(langsText[lang], sizeof(langsText[]), langsText[lang], va_start<3>);	
			}
		}
	#if defined foreach
		foreach (new playerid : Player) {
	#else
		for (new playerid = GetPlayerPoolSize(); playerid != -1; playerid--) {
			if (!IsPlayerConnected(playerid)) {
				continue;
			}
	#endif
			GameTextForPlayer(playerid, langsText[_:Lang_GetPlayerLang(playerid)], time, style);
		}
		return 1;
	}
	else {
		if(numargs() > 3)
		{
			va_format(langText, sizeof(langText), string, va_start<3>);
			return GameTextForAll(langText, time, style);
		}
		return GameTextForAll(string, time, style);
	}
}

#if defined _ALS_GameTextForAll
	#undef GameTextForAll
#else
	#define _ALS_GameTextForAll
#endif
#define GameTextForAll zLang_GameTextForAll

/*
 * GameTextForPlayer
 */
stock zLang_GameTextForPlayer(playerid, string[], time, style, va_args<>) {
	if(Lang_IsTextExists(Lang_GetPlayerLang(playerid), string)) {
		Lang_GetText(Lang_GetPlayerLang(playerid), string, langText);
		if(numargs() > 4)
		{
			va_format(langText, sizeof(langText), langText, va_start<4>);	
		}
		return GameTextForPlayer(playerid, langText, time, style);
	}
	else {
		if(numargs() > 4)
		{
			va_format(langText, sizeof(langText), string, va_start<4>);
			return GameTextForPlayer(playerid, langText, time, style);
		}
		return GameTextForPlayer(playerid, string, time, style);
	}
}

#if defined _ALS_GameTextForPlayer
	#undef GameTextForPlayer
#else
	#define _ALS_GameTextForPlayer
#endif
#define GameTextForPlayer zLang_GameTextForPlayer

/*
 * CreatePlayer3DTextLabel
 */
stock PlayerText3D:zLang_CreatePlayer3DTextLabel(playerid, text[], color, Float:x, Float:y, Float:z, Float:DrawDistance, attachedplayer = INVALID_PLAYER_ID, attachedvehicle = INVALID_VEHICLE_ID, testLOS = 0, va_args<>) {
	if(Lang_IsTextExists(Lang_GetPlayerLang(playerid), text)) {
		Lang_GetText(Lang_GetPlayerLang(playerid), text, langText);
		if(numargs() > 10)
		{
			va_format(langText, sizeof(langText), langText, va_start<10>);	
		}
		return CreatePlayer3DTextLabel(playerid, langText, color, x, y, z, DrawDistance, attachedplayer, attachedvehicle, testLOS);
	}
	else {
		if(numargs() > 10)
		{
			va_format(langText, sizeof(langText), text, va_start<10>);
			return CreatePlayer3DTextLabel(playerid, langText, color, x, y, z, DrawDistance, attachedplayer, attachedvehicle, testLOS);
		}
		return CreatePlayer3DTextLabel(playerid, text, color, x, y, z, DrawDistance, attachedplayer, attachedvehicle, testLOS);
	}
}

#if defined _ALS_CreatePlayer3DTextLabel
	#undef CreatePlayer3DTextLabel
#else
	#define _ALS_CreatePlayer3DTextLabel
#endif
#define CreatePlayer3DTextLabel zLang_CreatePlayer3DTextLabel

/*
 * UpdatePlayer3DTextLabelText
 */
stock zLang_UpdPlayer3DTextLabelText(playerid, PlayerText3D:id, color, text[], va_args<>) {
	if(Lang_IsTextExists(Lang_GetPlayerLang(playerid), text)) {
		Lang_GetText(Lang_GetPlayerLang(playerid), text, langText);
		if(numargs() > 4)
		{
			va_format(langText, sizeof(langText), langText, va_start<4>);	
		}
		return UpdatePlayer3DTextLabelText(playerid, id, color, langText);
	}
	else {
		if(numargs() > 4)
		{
			va_format(langText, sizeof(langText), text, va_start<4>);
			return UpdatePlayer3DTextLabelText(playerid, id, color, langText);
		}
		return UpdatePlayer3DTextLabelText(playerid, id, color, text);
	}
}

#if defined _ALS_UpdatePlayer3DTextLabelTex
	#undef UpdatePlayer3DTextLabelText
#else
	#define _ALS_UpdatePlayer3DTextLabelTex
#endif
#define UpdatePlayer3DTextLabelText zLang_UpdPlayer3DTextLabelText

/*
 * SetPlayerObjectMaterialText
 */
stock zLang_SetPlayerObjMaterialText(playerid, objectid, text[], materialindex = 0, materialsize = OBJECT_MATERIAL_SIZE_256x128, fontface[] = "Arial", fontsize = 24, bold = 1, fontcolor = 0xFFFFFFFF, backcolor = 0, textalignment = 0, va_args<>) {
	if(Lang_IsTextExists(Lang_GetPlayerLang(playerid), text)) {
		Lang_GetText(Lang_GetPlayerLang(playerid), text, langText);
		if(numargs() > 11)
		{
			va_format(langText, sizeof(langText), langText, va_start<11>);	
		}
		return SetPlayerObjectMaterialText(playerid, objectid, langText, materialindex, materialsize, fontface, fontsize, bold, fontcolor, backcolor, textalignment);
	}
	else {
		if(numargs() > 11)
		{
			va_format(langText, sizeof(langText), text, va_start<11>);
			return SetPlayerObjectMaterialText(playerid, objectid, langText, materialindex, materialsize, fontface, fontsize, bold, fontcolor, backcolor, textalignment);
		}
		return SetPlayerObjectMaterialText(playerid, objectid, text, materialindex, materialsize, fontface, fontsize, bold, fontcolor, backcolor, textalignment);
	}
}

#if defined _ALS_SetPlayerObjectMaterialTex
	#undef SetPlayerObjectMaterialText
#else
	#define _ALS_SetPlayerObjectMaterialTex
#endif
#define SetPlayerObjectMaterialText zLang_SetPlayerObjMaterialText


/*
 * ShowPlayerDialog
 */
stock zLang_ShowPlayerDialog(playerid, dialogid, style, caption[], info[], button1[], button2[], va_args<>) {
	static _button1[64], _button2[64], _caption[64];
	new Lang:pLang = Lang_GetPlayerLang(playerid), ret[4];
	if((ret[0] = Lang_IsTextExists(pLang, caption)) || (ret[1] = Lang_IsTextExists(pLang, info)) || (ret[2] = Lang_IsTextExists(pLang, button1)) || (ret[3] = Lang_IsTextExists(pLang, button2))) {

		if(ret[0]) Lang_GetText(pLang, caption, _caption);
		if(ret[1]) Lang_GetText(pLang, info, langText);
		if(ret[2]) Lang_GetText(pLang, button1, _button1);
		if(ret[3]) Lang_GetText(pLang, button2, _button2);

		if(numargs() > 4)
		{
			va_format(langText, sizeof(langText), langText, va_start<4>);	
		}
		else
		{
			strcat((langText[0] = EOS, langText), info);
		}
		return ShowPlayerDialog(playerid, dialogid, style, 
			ret[0] ? (_caption) : (caption),
			langText,
			ret[2] ? (_button1) : (button1),
			ret[3] ? (_button2) : (button2)
		);
	}
	else {
		if(numargs() > 7)
		{
			va_format(langText, sizeof(langText), info, va_start<7>);
			return ShowPlayerDialog(playerid, dialogid, style, caption, langText, button1, button2);
		}
		return ShowPlayerDialog(playerid, dialogid, style, caption, info, button1, button2);
	}
}

#if defined _ALS_ShowPlayerDialog
	#undef ShowPlayerDialog
#else
	#define _ALS_ShowPlayerDialog
#endif
#define ShowPlayerDialog zLang_ShowPlayerDialog
