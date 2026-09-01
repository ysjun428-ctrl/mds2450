#include <stdint.h>
#include "platform.h"
#include "bench.h"
#include "cp15.h"

static volatile uint32_t sink;
static volatile uint32_t * const region = (volatile uint32_t *)(uintptr_t)BENCH_REGION_ADDR;

void bench_region_init(void)
{
    for (uint32_t i = 0; i < BENCH_WORDS; ++i)
        region[i] = 0x12340000u ^ (i * 2654435761u);
    cp15_drain_write_buffer();
}

uint32_t bench_integer(void)
{
    uint32_t x = 0x13579BDFu;
    for (uint32_t i = 0; i < INTEGER_ITERS; ++i) {
        x += i * 33u;
        x ^= x >> 7;
        x = (x << 3) | (x >> 29);
    }
    sink = x;
    return x;
}

uint32_t bench_mem_read(void)
{
    uint32_t sum = 0u;
    for (uint32_t r = 0; r < MEM_REPEATS; ++r)
        for (uint32_t i = 0; i < BENCH_WORDS; ++i)
            sum += region[i];
    sink = sum;
    return sum;
}

uint32_t bench_mem_write(void)
{
    uint32_t v = 0xA5A50000u;
    for (uint32_t r = 0; r < MEM_REPEATS; ++r) {
        for (uint32_t i = 0; i < BENCH_WORDS; ++i)
            region[i] = v + i + r;
    }
    cp15_drain_write_buffer();
    sink = region[BENCH_WORDS - 1u];
    return sink;
}

uint32_t bench_mem_stride(void)
{
    uint32_t sum = 0u;
    const uint32_t stride = 16u; /* 64-byte stride for 32-bit words */
    for (uint32_t r = 0; r < (MEM_REPEATS * 4u); ++r)
        for (uint32_t i = 0; i < BENCH_WORDS; i += stride)
            sum += region[i];
    sink = sum;
    return sum;
}
