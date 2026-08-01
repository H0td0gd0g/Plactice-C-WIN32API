# include <stdio.h>
# include <windows.h>
# include <string.h>
# include <tlhelp32.h>



int main(){

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    HANDLE hsprocess = CreateToolhelp32Snapshot(
    TH32CS_SNAPPROCESS,
    0
    );
    Process32First(hsprocess,&pe);
    
    do{
        printf("PID: %lu Name: %s\n", pe.th32ProcessID, pe.szExeFile);
        if (strcmp(pe.szExeFile, "notepad.exe") == 0){
            printf("Found notepad.exe");
            break;
        };
    }while( Process32Next(hsprocess, &pe));
}

