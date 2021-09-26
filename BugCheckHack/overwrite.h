#pragma once

#include <ntddk.h>

NTSTATUS OverwriteFrowny(UINT64 Address, PUNICODE_STRING String);
NTSTATUS OverwriteString(PUNICODE_STRING Source, PUNICODE_STRING Replace);