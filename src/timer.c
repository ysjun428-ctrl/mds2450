#include "s3c2450.h"
#include "timer.h"

void timer0_init_free_running(void)
{
    /* Original course setting: prescaler0=255, timer0 divider=1/16.
       If PCLK is 66 MHz, timer tick is about 16.1 kHz. */
    TCON &= ~0xFu;
    TCFG0 = (TCFG0 & ~0xFFu) | 0xFFu;
    TCFG1 = (TCFG1 & ~0xFu) | 0x3u; /* 0x3 => 1/16 */
    TCNTB0 = 0xFFFFu;
    TCMPB0 = 0u;

    /* auto-reload + manual update, then auto-reload + start */
    TCON = (TCON & ~0xFu) | (1u << 3) | (1u << 1);
    TCON = (TCON & ~0xFu) | (1u << 3) | (1u << 0);
}

uint16_t timer0_read(void)
{
    return (uint16_t)(TCNTO0 & 0xFFFFu);
}

uint32_t timer0_elapsed(uint16_t start, uint16_t end)
{
    /* Down-counter; modulo arithmetic is valid if elapsed < 65536 ticks. */
    return (uint16_t)(start - end);
}
