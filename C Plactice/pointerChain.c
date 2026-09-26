#include <stdio.h>


int main(void){

    int a = 10, b = 20;
    int *p = &a;
    int **pp = &p;

    *p = 30;
    printf("%d\n", a); // 30

    p = &b;
    printf("%d\n", *p); // 20

    **pp = 99;
    printf("%d\n", b); // 99
    printf("%p\n", *pp); // &p　でリファレンスは
}
















