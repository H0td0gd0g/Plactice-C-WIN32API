#include <stdio.h>
#include <windows.h>

int main() {

    LUID luid;

    LookupPrivilegeValueA(
        NULL, // 特権名を取得するシステムの名前
        "SeDebugPrivilege", // privname
        &luid
    );
}


