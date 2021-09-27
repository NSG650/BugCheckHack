#include <wdm.h>
#include <ntddk.h>
#include <Zydis/Zydis.h>
#include "common.h"

UINT64 Disassemble_KeBugCheck2(UINT64* Result) {
	UNICODE_STRING FunctionName;
	RtlInitUnicodeString(&FunctionName, L"KeBugCheckEx");
	PVOID KeBugCheckExAddress = MmGetSystemRoutineAddress(&FunctionName);
	if (KeBugCheckExAddress == NULL) {
		return 0;
	}
	ZydisDecoder Decoder;
	ZydisDecoderInit(&Decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
	ZydisFormatter Formatter;
	ZydisFormatterInit(&Formatter, ZYDIS_FORMATTER_STYLE_INTEL);
	UINT64 ReadOffset = 0;
	ZydisDecodedInstruction Instruction;
	ZyanStatus Status;
	CHAR PrintBuffer[128];
	while ((Status = ZydisDecoderDecodeBuffer(&Decoder, (PVOID)((UINT64)KeBugCheckExAddress + ReadOffset), sizeof(KeBugCheckExAddress) + 1, &Instruction)) != ZYDIS_STATUS_NO_MORE_DATA) {
		NT_ASSERT(ZYAN_SUCCESS(Status));
		if (!ZYAN_SUCCESS(Status)) {
			ReadOffset++;
			continue;
		}
		CONST UINT64 InstrAddress = (UINT64)((UINT64)KeBugCheckExAddress + ReadOffset);
		ZydisFormatterFormatInstruction(&Formatter, &Instruction, PrintBuffer, sizeof(PrintBuffer), InstrAddress);
		//Print("+%-4X 0x%-16llX\t\t%hs\n", (ULONG)ReadOffset, InstrAddress, PrintBuffer);
		if ((ULONG)ReadOffset == 0x119) {
			Print("Found KeBugCheck2\n");
			*Result = _strtoui64(&PrintBuffer[5], NULL, 16);
			return 1;
		}
		ReadOffset += Instruction.length;
	}
	return 0;
}

UINT64 Disassemble_KiDisplayBlueScreen(PVOID KeBugCheck2Address, UINT64* Result) {
	ZydisDecoder Decoder;
	ZydisDecoderInit(&Decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
	ZydisFormatter Formatter;
	ZydisFormatterInit(&Formatter, ZYDIS_FORMATTER_STYLE_INTEL);
	UINT64 ReadOffset = 0;
	ZydisDecodedInstruction Instruction;
	ZyanStatus Status;
	CHAR PrintBuffer[128];
	while ((Status = ZydisDecoderDecodeBuffer(&Decoder, (PVOID)((UINT64)KeBugCheck2Address + ReadOffset), 614, &Instruction)) != ZYDIS_STATUS_NO_MORE_DATA) {
		NT_ASSERT(ZYAN_SUCCESS(Status));
		if (!ZYAN_SUCCESS(Status)) {
			ReadOffset++;
			continue;
		}
		CONST UINT64 InstrAddress = (UINT64)((UINT64)KeBugCheck2Address + ReadOffset);
		ZydisFormatterFormatInstruction(&Formatter, &Instruction, PrintBuffer, sizeof(PrintBuffer), InstrAddress);
		//Print("+%-4X 0x%-16llX\t\t%hs\n", (ULONG)ReadOffset, InstrAddress, PrintBuffer);
		if ((ULONG)ReadOffset == 0xA5d) {
			Print("Found KiDisplayBlueScreen\n");
			*Result = _strtoui64(&PrintBuffer[5], NULL, 16);
			return 1;
		}
		ReadOffset += Instruction.length;
	}
	return 0;
}

UINT64 Disassemble_BgpFwDisplayBugCheckScreen(PVOID KiDisplayBlueScreenAddress, UINT64* Result) {
	ZydisDecoder Decoder;
	ZydisDecoderInit(&Decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
	ZydisFormatter Formatter;
	ZydisFormatterInit(&Formatter, ZYDIS_FORMATTER_STYLE_INTEL);
	UINT64 ReadOffset = 0;
	ZydisDecodedInstruction Instruction;
	ZyanStatus Status;
	CHAR PrintBuffer[128];
	while ((Status = ZydisDecoderDecodeBuffer(&Decoder, (PVOID)((UINT64)KiDisplayBlueScreenAddress + ReadOffset), 144, &Instruction)) != ZYDIS_STATUS_NO_MORE_DATA) {
		NT_ASSERT(ZYAN_SUCCESS(Status));
		if (!ZYAN_SUCCESS(Status)) {
			ReadOffset++;
			continue;
		}
		CONST UINT64 InstrAddress = (UINT64)((UINT64)KiDisplayBlueScreenAddress + ReadOffset);
		ZydisFormatterFormatInstruction(&Formatter, &Instruction, PrintBuffer, sizeof(PrintBuffer), InstrAddress);
		//Print("+%-4X 0x%-16llX\t\t%hs\n", (ULONG)ReadOffset, InstrAddress, PrintBuffer);
		if ((ULONG)ReadOffset == 0x240) {
			Print("Found BgpFwDisplayBugCheckScreen\n");
			*Result = _strtoui64(&PrintBuffer[5], NULL, 16);
			return 1;
		}
		ReadOffset += Instruction.length;
	}
	return 0;
}

UINT64 Disassemble_HalpPCIConfigReadHandlers(PVOID BgpFwDisplayBugCheckScreenAddress, UINT64* Result1, UINT64* Result2, UINT64* Result3, UINT64* Result4, UINT64* Result5, UINT64* Result6) {
	ZydisDecoder Decoder;
	ZydisDecoderInit(&Decoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
	ZydisFormatter Formatter;
	ZydisFormatterInit(&Formatter, ZYDIS_FORMATTER_STYLE_INTEL);
	UINT64 ReadOffset = 0;
	ZydisDecodedInstruction Instruction;
	ZyanStatus Status;
	CHAR PrintBuffer[128];
	while ((Status = ZydisDecoderDecodeBuffer(&Decoder, (PVOID)((UINT64)BgpFwDisplayBugCheckScreenAddress + ReadOffset), 895, &Instruction)) != ZYDIS_STATUS_NO_MORE_DATA) {
		NT_ASSERT(ZYAN_SUCCESS(Status));
		if (!ZYAN_SUCCESS(Status)) {
			ReadOffset++;
			continue;
		}
		UINT64 InstrAddress = (UINT64)((UINT64)BgpFwDisplayBugCheckScreenAddress + ReadOffset);
		ZydisFormatterFormatInstruction(&Formatter, &Instruction, PrintBuffer, sizeof(PrintBuffer), InstrAddress);
		// Print("+%-4X 0x%-16llX\t\t%hs\n", (ULONG)ReadOffset, InstrAddress, PrintBuffer);
		if ((ULONG)ReadOffset == 0x116) {
			Print("Found HalpPCIConfigReadHandlers+0x18\n");
			*Result1 = _strtoui64(&PrintBuffer[10], NULL, 16);
		}
		if ((ULONG)ReadOffset == 0x132) {
			Print("FoundEtwpLastBranchLookAsideList+0x60\n");
			*Result2 = _strtoui64(&PrintBuffer[10], NULL, 16);
			PUNICODE_STRING temp = (PUNICODE_STRING)_strtoui64(&PrintBuffer[10], NULL, 16);
			for (UINT8 i = 0; i < sizeof(UNICODE_STRING); i++, temp++) {
				// Print("%ls\n", temp->Buffer);
				if (wcsstr(temp->Buffer, L"and then we'll restart for you")) {
					*Result3 = (UINT64)temp;
				}
				if (wcsstr(temp->Buffer, L"www.windows.com/stopcode")) {
					*Result4 = (UINT64)temp;
				}
				if (wcsstr(temp->Buffer, L"this issue and possible fixes, visit")) {
					*Result5 = (UINT64)temp;
				}
				if (wcsstr(temp->Buffer, L"give them this info:")) {
					*Result6 = (UINT64)temp;
				}
			}
			return 1;
		}
		ReadOffset += Instruction.length;
	}
	return 0;
}
