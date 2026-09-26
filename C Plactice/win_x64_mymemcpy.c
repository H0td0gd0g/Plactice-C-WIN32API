typedef unsigned long long size_t; //windows ではlongは4バイトでlong longは8バイト


void* origmemcpy( void *dest, const void *src, size_t n){
    unsigned char *d = (unsigned char*)dest;
    const unsigned char *s = (const unsigned char*)src;

    for(size_t i = 0; i < n; i++){
        d[i] = s[i];   
    }

    // 結果的に同じ位置のメモリに書き込まれる
    return dest;
}