#include "service.h"

BOOLEAN ServiceCreate(PCWSTR Name, PCWSTR Path) {
	SC_HANDLE pSCM = OpenSCManagerW(NULL, NULL,
		SC_MANAGER_CREATE_SERVICE);
	if (!pSCM)
		return FALSE;
	SC_HANDLE pService = CreateService(
		pSCM,
		Name,
		Name,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_AUTO_START,
		SERVICE_ERROR_IGNORE,
		Path,
		NULL, NULL, NULL, NULL, NULL
	);
	if (!pService) {
		CloseServiceHandle(pSCM);
		return FALSE;
	}
	CloseServiceHandle(pService);
	CloseServiceHandle(pSCM);
	return TRUE;
}

BOOLEAN ServiceDelete(PCWSTR Name) {
	SC_HANDLE pSCM = OpenSCManagerW(NULL, NULL,
		SC_MANAGER_CREATE_SERVICE);
	if (!pSCM)
		return FALSE;
	SC_HANDLE pService = OpenService(pSCM, Name, SERVICE_ALL_ACCESS);
	if (!pService) {
		CloseServiceHandle(pSCM);
		return FALSE;
	}
	BOOLEAN ret = DeleteService(pService);
	CloseServiceHandle(pService);
	CloseServiceHandle(pSCM);
	return ret;
}

BOOLEAN ServiceStart(PCWSTR Name) {
	SC_HANDLE pSCM = OpenSCManagerW(NULL, NULL,
		SC_MANAGER_CREATE_SERVICE);
	if (!pSCM)
		return FALSE;

	SC_HANDLE pService = OpenService(pSCM, Name, SERVICE_ALL_ACCESS);
	if (!pService) {
		CloseServiceHandle(pSCM);
		return FALSE;
	}
	BOOLEAN ret = (StartService(pService, 0, NULL) || GetLastError() == 0x287);
	CloseServiceHandle(pService);
	CloseServiceHandle(pSCM);
	return ret;
}