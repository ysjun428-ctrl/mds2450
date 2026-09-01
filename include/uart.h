#ifndef UART_H
#define UART_H
#include <stdint.h>
void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
void uart_puthex32(uint32_t v);
void uart_putdec(uint32_t v);
void uart_nl(void);
#endif
