#include <ntddk.h>
#include <Zydis/Zydis.h>
#include "disassemble.h"
#include "common.h"
#include "overwrite.h"
#include "types.h"

DRIVER_INITIALIZE DriverEntry;
PBSOD_INFORMATION BsodInformation = { 0 };

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);
    UNREFERENCED_PARAMETER(DriverObject);
    Print("Hello World!\n");
    UINT64 KeBugCheck2_Address;
    if (!Disassemble_KeBugCheck2(&KeBugCheck2_Address)) {
        Print("Failed to disassemble KeBugCheck2\n");
        return STATUS_DRIVER_INTERNAL_ERROR;
    }
    UINT64 KiDisplayBlueScreen_Address;
    if (!Disassemble_KiDisplayBlueScreen((PVOID)KeBugCheck2_Address, &KiDisplayBlueScreen_Address)) {
        Print("Failed to disassemble KiDisplayBlueScreen");
        return STATUS_DRIVER_INTERNAL_ERROR;
    }
    UINT64 BgpFwDisplayBugCheckScreen_Address;
    if (!Disassemble_BgpFwDisplayBugCheckScreen((PVOID)KiDisplayBlueScreen_Address, &BgpFwDisplayBugCheckScreen_Address)) {
        return STATUS_DRIVER_INTERNAL_ERROR;
    }
    UINT64 HalPCIConfigReadHandlers_0x18;
    UINT64 EtwpLastBranchEntry_Address;
    UINT64 EtwpLastBranchEntry2_Address;
    Disassemble_HalpPCIConfigReadHandlers((PVOID)BgpFwDisplayBugCheckScreen_Address, &HalPCIConfigReadHandlers_0x18, &EtwpLastBranchEntry_Address, &EtwpLastBranchEntry2_Address);
    Print("KeBugCheck2 located at %llx\n", KeBugCheck2_Address);
    Print("KiDisplayBlueScreen located at %llx\n", KiDisplayBlueScreen_Address);
    Print("BgpFwDisplayBugCheckScreen located at %llx\n", BgpFwDisplayBugCheckScreen_Address);
    Print("EtwpLastBranchEntry located at %llx\n", EtwpLastBranchEntry_Address);
    Print("EtwpLastBranchEntry2 located at %llx\n", EtwpLastBranchEntry2_Address);
    Print("HalpPCIConfigReadHandlers+0x18 located at %llx\n", HalPCIConfigReadHandlers_0x18);
   
    UNICODE_STRING Emoticon;
    UNICODE_STRING StringOne;
    UNICODE_STRING StringTwo;
    RTL_QUERY_REGISTRY_TABLE query[2];
    NTSTATUS regStatus = 0;

    Emoticon.Buffer = NULL;
    StringOne.Buffer = NULL;
    StringTwo.Buffer = NULL;

    Emoticon.Length = 0;
    StringOne.Length = 0;
    StringTwo.Length = 0;

    Emoticon.MaximumLength = 10;
    StringOne.MaximumLength = 100;
    StringTwo.MaximumLength = 100;

    RtlZeroMemory(query, sizeof(RTL_QUERY_REGISTRY_TABLE) * 2);
    query[0].Name = L"Emoticon";
    query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    query[0].EntryContext = &Emoticon;
    regStatus = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, L"\\Registry\\Machine\\Software\\BugCheckHack", query, NULL, NULL);
    if (regStatus != STATUS_SUCCESS) {
        RtlInitUnicodeString(&Emoticon, L":)");
    }
    
    if (OverwriteFrowny(HalPCIConfigReadHandlers_0x18, &Emoticon) != STATUS_SUCCESS)
        return STATUS_DRIVER_INTERNAL_ERROR;

    RtlZeroMemory(query, sizeof(RTL_QUERY_REGISTRY_TABLE) * 2);
    query[0].Name = L"StringOne";
    query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    query[0].EntryContext = &StringOne;
    regStatus = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, L"\\Registry\\Machine\\Software\\BugCheckHack", query, NULL, NULL);
    
    if (regStatus != STATUS_SUCCESS) {
        RtlInitUnicodeString(&StringOne, L"Windows tried to break your hard drive and failed.");
    }

    if (OverwriteString((PUNICODE_STRING)EtwpLastBranchEntry_Address, &StringOne) != STATUS_SUCCESS)
        return STATUS_DRIVER_INTERNAL_ERROR;

    RtlZeroMemory(query, sizeof(RTL_QUERY_REGISTRY_TABLE) * 2);
    query[0].Name = L"StringTwo";
    query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    query[0].EntryContext = &StringTwo;
    regStatus = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, L"\\Registry\\Machine\\Software\\BugCheckHack", query, NULL, NULL);

    if (regStatus != STATUS_SUCCESS) {
        RtlInitUnicodeString(&StringTwo, L"We are restarting and thinking how stupid you are lmfao.");
    }

    if (OverwriteString((PUNICODE_STRING)EtwpLastBranchEntry2_Address, &StringTwo) != STATUS_SUCCESS)
        return STATUS_DRIVER_INTERNAL_ERROR;

    return STATUS_UNSUCCESSFUL;
}