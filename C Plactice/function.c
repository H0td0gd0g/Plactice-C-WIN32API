#include <stdio.h>

// 関数のアドレスをいれる変数＝変数ポインタ
typedef int (*OPERATION)(int, int);

// intを２つ受け取ってint型を返す関数へのポインタ型にOPERATIONという名前をつける


int add(int a, int b) { return a + b; }
int mul(int a, int b) { return a * b; }

int main(void){
    OPERATION op = add;
    // addがあるメモリをopに渡している
    printf("add(3, 4) = %d\n", op(3, 4));

    op = mul; // 別の関数をいれる
    printf("mul(3, 4) = %d\n", op(3, 4));

    // 生のアドレスとして見る
    printf("add のアドレス: %p\n", (void *)add);
    printf("mul のアドレス: %p\n", (void *)mul);

    // シェルコードを実行するパターン (概念)
    // unsigned char sc[] = { 0x90, ... };
    // void (*shellcode)(void) = (void (*)(void))sc;
    // shellcode();  // メモリが実行可能なら動く

    return 0;
}

