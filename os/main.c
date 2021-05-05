#include "defines.h"
#include "serial.h"
#include "lib.h"

int main(void)
{
    static char buf[32];

    puts("Helllo World!\n");

    while (1) {
        puts("> ");
        gets(buf); /* コンソールからの一行入力 */

        if (!strncmp(buf, "echo", 4)) {
            puts(buf + 4);
            puts("\n");
        } else if (!strcmp(buf, "exit")) {
            break;
        } else {
            puts("unknown.\n");
        }
    }
    return 0;
}
