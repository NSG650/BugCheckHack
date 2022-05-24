#pragma once

#include <Windows.h>

BOOLEAN ServiceCreate(PCWSTR Name, PCWSTR Path);
BOOLEAN ServiceDelete(PCWSTR Name);
BOOLEAN ServiceStart(PCWSTR Name);