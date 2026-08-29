#include <windows.h>
#include <stdio.h>

// chcp 65001 
// change UTF-8


int main() {
    HANDLE hToken;
    HANDLE hNewToken;

    OpenProcessToken (
        GetCurrentProcess(), // 現在のプロセスのトークンを取得
        TOKEN_DUPLICATE,  // https://learn.microsoft.com/ja-jp/windows/win32/secauthz/access-rights-for-access-token-objects
        &hToken
    );

    if (hToken == NULL) {
        printf("失敗\n");
        return 1;
    }
    printf("現在のプロセストークンの取得に成功しました。\n");

    DuplicateTokenEx (
        hToken,
        MAXIMUM_ALLOWED, // 取得できるすべての権限を取得する
        NULL,
        SecurityImpersonation, // https://learn.microsoft.com/ja-jp/windows/win32/api/winnt/ne-winnt-security_impersonation_level
        TokenPrimary,
        &hNewToken
    );
    
    if (hNewToken == NULL) {
        printf("失敗\n");
        return 1;
    }
    printf("トークンの複製に成功しました。\n");

    CloseHandle(hToken);
    CloseHandle(hNewToken);
    return 0;
}