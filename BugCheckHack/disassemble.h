#pragma once

#include <wdm.h>

UINT64 Disassemble_KeBugCheck2(UINT64* Result);
UINT64 Disassemble_KiDisplayBlueScreen(PVOID KeBugCheck2Address, UINT64* Result);
UINT64 Disassemble_BgpFwDisplayBugCheckScreen(PVOID KiDisplayBlueScreenAddress, UINT64* Result); 
UINT64 Disassemble_HalpPCIConfigReadHandlers(PVOID BgpFwDisplayBugCheckScreenAddress, UINT64* Result1, UINT64* Result2, UINT64* Result3);