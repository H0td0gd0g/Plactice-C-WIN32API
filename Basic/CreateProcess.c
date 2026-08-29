# include <windows.h>

int main() {

    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // 何バイト分0にするかわかんないのでsizeofで教える
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));


    CreateProcess(
        NULL,
        "cmd.exe",
        NULL,
        NULL,
        FALSE,
        0,
        NULL,
        NULL,
        &si,
        &pi
    );
    return 0;
}