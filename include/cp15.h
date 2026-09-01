#ifndef CP15_H
#define CP15_H
#include <stdint.h>
uint32_t cp15_get_control(void);
void cp15_set_control(uint32_t value);
void cp15_set_ttbr(uint32_t addr);
uint32_t cp15_get_ttbr(void);
void cp15_set_domain(uint32_t value);
void cp15_invalidate_tlb(void);
void cp15_invalidate_caches(void);
void cp15_drain_write_buffer(void);
void cp15_flush_prefetch(void);
void cp15_disable_mmu_caches(void);
#endif
