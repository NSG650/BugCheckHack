#include <windows.h>
#include "registry.h"
#include "pdb.h"
#include "service.h"

// shit to make it look nicer

#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

// https://stackoverflow.com/a/41808496

BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam) {
    HFONT hfDefault = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    SendMessage(hWnd, WM_SETFONT, (WPARAM)hfDefault, MAKELPARAM(TRUE, 0));
    return TRUE;
}

PCWSTR* ValueNames[6] = {
    L"Frowny",
    L"String One",
    L"String Two",
    L"Website URL",
    L"Called Support",
    L"Possible Fixes"
};

PCWSTR* ButtonNames[4] = {
    L"Save Values",
    L"Create Service",
    L"Delete Service",
    L"Run Service"
};

PCWSTR* RegValueNames[9] = {
    L"FrownyOffset",
    L"StringsOffset",
    L"BgpClearScreenOffset",
    L"Emoticon",
    L"StringOne",
    L"StringTwo",
    L"WebsiteUrl",
    L"CalledSupport",
    L"PossibleFixes"
};

#define KEY L"SOFTWARE\\BugCheckHack"

CHAR FilePathA[MAX_PATH] = { 0 };
WCHAR FilePathW[MAX_PATH] = { 0 };
UCHAR count = 2;

VOID GetWindowsVersion(DWORD* Version) {
    DWORD Dummy;
    DWORD FileInfoSize = GetFileVersionInfoSizeExW(FILE_VER_GET_NEUTRAL, L"KernelBase.dll", &Dummy);
    BYTE* Buffer = (BYTE*)malloc(FileInfoSize);
    GetFileVersionInfoExW(FILE_VER_GET_NEUTRAL, L"KernelBase.dll", Dummy,
        FileInfoSize, Buffer);
    VOID* p = NULL;
    UINT Size = 0;
    VerQueryValueW(Buffer, L"\\", &p, &Size);
    const VS_FIXEDFILEINFO* FileInfo = (const VS_FIXEDFILEINFO*)p;
    Version[0] = HIWORD(FileInfo->dwFileVersionMS);
    Version[1] = LOWORD(FileInfo->dwFileVersionMS);
    Version[2] = HIWORD(FileInfo->dwFileVersionLS);
    Version[3] = LOWORD(FileInfo->dwFileVersionLS);
    free(Buffer);
}

BOOL IsProcessElevated(VOID) {
    BOOL IsElevated = FALSE;
    HANDLE hToken = NULL;
    TOKEN_ELEVATION elevation;
    DWORD dwSize;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        if (hToken) {
            CloseHandle(hToken);
            hToken = NULL;
            return FALSE;
        }
    }


    if (!GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize)) {
        if (hToken) {
            CloseHandle(hToken);
            hToken = NULL;
            return FALSE;
        }
    }

    return elevation.TokenIsElevated;
}

// Kill me

BOOLEAN ParseNtoskrnl(VOID) {
    if (VnDownloadSymbols(NULL, FilePathA,
        FilePathA, MAX_PATH)) {
        MessageBox(NULL, L"Failed to download symbols", NULL, MB_ICONSTOP);
        return FALSE;
    }
    DWORD Address[3] = { 0 };
    DWORD WindowsVersion[4] = { 0 };
    GetWindowsVersion(WindowsVersion);
    if (WindowsVersion[2] >= 19041 && WindowsVersion[2] < 22000) {
        CHAR* SymbolName[3] = { "HalpPCIConfigReadHandlers", "EtwpLastBranchLookAsideList", "BgpClearScreen" };
        if (VnGetSymbols(FilePathA,
            Address, SymbolName, count)) {
            MessageBox(NULL, L"Failed to find the required functions", NULL, MB_ICONSTOP);
            return FALSE;
        }
    }
    else if (WindowsVersion[2] >= 22000) {
        if (WindowsVersion[3] >= 434) {
            CHAR* SymbolName[3] = { "HalpPCIConfigReadHandlers", "EtwpStackLookAsideList", "BgpClearScreen" };
            if (VnGetSymbols(FilePathA,
                Address, SymbolName, count)) {
                MessageBox(NULL, L"Failed to find the required functions", NULL, MB_ICONSTOP);
                return FALSE;
            }
        }
        else {
            CHAR* SymbolName[3] = { "HalpPCIConfigReadHandlers", "EtwpLastBranchLookAsideList", "BgpClearScreen" };
            if (VnGetSymbols(FilePathA,
                Address, SymbolName, count)) {
                MessageBox(NULL, L"Failed to find the required functions", NULL, MB_ICONSTOP);
                return FALSE;
            }
        }
    }
    else {
        MessageBox(NULL, L"This version of Windows is not supported", NULL, MB_ICONSTOP);
        return FALSE;
    }
    for (int i = 0; i < 3; i++) {
        if (!i)
            Address[i] += 0x18;
        RegistryWriteDword32(KEY, RegValueNames[i], Address[i]);
    }
    if (!ServiceStart(L"BugCheckHack")) {
        MessageBox(NULL, L"Failed to start service", NULL, MB_ICONSTOP);
        return FALSE;
    }
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg,
    WPARAM wParam, LPARAM lParam) {

    BOOLEAN checked = TRUE;
    INT first = 5;
    static HWND TextBoxes[6];
    HINSTANCE inst = { 0 };
    switch (msg) {

    case WM_CREATE:

        for (UCHAR i = 0; i < 6; i++) {
            CreateWindow(L"Static", ValueNames[i],
                WS_CHILD | WS_VISIBLE | SS_LEFT,
                5, 10 + (i * 30), 300, 230,
                hwnd, (HMENU)1, NULL, NULL);
            TextBoxes[i] = CreateWindow(L"Edit", NULL,
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                90, 10 + (i * 30), 325, 20, hwnd, (HMENU)i,
                NULL, NULL);
        }

        CreateWindowW(L"Button", L"Aero BSOD",
            WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
            5, 10 + (6 * 30), (10 * 9), 15, hwnd, (HMENU)42, inst, NULL);

        for (int i = 0; i < 4; i++) {
            CreateWindow(L"Button", ButtonNames[i],
                WS_VISIBLE | WS_CHILD,
                first, (7 * 30) + 10, (wcslen(ButtonNames[i]) * 8), 25, hwnd, (HMENU)10 + i, NULL, NULL);
                first += (wcslen(ButtonNames[i]) * 8) + 10;
        }

        break;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
            case 0xa:
                for (INT i = 0; i < 6; i++) {
                    PCWSTR Text = malloc((GetWindowTextLength(TextBoxes[i]) + 1) * sizeof(WCHAR));
                    GetWindowText(TextBoxes[i], Text, GetWindowTextLength(TextBoxes[i]) + 1);
                    RegistryWriteString(KEY, RegValueNames[i + 3], Text);
                    free(Text);
                }
                break;
            case 0xe:
                if (!ServiceCreate(L"BugCheckHack", FilePathW)) {
                    MessageBox(hwnd, L"Failed to create service", NULL, MB_ICONSTOP);
                    break;
                }
                MessageBox(hwnd, L"Successfully created the service", NULL, 0);
                break;
            case 0x12:
                if (!ServiceDelete(L"BugCheckHack")) {
                    MessageBox(hwnd, L"Failed to delete service", NULL, MB_ICONSTOP);
                    break;
                }
                MessageBox(hwnd, L"Successfully deleted the service", NULL, 0);
                break;
            case 0x16:
                if (!ParseNtoskrnl()) {
                    MessageBox(hwnd, L"Failed to modify the bsod", NULL, 0);
                    break;
                }
                MessageBox(hwnd, L"Successfully modified the bsod", NULL, 0);
                break;
            case 0x2a:
                count = 3;
        }
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PWSTR lpCmdLine, int nCmdShow) {

    if (!IsProcessElevated()) {
        MessageBox(NULL, L"Please run the program as administrator!", NULL, MB_ICONSTOP);
        return -1;
    }

    if (!RegistryDoesKeyExist(KEY))
        RegistryCreateKey(KEY);

    GetSystemDirectoryA(FilePathA, MAX_PATH);
    strcat_s(FilePathA, MAX_PATH, "\\ntoskrnl.exe");

    GetWindowsDirectoryW(FilePathW, MAX_PATH * sizeof(WCHAR));
    lstrcatW(FilePathW, L"\\BugCheckHack.sys");

    DWORD dwAttrib = GetFileAttributes(FilePathW);

    if (!(dwAttrib != INVALID_FILE_ATTRIBUTES &&
        !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY))) {
        MessageBox(NULL, L"Make sure to copy over BugCheckHack.sys to your Windows directory before running", NULL, MB_ICONSTOP);
        return -1;
    }

    MSG  msg;
    WNDCLASSW wc = { 0 };
    wc.lpszClassName = L"BugCheckHack";
    wc.hInstance = hInstance;
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(0, IDC_ARROW);

    RegisterClassW(&wc);
    HWND window = CreateWindow(wc.lpszClassName, L"BugCheckHack",
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
        150, 150, 455, (10 * 30), 0, 0, hInstance, 0);

    EnumChildWindows(window, EnumChildProc, 0);

    ShowWindow(window, SW_SHOW);

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}