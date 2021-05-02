#include "defines.h"
#include "serial.h"
#include "lib.h"

volatile int value = 10;

int main(void)
{
    serial_init(SERIAL_DEFAULT_DEVICE); //シリアルデバイスを初期化
    puts("Hello World!\n");
    
    putxval(value, 0);   puts("\n");
    value = 20;
    putxval(value, 0); puts("\n");

    while (1) //無限ループ
      ;
    return 0;
}
