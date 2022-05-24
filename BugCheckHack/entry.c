#include <ntddk.h>

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY InLoadOrderLinks;
	LIST_ENTRY InMemoryOrderLinks;
	LIST_ENTRY InInitializationOrderLinks;
	PVOID DllBase;
	PVOID EntryPoint;
	ULONG SizeOfImage;
	UNICODE_STRING FullDllName;
	UNICODE_STRING BaseDllName;
	ULONG Flags;
	USHORT LoadCount;
	USHORT TlsIndex;
	LIST_ENTRY HashLinks;
	ULONG TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, * PLDR_DATA_TABLE_ENTRY;

PVOID KernelBase = NULL;

PCWSTR Entries[6] = {
	L"Emoticon",
	L"StringOne",
	L"StringTwo",
	L"WebsiteUrl",
	L"CalledSupport",
	L"PossibleFixes"
};

NTSTATUS RegistryGetValue(PCWSTR EntryName, PVOID Address) {
	RTL_QUERY_REGISTRY_TABLE Query[2] = { 0 };
	Query[0].Name = EntryName;
	Query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
	Query[0].EntryContext = Address;
	return RtlQueryRegistryValues(
		RTL_REGISTRY_ABSOLUTE,
		L"\\Registry\\Machine\\Software\\BugCheckHack",
		Query,
		NULL,
		NULL
	);
}

NTSTATUS Overwrite(PVOID Address, PVOID Data, ULONG Size) {
	PHYSICAL_ADDRESS PhysAddress = MmGetPhysicalAddress(Address);
	PVOID MappedAddress = MmMapIoSpace(PhysAddress, Size, MmNonCached);

	if (MappedAddress == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;

	RtlCopyMemory(MappedAddress, Data, Size);
	MmUnmapIoSpace(MappedAddress, Size);
	return STATUS_SUCCESS;
}


// https://www.unknowncheats.me/forum/general-programming-and-reversing/427419-getkernelbase.html

__forceinline wchar_t locase_w(wchar_t c) {
	if ((c >= 'A') && (c <= 'Z'))
		return c + 0x20;
	else
		return c;
}

int _strcmpi_w(const wchar_t* s1, const wchar_t* s2) {
	wchar_t c1, c2;

	if (s1 == s2)
		return 0;

	if (s1 == 0)
		return -1;

	if (s2 == 0)
		return 1;

	do {
		c1 = locase_w(*s1);
		c2 = locase_w(*s2);
		s1++;
		s2++;
	} while ((c1 != 0) && (c1 == c2));

	return (int)(c1 - c2);
}

PVOID GetKernelBase(PDRIVER_OBJECT DriverObject) {
	if (KernelBase)
		return KernelBase;
	PLDR_DATA_TABLE_ENTRY entry = (PLDR_DATA_TABLE_ENTRY)DriverObject->DriverSection;
	PLDR_DATA_TABLE_ENTRY first = entry;
	while ((PLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink != first) {
		if (_strcmpi_w(entry->BaseDllName.Buffer, L"ntoskrnl.exe") == 0) {
			return entry->DllBase;
		}
		entry = (PLDR_DATA_TABLE_ENTRY)entry->InLoadOrderLinks.Flink;
	}
	KernelBase = NULL;
	return NULL;
}

BOOLEAN IsInsider(VOID) {
	DWORD32 Exist;
	RTL_QUERY_REGISTRY_TABLE Query[2] = { 0 };
	Query[0].Name = L"IsBuildFlightingEnabled";
	Query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
	Query[0].EntryContext = &Exist;
	if (RtlQueryRegistryValues(
		RTL_REGISTRY_ABSOLUTE,
		L"\\Registry\\Machine\\Software\\Microsoft\\WindowsSelfHost\\Applicability",
		Query,
		NULL,
		NULL
	)) {
		return FALSE;
	}
	return Exist;
}

VOID ParseEtwpLastBranchLookAsideListStrings(ULONG_PTR EtwpLastBranchLookAsideList, ULONG_PTR* Addresses) {
	Addresses[0] = EtwpLastBranchLookAsideList + 0x60;
	PUNICODE_STRING temp = (PUNICODE_STRING)(EtwpLastBranchLookAsideList + 0x60);
	for (UCHAR i = 0; i < sizeof(UNICODE_STRING); i++, temp++) {
		if (wcsstr(temp->Buffer, L"Insider Build ran into a problem and needs to restart.") && IsInsider())
			Addresses[0] = (UINT64)temp;
		if (wcsstr(temp->Buffer, L"and then we'll restart for you"))
			Addresses[1] = (UINT64)temp;
		if (wcsstr(temp->Buffer, L"www.windows.com/stopcode"))
			Addresses[2] = (UINT64)temp;
		if (wcsstr(temp->Buffer, L"this issue and possible fixes, visit"))
			Addresses[3] = (UINT64)temp;
		if (wcsstr(temp->Buffer, L"give them this info:"))
			Addresses[4] = (UINT64)temp;
	}
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	DWORD32 FrownyOffset = 0;
	DWORD32 StringsOffset = 0;
	DWORD32 BgpClearScreen = 0;
	KernelBase = GetKernelBase(DriverObject);
	if (RegistryGetValue(L"FrownyOffset", &FrownyOffset)) {
		return STATUS_DRIVER_INTERNAL_ERROR;
	}
	if (RegistryGetValue(L"StringsOffset", &StringsOffset)) {
		return STATUS_DRIVER_INTERNAL_ERROR;
	}
	if (RegistryGetValue(L"BgpClearScreenOffset", &BgpClearScreen)) {
		return STATUS_DRIVER_INTERNAL_ERROR;
	}
	
	DbgPrint("%llx\n%llx\n%llx\n%llx\n", FrownyOffset, StringsOffset, BgpClearScreen, KernelBase);

	if (!FrownyOffset || !StringsOffset)
		return STATUS_DRIVER_INTERNAL_ERROR;

	ULONG_PTR Strings[5] = { 0 };
	ParseEtwpLastBranchLookAsideListStrings((ULONG_PTR)KernelBase + StringsOffset, Strings);
	
	// faster than memset lmfao
	CHAR Patch[8] = { 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3 };

	if (BgpClearScreen != 0) {
		Overwrite((PVOID)((ULONG_PTR)KernelBase + BgpClearScreen), Patch, 1);
	}

	UNICODE_STRING String[6] = { 0 };

	String[0].MaximumLength = 10;
	for (UCHAR i = 1; i < 6; i++)
		String[i].MaximumLength = 100;

	for (UCHAR i = 0; i < 6; i++) {
		if (RegistryGetValue(Entries[i], &String[i])) {
			return STATUS_DRIVER_INTERNAL_ERROR;
		}
		if (i == 0) {
			if (Overwrite(((ULONG_PTR)KernelBase + FrownyOffset), &String[i], sizeof(UNICODE_STRING)))
				return STATUS_DRIVER_INTERNAL_ERROR;
		}
		else {
			if (Overwrite(Strings[i - 1], &String[i], sizeof(UNICODE_STRING)))
				return STATUS_DRIVER_INTERNAL_ERROR;
		}
	}

	return STATUS_FAILED_DRIVER_ENTRY;
}