#include <ntddk.h>
#include <Zydis/Zydis.h>
#include "disassemble.h"
#include "common.h"
#include "overwrite.h"

DRIVER_INITIALIZE DriverEntry;

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
    UINT64 EtwpLastBranchEntry3_Address;
    UINT64 EtwpLastBranchEntry4_Address;
    UINT64 EtwpLastBranchEntry5_Address;
    if (!Disassemble_HalpPCIConfigReadHandlers((PVOID)BgpFwDisplayBugCheckScreen_Address, &HalPCIConfigReadHandlers_0x18, &EtwpLastBranchEntry_Address, 
        &EtwpLastBranchEntry2_Address, &EtwpLastBranchEntry3_Address, &EtwpLastBranchEntry4_Address, &EtwpLastBranchEntry5_Address)) {
        return STATUS_DRIVER_INTERNAL_ERROR;
    }
    Print("KeBugCheck2 located at %llx\n", KeBugCheck2_Address);
    Print("KiDisplayBlueScreen located at %llx\n", KiDisplayBlueScreen_Address);
    Print("BgpFwDisplayBugCheckScreen located at %llx\n", BgpFwDisplayBugCheckScreen_Address);
    Print("StringOne located at %llx\n", EtwpLastBranchEntry_Address);
    Print("StringTwo located at %llx\n", EtwpLastBranchEntry2_Address);
    Print("WebsiteUrl located at %llx\n", EtwpLastBranchEntry3_Address);
    Print("PossibleFixes located at %llx\n", EtwpLastBranchEntry4_Address);
    Print("CalledSupport located at %llx\n", EtwpLastBranchEntry5_Address);
    Print("Frowny located at %llx\n", HalPCIConfigReadHandlers_0x18);
   
    UNICODE_STRING Emoticon;
    UNICODE_STRING StringOne;
    UNICODE_STRING StringTwo;
    UNICODE_STRING WebsiteUrl;
    UNICODE_STRING CalledSupport;
    UNICODE_STRING PossibleFixes;

    RTL_QUERY_REGISTRY_TABLE query[2];
    NTSTATUS regStatus = 0;

    Emoticon.Buffer = NULL;
    StringOne.Buffer = NULL;
    StringTwo.Buffer = NULL;
    WebsiteUrl.Buffer = NULL;
    CalledSupport.Buffer = NULL;
    PossibleFixes.Buffer = NULL;

    Emoticon.Length = 0;
    StringOne.Length = 0;
    StringTwo.Length = 0;
    WebsiteUrl.Length = 0;
    CalledSupport.Length = 0;
    PossibleFixes.Length = 0;

    Emoticon.MaximumLength = 10;
    StringOne.MaximumLength = 100;
    StringTwo.MaximumLength = 100;
    WebsiteUrl.MaximumLength = 100;
    CalledSupport.MaximumLength = 100;
    PossibleFixes.MaximumLength = 100;

    RtlZeroMemory(query, sizeof(RTL_QUERY_REGISTRY_TABLE) * 2);
    query[0].Name = L"Emoticon";
    query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    query[0].EntryContext = &Emoticon;
    regStatus = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, L"\\Registry\\Machine\\Software\\BugCheckHack", query, NULL, NULL);
    
    if (regStatus != STATUS_SUCCESS)
        RtlInitUnicodeString(&Emoticon, L":) ");
    
    if (OverwriteFrowny(HalPCIConfigReadHandlers_0x18, &Emoticon) != STATUS_SUCCESS)
        return STATUS_DRIVER_INTERNAL_ERROR;

    RtlZeroMemory(query, sizeof(RTL_QUERY_REGISTRY_TABLE) * 2);
    query[0].Name = L"StringOne";
    query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    query[0].EntryContext = &StringOne;
    regStatus = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, L"\\Registry\\Machine\\Software\\BugCheckHack", query, NULL, NULL);
    
    if (regStatus != STATUS_SUCCESS)
        RtlInitUnicodeString(&StringOne, L"Windows tried to break your hard drive and failed.");

    if (OverwriteString((PUNICODE_STRING)EtwpLastBranchEntry_Address, &StringOne) != STATUS_SUCCESS)
        return STATUS_DRIVER_INTERNAL_ERROR;

    RtlZeroMemory(query, sizeof(RTL_QUERY_REGISTRY_TABLE) * 2);
    query[0].Name = L"StringTwo";
    query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    query[0].EntryContext = &StringTwo;
    regStatus = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, L"\\Registry\\Machine\\Software\\BugCheckHack", query, NULL, NULL);

    if (regStatus != STATUS_SUCCESS)
        RtlInitUnicodeString(&StringTwo, L"We are restarting and thinking how stupid you are lmfao.");

    if (OverwriteString((PUNICODE_STRING)EtwpLastBranchEntry2_Address, &StringTwo) != STATUS_SUCCESS)
        return STATUS_DRIVER_INTERNAL_ERROR;


    RtlZeroMemory(query, sizeof(RTL_QUERY_REGISTRY_TABLE) * 2);
    query[0].Name = L"WebsiteUrl";
    query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    query[0].EntryContext = &WebsiteUrl;
    regStatus = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, L"\\Registry\\Machine\\Software\\BugCheckHack", query, NULL, NULL);
    
    if (regStatus != STATUS_SUCCESS)
        RtlInitUnicodeString(&WebsiteUrl, L"https://cryaboutit.com/");

    if (OverwriteString((PUNICODE_STRING)EtwpLastBranchEntry3_Address, &WebsiteUrl))
        return STATUS_DRIVER_INTERNAL_ERROR;

    RtlZeroMemory(query, sizeof(RTL_QUERY_REGISTRY_TABLE) * 2);
    query[0].Name = L"CalledSupport";
    query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    query[0].EntryContext = &CalledSupport;
    regStatus = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, L"\\Registry\\Machine\\Software\\BugCheckHack", query, NULL, NULL);

    if (regStatus != STATUS_SUCCESS)
        RtlInitUnicodeString(&CalledSupport, L"Here is some useless code that wont help you at all!");

    if (OverwriteString((PUNICODE_STRING)EtwpLastBranchEntry5_Address, &CalledSupport))
        return STATUS_DRIVER_INTERNAL_ERROR;

    RtlZeroMemory(query, sizeof(RTL_QUERY_REGISTRY_TABLE) * 2);
    query[0].Name = L"PossibleFixes";
    query[0].Flags = RTL_QUERY_REGISTRY_DIRECT;
    query[0].EntryContext = &PossibleFixes;
    regStatus = RtlQueryRegistryValues(RTL_REGISTRY_ABSOLUTE, L"\\Registry\\Machine\\Software\\BugCheckHack", query, NULL, NULL);

    if (regStatus != STATUS_SUCCESS)
        RtlInitUnicodeString(&PossibleFixes, L"Please visit the website which wont help you at all!");

    if (OverwriteString((PUNICODE_STRING)EtwpLastBranchEntry4_Address, &PossibleFixes))
        return STATUS_DRIVER_INTERNAL_ERROR;


    return STATUS_UNSUCCESSFUL;
}