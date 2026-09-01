#include <stdint.h>
#include "platform.h"
#include "uart.h"
#include "timer.h"
#include "mmu.h"
#include "cp15.h"
#include "bench.h"

#if BENCH_RUNS != 5
#error "This classroom build fixes BENCH_RUNS at 5 because median logic is 5-sample based."
#endif

static void print_kv_hex(const char *k, uint32_t v)
{
    uart_puts(k); uart_puts("="); uart_puthex32(v); uart_nl();
}

static void print_kv_dec(const char *k, uint32_t v)
{
    uart_puts(k); uart_puts("="); uart_putdec(v); uart_nl();
}

static void sort5(uint32_t a[5])
{
    for (unsigned i = 1; i < 5; ++i) {
        uint32_t x = a[i];
        int j = (int)i - 1;
        while (j >= 0 && a[j] > x) { a[j + 1] = a[j]; --j; }
        a[j + 1] = x;
    }
}

typedef uint32_t (*bench_fn)(void);

static void run_one(const char *name, bench_fn fn)
{
    uint32_t ticks[5];
    uart_puts("\n[BENCH] "); uart_puts(name); uart_nl();

    for (unsigned r = 0; r < BENCH_RUNS; ++r) {
        uint16_t s = timer0_read();
        uint32_t checksum = fn();
        uint16_t e = timer0_read();
        uint32_t t = timer0_elapsed(s, e);
        ticks[r] = t;

        uart_puts("run="); uart_putdec(r + 1u);
        uart_puts(" ticks="); uart_putdec(t);
        uart_puts(" hex="); uart_puthex32(t);
        uart_puts(" checksum="); uart_puthex32(checksum);
        uart_nl();

        uart_puts("CSV,"); uart_putdec(MMU_CASE); uart_puts(",");
        uart_puts(name); uart_puts(","); uart_putdec(r + 1u); uart_puts(",");
        uart_putdec(t); uart_nl();
    }

    /* The course uses 5 runs; keep median calculation fixed at 5. */
    uint32_t m[5];
    for (unsigned i = 0; i < 5; ++i) m[i] = ticks[i];
    sort5(m);
    uart_puts("MEDIAN,"); uart_putdec(MMU_CASE); uart_puts(",");
    uart_puts(name); uart_puts(","); uart_putdec(m[2]); uart_nl();
}

int main(void)
{
    uart_init();
    uart_puts("\n========================================\n");
    uart_puts(" MDS2450 ARM926EJ Performance Lab\n");
    uart_puts(" Ubuntu 26.04 cross-build edition\n");
    uart_puts("========================================\n");

    uart_puts("case="); uart_putdec(MMU_CASE); uart_puts(" ");
    uart_puts(mmu_case_name(MMU_CASE)); uart_nl();
    print_kv_hex("cp15_c1_before", cp15_get_control());

    mmu_enable_case(MMU_CASE);

    print_kv_hex("ttbr", cp15_get_ttbr());
    print_kv_hex("cp15_c1_after", cp15_get_control());
    print_kv_hex("bench_region", BENCH_REGION_ADDR);
    print_kv_dec("bench_words", BENCH_WORDS);

    timer0_init_free_running();
    bench_region_init();

    run_one("integer", bench_integer);
    run_one("mem_read", bench_mem_read);
    run_one("mem_write", bench_mem_write);
    run_one("stride64", bench_mem_stride);

    uart_puts("\nDONE. Reset board or download another CASE binary.\n");
    for (;;) { }
}
