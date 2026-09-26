#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
// リトルエンディアン

// int *p int型のポインタ
// *p デリファレンス　関節参照

int main(void){
    int value = 0x41414141;
    int *p = &value;

    // printfに渡すときはvoid*にキャストする。
    printf("valueeのアドレス: %p\n", (void*)p);

    int arr[4] = { 0x11, 0x22, 0x33, 0x44 };
    int *p = arr; // 配列名は先頭アドレスを所持している

    for(int i = 0; i < 4; i++){
        printf("[%d] addr=%p val=0x%02X\n", i, (void*)(p + i), *(p + 1));        // val=0x%02X 最低2桁16進数の大文字で埋める
    }

    // バイト単位で歩くにはuint8_t*にキャスト

    uint8_t *bp = (uint8_t *)arr;

    for(int i = 0; i < 16; i++){
        printf("%02X", bp[i]);
    }

    // mallocはvoidポインタを返すvoidポインタは方が決まっていないので何でもいれることができる
    // シェルコードをいれることを想定してunsigned charにキャストする。
    void *raw = malloc(64);
    if (!raw) return EXIT_FAILURE;

    unsigned char *buf = (unsigned char)raw;
    memset(buf, 0x90, 64); // NOP No Opereation sled
    // 0x90はなにもしないと言う意味でsled(そり)のように滑り落ちてくることかこのように呼ばれる。

    printf("割り当てアドレス: %p\n", raw);
    printf("最初の8バイト: ");

    for (int i = 0; i < 8; i++){
        printf("%02X", buf[i]);
        printf("\n");
    }

    free(raw);

    return 0;
}