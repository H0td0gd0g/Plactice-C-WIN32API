#include <stdio.h>
#include <windows.h>

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


    LookupPrivilegeValueA(
        NULL, // 特権名を取得するシステムの名前
        "SeDebugPrivilege", // privname
        &luid
    );
    
    // add
    tp.Privileges[0].Luid = luid;

    OpenProcessToken (
        GetCurrentProcess(), // 現在のプロセスのトークンを取得
        TOKEN_DUPLICATE|TOKEN_ADJUST_PRIVILEGES,  // https://learn.microsoft.com/ja-jp/windows/win32/secauthz/access-rights-for-access-token-objects
        &hToken
    );

    if (hToken == NULL) {
        printf("失敗\n");
        return 1;
    }
    printf("成功\n");


    AdjustTokenPrivileges (
        hToken,
        FALSE,
        &tp,
        0,
        NULL,
        NULL
    );

    // clean up
    CloseHandle(hToken);
}