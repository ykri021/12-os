#include "defines.h"
#include "serial.h"
#include "lib.h"
#include "xmodem.h"

#define XMODEM_SOH 0x01 /* ブロックの先頭 */
#define XMODEM_STX 0x02 /*  */
#define XMODEM_EOT 0x04 /* データ終了 */
#define XMODEM_ACK 0x06 /* ACKnowledge */
#define XMODEM_NAK 0x15 /* Negative AcKnowledge */
#define XMODEM_CAN 0x18 /* 中断 */
#define XMODEM_EOF 0x1a /* Ctrl-Z */

#define XMODEM_BLOCK_SIZE 128

/* 受信開始されるまで送信要求を出す */
static int xmodem_wait(void)
{
    long cnt = 0;

    while(!serial_is_recv_enable(SERIAL_DEFAULT_DEVICE)) {
        if (++cnt >= 2000000) {
            cnt = 0;
            serial_send_byte(SERIAL_DEFAULT_DEVICE, XMODEM_NAK);
        }
    }
    return 0;
}

/* ブロック単位での受信 */
static int xmodem_read_block(unsigned char block_number, char *buf)
{
    unsigned char c, block_num, check_sum;
    int i;

    // ブロック番号の受信
    block_num = serial_recv_byte(SERIAL_DEFAULT_DEVICE);
    if(block_num != block_number)
        return -1;
    
    // 反転したブロック番号の受信
    block_num ^= serial_recv_byte(SERIAL_DEFAULT_DEVICE);
    if(block_num != 0xff)
        return -1;
    
    // 128バイトのデータを受信する
    check_sum = 0;
    for (i = 0; i < XMODEM_BLOCK_SIZE; i++) {
        c = serial_recv_byte(SERIAL_DEFAULT_DEVICE);
        *(buf++) = c;
        check_sum += c;
    }

    // チェックサムの受信
    check_sum ^= serial_recv_byte(SERIAL_DEFAULT_DEVICE);
    if(check_sum)
        return -1;
    
    return i;
}

long xmodem_recv(char *buf)
{
    int r, receiving = 0;
    long size = 0;
    unsigned char c, block_number = 1;

    while(1) {
        if (!receiving)
          xmodem_wait(); /* 受信開始されるまで送信要求を出す */
        
        c = serial_recv_byte(SERIAL_DEFAULT_DEVICE); // 1文字の受信

        if (c == XMODEM_EOT) { /* 受信完了 */
            serial_send_byte(SERIAL_DEFAULT_DEVICE, XMODEM_ACK);
            break;
        } else if (c == XMODEM_CAN) { /* 受信中断 */
            return -1;
        } else if (c == XMODEM_SOH) { /* 受信開始 */
            receiving++;
            r = xmodem_read_block(block_number, buf); /* ブロック単位での受信 */
            if (r < 0) { /* 受信エラー */
                serial_send_byte(SERIAL_DEFAULT_DEVICE, XMODEM_NAK);
            } else { /* 正常受信 正常時にはバッファのポインタを進め、ACKを返す*/
                block_number++;
                size += r;
                buf += r;
                serial_send_byte(SERIAL_DEFAULT_DEVICE, XMODEM_ACK);
            }
        } else {
            if (receiving)
                return -1;
        }
    }

    return size;
}
