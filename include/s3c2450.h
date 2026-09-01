#ifndef S3C2450_H
#define S3C2450_H

#include <stdint.h>

#define REG32(addr) (*(volatile uint32_t *)(uintptr_t)(addr))
#define REG8(addr)  (*(volatile uint8_t  *)(uintptr_t)(addr))

/* UART0: MDS2450 host console in the original lab material. */
#define ULCON0      REG32(0x50000000u)
#define UCON0       REG32(0x50000004u)
#define UFCON0      REG32(0x50000008u)
#define UMCON0      REG32(0x5000000Cu)
#define UTRSTAT0    REG32(0x50000010u)
#define UERSTAT0    REG32(0x50000014u)
#define UFSTAT0     REG32(0x50000018u)
#define UMSTAT0     REG32(0x5000001Cu)
#define UTXH0       REG8 (0x50000020u)
#define URXH0       REG8 (0x50000024u)
#define UBRDIV0     REG32(0x50000028u)
#define UDIVSLOT0   REG32(0x5000002Cu)

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
