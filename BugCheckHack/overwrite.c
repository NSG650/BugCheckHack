#include "overwrite.h"

NTSTATUS OverwriteFrowny(UINT64 Address, PUNICODE_STRING String) {
	PHYSICAL_ADDRESS pa = MmGetPhysicalAddress((PVOID)Address);
	PUNICODE_STRING MapppedAddress = (PUNICODE_STRING)MmMapIoSpace(pa, sizeof(UNICODE_STRING), MmNonCached);
	if (MapppedAddress == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;
	MapppedAddress->Length = 10;
	MapppedAddress->MaximumLength = 10;
	PHYSICAL_ADDRESS paBuffer = MmGetPhysicalAddress(MapppedAddress->Buffer);
	PVOID MappedBuffer = MmMapIoSpace(paBuffer, MapppedAddress->MaximumLength, MmNonCached);
	if (MappedBuffer == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;
	RtlCopyMemory(MappedBuffer, String->Buffer, String->Length);
	MmUnmapIoSpace(MappedBuffer, MapppedAddress->MaximumLength);
	MmUnmapIoSpace(MapppedAddress, sizeof(UNICODE_STRING));
	return STATUS_SUCCESS;
}

NTSTATUS OverwriteString(PUNICODE_STRING Source, PUNICODE_STRING Replace) {
	PHYSICAL_ADDRESS pa = MmGetPhysicalAddress((PVOID)Source);
	PUNICODE_STRING MapppedAddress = (PUNICODE_STRING)MmMapIoSpace(pa, sizeof(UNICODE_STRING), MmNonCached);
	if (MapppedAddress == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;
	MapppedAddress->Length = Replace->Length;
	MapppedAddress->MaximumLength = Replace->MaximumLength;
	PHYSICAL_ADDRESS paBuffer = MmGetPhysicalAddress(MapppedAddress->Buffer);
	PVOID MappedBuffer = MmMapIoSpace(paBuffer, MapppedAddress->MaximumLength, MmNonCached);
	if (MappedBuffer == NULL)
		return STATUS_INSUFFICIENT_RESOURCES;
	RtlCopyMemory(MappedBuffer, Replace->Buffer, Replace->Length);
	MmUnmapIoSpace(MappedBuffer, MapppedAddress->MaximumLength);
	MmUnmapIoSpace(MapppedAddress, sizeof(UNICODE_STRING));
	return STATUS_SUCCESS;
}