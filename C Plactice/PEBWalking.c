/*
PEB(Process Enviroment Blocl)はwindowsがプロセスごとに管理する
内部データ構造。DLLのベースアドレス一覧、プロセスパラメータ、ヒープ情報
が詰まっている
インポートテーブルを使わずにAPIを解決するテクニックはこれを使用する
*/

#include <windows.h>
#include <winternl.h>
#include <stdio.h>

PPEB get_peb(void) {
    #ifdef _M_X64
        return (PPEB)__readgsdword(0x60);
    #elif defined(_M_IX86)
        return (PPEB)__readfsdword(0x30);
    #endif
}

int main(void){
    PPEB peb = get_peb();

    printf("PEB addres: %p\n", (void*)peb);
    printf("ImageBaseAddress:      %p\n", peb->Reserved3[1]);  // EXE のベース
    printf("BeingDebugged:         %d\n", peb->BeingDebugged); // デバッガ検出
    
    return 0;
}


























