/*

*/

#define MAX_TEXT_ENTRIES  1024

#define MAX_VAR_STRING    32
#define MAX_VALUE_STRING  128

#define _(%0) Lang_GetText(#%0)

enum e_TEXT_VARS {
	e_TEXT_VAR_HASH,
	e_TEXT_VAR_NEXT,
	e_TEXT_VAR_PREV,
	e_TEXT_VALUE[MAX_VALUE_STRING],
}

static
	slot_last = 0,
	text_array[MAX_TEXT_ENTRIES][e_TEXT_VARS];

stock Lang_LoadText(filename[])
{
	for (new i = 0; i < MAX_TEXT_ENTRIES; i++) {
		text_array[i][e_TEXT_VAR_HASH] = 0;
		text_array[i][e_TEXT_VAR_PREV] = -1;
		text_array[i][e_TEXT_VAR_NEXT] = -1;
	}

	new File:flang = fopen(filename, io_read);
	if (!flang) {
		printf("Error: no such language file '%s'", filename);
		return 0;
	}
	
	new len, sep_pos, string[MAX_VALUE_STRING], varname[MAX_VAR_STRING];
	while (fread(flang, string, sizeof(string))) {
		StripNL(string);

		len = strlen(string);
		if (len == 0 || (string[0] == '/' && string[1] == '/')) {
			continue;
		}
		
		sep_pos = strfind(string, " = ", true);
		if (sep_pos != -1) {
			strmid(varname, string, 0, sep_pos);
			strmid(string, string, sep_pos + 3, len);
			Lang_FixSpecialChar(string);
			Lang_SetText(varname, string, len);
		}
	}
	fclose(flang);
	return 1;
}

stock Lang_FixSpecialChar(string[MAX_VALUE_STRING])
{
	for (new i = 0; string[i] != '\0'; i++) {
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
		}
	}
}

stock Lang_SetText(varname[], value[], length = -1)
{
	if (length == -1) {
		length = strlen(value);
	}

	new slot = Lang_GetFreeSlot();
	if (slot == -1) {
		print("Error: lang text buffer is full.");
		return 0;
	}

	new hash = bernstein(varname);

	if (slot != 0) {
		for (new idx = 0, cmp, next; ; idx = next) {
			cmp = text_array[idx][e_TEXT_VAR_HASH] - hash;

			if (cmp < 0) {
				next = text_array[idx][e_TEXT_VAR_NEXT];

				if (next == -1) {
					text_array[idx][e_TEXT_VAR_NEXT] = slot;
					break;
				}
			} else if (cmp > 0) {
				next = text_array[idx][e_TEXT_VAR_PREV];

				if (next == -1) {
					text_array[idx][e_TEXT_VAR_PREV] = slot;
					break;
				}
			}
		}
	}

	text_array[slot][e_TEXT_VAR_HASH] = hash;
	strmid(text_array[slot][e_TEXT_VALUE], value, 0, length, MAX_VALUE_STRING);
	return 1;
}

stock Lang_GetText(varname[])
{
	new
		result[MAX_VALUE_STRING],
		hash = bernstein(varname),
		cmp,
		slot = 0;

	while (slot != -1) {
		cmp = text_array[slot][e_TEXT_VAR_HASH] - hash;

		if (cmp < 0) {
			slot = text_array[slot][e_TEXT_VAR_NEXT];
		} else if (cmp > 0) {
			slot = text_array[slot][e_TEXT_VAR_PREV];
		} else {
			break;
		}
	}

	if (slot != -1) {
		strcat(result, text_array[slot][e_TEXT_VALUE]);
		return result;
	}

	/*for (new i = 0; i < MAX_TEXT_ENTRIES; i++) {
		if (text_array[i][e_TEXT_VAR_HASH] == hash) {
			strcat(result, text_array[i][e_TEXT_VALUE]);
			return result;
		}
	}*/

	strcat(result, "Error: lang value ");
	strcat(result, varname);
	strcat(result, " not found.");
	return result;
}

stock Lang_GetFreeSlot()
{
	if (slot_last >= MAX_TEXT_ENTRIES) {
		return -1;
	}
	text_array[slot_last][e_TEXT_VAR_HASH] = 0;
	text_array[slot_last][e_TEXT_VAR_PREV] = -1;
	text_array[slot_last][e_TEXT_VAR_NEXT] = -1;
	return slot_last++;
}

stock bernstein(string[])
{
	new
		hash = -1,
		i,
		j;
	while ((j = string[i++]))
	{
		hash = hash * 33 + j;
	}
	return hash;
}

stock StripNL(str[])
{
	new i = strlen(str);
	while (i-- && str[i] <= ' ') str[i] = '\0';
}