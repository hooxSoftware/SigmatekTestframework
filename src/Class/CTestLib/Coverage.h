
Function Global __cdecl TestLib_Init
VAR_INPUT
   pClassList     : ^VOID;
   u32ClassSize   : UDINT;
   u32SizeSize    : UDINT;
END_VAR
VAR_OUTPUT

END_VAR;


Function Global __cdecl TestLib_LineCovered
VAR_INPUT
   u16ClassId : UDINT;
   u16Line    : UDINT;
END_VAR
VAR_OUTPUT

END_VAR;


Function Global __cdecl TestLib_GetMaxClassIndex
VAR_INPUT

END_VAR
VAR_OUTPUT
   u16RetValue : UDINT;
END_VAR;

Function Global __cdecl TestLib_GetMaxLineIndex
VAR_INPUT

END_VAR
VAR_OUTPUT
   u16RetValue : UDINT;
END_VAR;

Function Global __cdecl TestLib_GetSize
VAR_INPUT

END_VAR
VAR_OUTPUT
   u16RetValue : UINT;
END_VAR;


Function Global __cdecl TestLib_GetHits
VAR_INPUT
   u16ClassId : UDINT;
   u16Line    : UDINT;
END_VAR
VAR_OUTPUT
   u8RetValue : USINT;
END_VAR;