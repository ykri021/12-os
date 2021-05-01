# include "defines.h"
# include "serial.h"
# include "lib.h"

/* memset関数 (メモリを特定のバイトデータで埋める)を実装 */
void *memset(void *b, int c, long len)
{
    char *p;
    for(p = b; len > 0; len--)
        *(p++) = c;
    return b;
}

/* memcpy関数 (メモリのコピー)を実装 */
void *memcpy(void *dst, const void *src, long len)
{
    char *d = dst;
    const char *s = src;
    for(; len > 0; len--)
        *(d++) = *(s++);
    return dst;
}
/* memcmp関数 (メモリの比較)を実装 
 * 戻り値: p1=p2であれば0, p1>p2であれば1, p1<p2であれば-1
*/
int memcmp(const void *b1, const void *b2, long len)
{
    const char *p1 = b1, *p2 = b2;
    for(; len > 0; len--) {
        if (*p1 != *p2)
            return (*p1 > *p2) ? 1 : -1;
        p1++;
        p2++;
    }
    return 0;
}
/* strlen関数 (文字列の長さ)を実装 */
int strlen(const char *s)
{
    int len;
    for(len = 0; *s; s++, len++)
        ;
    return len;
}
/* strcpy関数 (文字列のコピー)を実装 */
char *strcpy(char *dst, const char *src)
{
    char *d = dst;
    for(;; dst++, src++)
    {
        *dst=*src;
        if (!*src) break;
    }
    return d;
}
/* strcmp関数 (文字列の比較)を実装 */
int strcmp(const char *s1, const char *s2)
{
    while (*s1 || *s2)
    {
        if(*s1 != *s2)
            return (*s1 > *s2) ? 1 : -1;
        s1++;
        s2++;
    }
    return 0;
}
/* strncmp関数 (長さ指定での文字列の比較)を実装 */
int strncmp(const char *s1, const char *s2, int len)
{
    while((*s1 || *s2) && (len > 0)) {
        if (*s1 != *s2)
            return (*s1 > *s2) ? 1 : -1;
        s1++;
        s2++;
        len--;
    }
    return 0;
}

/* 1文字送信
 * コンソールへの入出力関数 
*/
int putc(unsigned char c) 
{
    if(c == '\n')
        serial_send_byte(SERIAL_DEFAULT_DEVICE, '\r');
    return serial_send_byte(SERIAL_DEFAULT_DEVICE, c); // serial_send_byte関数は「シリアルへの」文字出力関数
}

/* 文字列送信 */
int puts(unsigned char *str)
{
    while (*str)
        putc(*(str++));
    return 0;
}

/* 数値の16進表示 
 * 引数
 * unsigned long value: 表示したい値
 * int column: 表示桁数
*/

int putxval(unsigned long value, int column)
{
    // 文字列出力用バッファ
    char buf[9];
    char *p;

    // 下の桁から処理するので、バッファの終端から利用する
    p = buf + sizeof(buf) - 1;
    *(p--) = '\0';
    
    if (!value && !column)
        column++;
    
    while (value || column) {
        // 16進文字に変換してバッファに格納する
        *(p--) = "0123456789abcdef"[value & 0xf];
        // 次の桁に進める
        value >>= 4;
        // 桁数指定がある場合にはカウントする
        if (column) column--;
    }
    // バッファの内容を出力する
    puts(p + 1);

    return 0;
}
