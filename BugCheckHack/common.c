#include "common.h"
#include <stdarg.h>
#include <stdio.h>

VOID Print(PCCH Format, ...) {
    CHAR message[512];
    va_list argList;
    va_start(argList, Format);
    const int n = _vsnprintf_s(message, sizeof(message), sizeof(message) - 1, Format, argList);
    message[n] = '\0';
    vDbgPrintExWithPrefix("[BCH] ", DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, message, argList);
    va_end(argList);
}