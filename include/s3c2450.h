#ifndef S3C2450_H
#define S3C2450_H

#include <stdint.h>

#define REG32(addr) (*(volatile uint32_t *)(uintptr_t)(addr))
#define REG8(addr)  (*(volatile uint8_t  *)(uintptr_t)(addr))

/* UART channels are spaced 0x4000 bytes apart; MDS2450 uses UART1. */
#define UART_BASE   (0x50000000u + (UART_CHANNEL * 0x4000u))
#define UART_ULCON  REG32(UART_BASE + 0x00u)
#define UART_UCON   REG32(UART_BASE + 0x04u)
#define UART_UFCON  REG32(UART_BASE + 0x08u)
#define UART_UMCON  REG32(UART_BASE + 0x0Cu)
#define UART_UTRSTAT REG32(UART_BASE + 0x10u)
#define UART_UTXH   REG8 (UART_BASE + 0x20u)
#define UART_UBRDIV REG32(UART_BASE + 0x28u)

/* PWM Timer block. TCFG1 canonical offset is +0x04. */
#define TCFG0       REG32(0x51000000u)
#define TCFG1       REG32(0x51000004u)
#define TCON        REG32(0x51000008u)
#define TCNTB0      REG32(0x5100000Cu)
#define TCMPB0      REG32(0x51000010u)
#define TCNTO0      REG32(0x51000014u)

/* GPIO G - useful for optional LED timing/marker experiments. */
#define GPGCON      REG32(0x56000060u)
#define GPGDAT      REG32(0x56000064u)
#define GPGUDP      REG32(0x56000068u)

#endif
