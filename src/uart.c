#include "s3c2450.h"
#include "platform.h"
#include "uart.h"

void uart_init(void)
{
#if UART_REINIT
    /* 8-N-1, polling mode, FIFO off. Exact baud depends on PCLK. */
    UART_UFCON = 0u;
    UART_UMCON = 0u;
    UART_ULCON = 0x3u;
    UART_UCON  = 0x5u; /* Tx/Rx polling/interrupt request mode using PCLK on classic Samsung UARTs */
    UART_UBRDIV = (PCLK_HZ / (UART_BAUD * 16u)) - 1u;
#else
    /* Keep bootloader UART settings. This is the safest classroom default. */
#endif
}

void uart_putc(char c)
{
    if (c == '\n') {
        while ((UART_UTRSTAT & (1u << 1)) == 0u) { }
        UART_UTXH = (uint8_t)'\r';
    }
    while ((UART_UTRSTAT & (1u << 1)) == 0u) { }
    UART_UTXH = (uint8_t)c;
}

void uart_puts(const char *s)
{
    while (*s) uart_putc(*s++);
}

void uart_nl(void) { uart_putc('\n'); }

void uart_puthex32(uint32_t v)
{
    static const char hex[] = "0123456789ABCDEF";
    uart_puts("0x");
    for (int i = 7; i >= 0; --i)
        uart_putc(hex[(v >> (i * 4)) & 0xFu]);
}

void uart_putdec(uint32_t v)
{
    char buf[11];
    unsigned n = 0;
    if (v == 0u) { uart_putc('0'); return; }
    while (v != 0u && n < sizeof(buf)) {
        buf[n++] = (char)('0' + (v % 10u));
        v /= 10u;
    }
    while (n) uart_putc(buf[--n]);
}
