# zlang
SVar lang system

# Defines
```Pawn
#if !defined MAX_MULTI_VAR_COUNT
	#define MAX_MULTI_VAR_COUNT    10
#endif

#define MAX_LANG_VAR_STRING    39
#define MAX_LANG_VALUE_STRING  144
#define MAX_LANG_MULTI_STRING  (MAX_LANG_VALUE_STRING * MAX_MULTI_VAR_COUNT)

#define _(%0) Lang_ReturnText(#%0)
#define _m(%0) Lang_ReturnMultiText(#%0)

#define __(%0,%1) Lang_GetText(#%0,%1)
#define __m(%0,%1) Lang_GetMultiText(#%0,%1)
```

# Functions
```Pawn
Lang_LoadText(filename[]);
Lang_SetText(varname[], value[]);
Lang_GetText(varname[], string[], size = sizeof(string));
Lang_ReturnText(varname[]);
Lang_GetMultiText(varname[], string[], size = sizeof(string));
Lang_ReturnMultiText(varname[]);
Lang_IsTextExists(varname[]);
Lang_DeleteText(varname[]);
```
