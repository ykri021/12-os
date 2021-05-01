#include "defines.h"
#include "serial.h"
#include "lib.h"

int main(void)
{
    serial_init(SERIAL_DEFAULT_DEVICE); //シリアルデバイスを初期化
    puts("Hello World!\n");
    while(1) //無限ループ
      ;
    return 0;
}
