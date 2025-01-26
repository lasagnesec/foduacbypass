#include "beacon.h"
#include <windows.h>

DECLSPEC_IMPORT HMODULE WINAPI KERNEL32$LoadLibraryA(LPCSTR lpLibFileName);
DECLSPEC_IMPORT FARPROC WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);

typedef LONG (WINAPI *RegDeleteTreeA_t)(HKEY, LPCSTR);

typedef VOID (WINAPI *BeaconPrintf_t)(int type, LPCSTR fmt, ...);

void go(char *args, int len) {
    HMODULE advapi32 = KERNEL32$LoadLibraryA("Advapi32.dll");
    if (!advapi32) {
        BeaconPrintf(CALLBACK_ERROR, "Failed to load Advapi32.dll");
        return;
    }

    RegDeleteTreeA_t ADVAPI32$RegDeleteTreeA = (RegDeleteTreeA_t)KERNEL32$GetProcAddress(advapi32, "RegDeleteTreeA");
    if (!ADVAPI32$RegDeleteTreeA) {
        BeaconPrintf(CALLBACK_ERROR, "Failed to resolve RegDeleteTreeA function");
        return;
    }

    // Perform registry cleanup
    if (ADVAPI32$RegDeleteTreeA(HKEY_CURRENT_USER, "Software\\Classes\\ms-settings") != ERROR_SUCCESS) {
        BeaconPrintf(CALLBACK_ERROR, "Failed to clean up registry keys");
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "[+] Registry keys cleaned up successfully");
    }
}