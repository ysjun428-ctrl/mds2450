#ifndef TIMER_H
#define TIMER_H
#include <stdint.h>
void timer0_init_free_running(void);
uint16_t timer0_read(void);
uint32_t timer0_elapsed(uint16_t start, uint16_t end);
#endif
