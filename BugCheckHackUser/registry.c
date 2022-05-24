#include "registry.h"

BOOLEAN RegistryDoesKeyExist(PCWSTR Key) {
	HKEY temp;
	return !(RegOpenKeyEx(HKEY_LOCAL_MACHINE, Key, 0, KEY_READ, &temp));
}

BOOLEAN RegistryCreateKey(PCWSTR Key) {
	DWORD dwDisposition;
	HKEY  hKey;
	DWORD Ret = RegCreateKeyEx(HKEY_LOCAL_MACHINE, Key, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, &dwDisposition);
	if (Ret != ERROR_SUCCESS) {
		return FALSE;
	}
	RegCloseKey(hKey);
	return TRUE;
}

BOOLEAN RegistryWriteDword32(PCWSTR Key, PCWSTR ValueName, DWORD32 Value) {
	HKEY hKey;
	DWORD Ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, Key, 0, KEY_WRITE, &hKey);
	if (Ret == ERROR_SUCCESS) {
		if (RegSetValueEx(hKey, ValueName, 0, REG_DWORD, (BYTE*)(&Value), sizeof(Value))) {
			RegCloseKey(hKey);
			return FALSE;
		}
		RegCloseKey(hKey);
		return TRUE;
	}
	return FALSE;
}

BOOLEAN RegistryWriteString(PCWSTR Key, PCWSTR ValueName, PCWSTR Value) {
	DWORD Ret;
	HKEY hKey;
	Ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, Key, 0, KEY_WRITE, &hKey);
	if (Ret == ERROR_SUCCESS) {
		if (RegSetValueEx(hKey, ValueName, 0, REG_SZ, (LPBYTE)(Value), ((((DWORD)lstrlen(Value) + 1)) * 2))) {
			RegCloseKey(hKey);
			return FALSE;
		}
		RegCloseKey(hKey);
		return TRUE;
	}
	return FALSE;
}