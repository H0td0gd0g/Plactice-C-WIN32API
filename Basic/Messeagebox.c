#include <windows.h>

int main() {
    MessageBox (
        NULL, // parent window
        "Hello!", // message
        "test", // title
        MB_OK // button
    );
}