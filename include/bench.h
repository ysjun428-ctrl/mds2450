#ifndef BENCH_H
#define BENCH_H
#include <stdint.h>
void bench_region_init(void);
uint32_t bench_integer(void);
uint32_t bench_mem_read(void);
uint32_t bench_mem_write(void);
uint32_t bench_mem_stride(void);
#endif
