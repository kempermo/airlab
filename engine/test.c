#include <string.h>

__attribute__((import_module("env"), import_name("al_clear")))
extern void al_clear(int c);

__attribute__((import_module("env"), import_name("al_write")))
extern void _al_write(int x, int y, int f, int c, const void* s, int sl);
void al_write(int x, int y, int f, int c, const char* s) {
    _al_write(x, y, f, c, s, strlen(s));
}

int main() {
    // clear screen
    al_clear(0);

    // write text
    al_write(10, 10, 8, 1, "Hello, World!");

    return 0;
}
