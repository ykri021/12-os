#include "defines.h"
#include "serial.h"
#include "xmodem.h"
#include "elf.h"
#include "lib.h"

volatile int value = 10;

static int init(void)
{
  /* 以下はリンカスクリプトで定義してあるシンボル 
   * リンカ・スクリプトで定義されたシンボルを参照可能にする
   */
  extern int erodata, data_start, edata, bss_start, ebss;

  /* データ領域とBSS領域を初期化する。この処理以降でないと、 
   * グローバル変数が初期化されていないので注意
   */
  memcpy(&data_start, &erodata, (long)&edata - (long)&data_start);
  memset(&bss_start, 0, (long)&ebss - (long)&bss_start);

  /* シリアルデバイスの初期化 */
  serial_init(SERIAL_DEFAULT_DEVICE);
  
  return 0;
}

/* メモリの16進ダンプ出力 */
static int dump(char *buf, long size)
{
  long i;

  if(size < 0)
  {
    puts("no data.\n");
    return -1;
  }
  for (i = 0; i < size; i++) {
    putxval(buf[i], 2);

    if ((i & 0xf) == 15) {
      puts("\n");
    } else {
      if ((i & 0xf) == 7) puts(" ");
      puts(" ");
    }
  }
  puts("\n");
  return 0;
}

static void wait()
{
  volatile long i;
  for(i = 0; i < 300000; i++)
    ;
}

int main(void)
{
    static char buf[16];
    static long size = -1;
    static unsigned char *loadbuf = NULL;
    extern int buffer_start; /* リンカスクリプトで定義されているバッファ */
    char * entry_point;
    void (*f)(void);

    init();
    puts("kzload (kozos boot loader) started.\n");

    while (1) {//無限ループ
      puts("kzload> "); /* プロンプト表示 */
      gets(buf); /* シリアルからのコマンド受信 */

      if (!strcmp(buf, "load")) { /* XMODEMでのファイルのダウンロード */
        loadbuf = (char *)(&buffer_start);
        size = xmodem_recv(loadbuf);
        wait(); /* 転送アプリが終了し端末に制御が戻るまで待ち合わせる */
        if (size < 0) {
          puts("\nXMODEM recieve error!\n");
        } else {
          puts("\nXMODEM recieve succeeded.\n");
        }
      } else if (!strcmp(buf, "dump")) { /* メモリの16進ダンプ出力 */
        puts("size: ");
        putxval(size, 0);
        puts("\n");
        dump(loadbuf, size);
      } else if (!strcmp(buf, "run")) { /* ELF形式ファイルの実行 */
        entry_point = elf_load(loadbuf); /* メモリ上に展開（ロード） */
        if (!entry_point) {
          puts("run error!\n");
        } else {
          puts("starting from entry point: ");
          putxval((unsigned long)entry_point, 0);
          puts("\n");
          f = (void (*) (void))entry_point;
          f(); /* ここで、ロードしたプログラムに処理を渡す */
          /* ここには返ってこない */
        }
      } else {
        puts("unknown\n");
      }
    }

    return 0;
}
