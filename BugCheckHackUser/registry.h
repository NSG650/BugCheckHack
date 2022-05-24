#pragma once

#include <Windows.h>

BOOLEAN RegistryDoesKeyExist(PCWSTR Key);
BOOLEAN RegistryCreateKey(PCWSTR Key);
BOOLEAN RegistryWriteDword32(PCWSTR Key, PCWSTR ValueName, DWORD32 Value);
BOOLEAN RegistryWriteString(PCWSTR Key, PCWSTR ValueName, PCWSTR Value);