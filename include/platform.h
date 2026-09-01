#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>

#ifndef MMU_CASE
#define MMU_CASE 2
#endif

#ifndef BENCH_RUNS
#define BENCH_RUNS 5
#endif

#ifndef BENCH_REGION_ADDR
#define BENCH_REGION_ADDR 0x32000000u
#endif

#ifndef BENCH_WORDS
#define BENCH_WORDS 2048u   /* 8 KiB: fits in a typical ARM926 D-cache */
#endif

#ifndef INTEGER_ITERS
#define INTEGER_ITERS 300000u
#endif

#ifndef MEM_REPEATS
#define MEM_REPEATS 128u
#endif

/* Bootloader is expected to have configured UART0 already. */
#ifndef UART_REINIT
#define UART_REINIT 0
#endif

/* The MDS2450 bootloader console is connected to UART1. */
#ifndef UART_CHANNEL
#define UART_CHANNEL 1u
#endif

#if UART_CHANNEL > 3
#error "UART_CHANNEL must be between 0 and 3"
#endif

#ifndef PCLK_HZ
#define PCLK_HZ 66000000u
#endif

#ifndef UART_BAUD
#define UART_BAUD 115200u
#endif

#endif
