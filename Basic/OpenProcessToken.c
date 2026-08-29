#include <windows.h>
#include <stdio.h>

// chcp 65001 
// change UTF-8


int main() {
    HANDLE hToken;

    OpenProcessToken (
        GetCurrentProcess(), // 現在のプロセスのトークンを取得
        TOKEN_DUPLICATE,  // https://learn.microsoft.com/ja-jp/windows/win32/secauthz/access-rights-for-access-token-objects
        &hToken
    );

    if (hToken == NULL) {
        printf("失敗\n");
        return 1;
    }
    printf("成功\n");

    CloseHandle(hToken);
    return 0;

}