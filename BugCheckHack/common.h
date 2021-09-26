#pragma once

#include <wdm.h>

unsigned __int64 _strtoui64(
	const char* nptr,
	char** endptr,
	int base
);

VOID Print(PCCH Format, ...);