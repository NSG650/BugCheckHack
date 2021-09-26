#pragma once

#include <ntdef.h>

// Taken from AngryWindows (https://github.com/ch3rn0byl/AngryWindows/)


///================================================
/// Types
///================================================
typedef struct _BSOD_INFORMATION
{
	UINT64 HalpPCIConfigReadHandler;
	UINT64 EtwpLastBranchEntry;
	UINT64 AddressOfColorVar;
	UINT8 offset;
	UINT8 colorOffset;
	UNICODE_STRING Emoticon;
	UNICODE_STRING String1;
	UNICODE_STRING String2;
	PUNICODE_STRING BsodMessageOne;
	PUNICODE_STRING BsodMessageTwo;
} BSOD_INFORMATION, * PBSOD_INFORMATION;

extern PBSOD_INFORMATION BsodInformation;