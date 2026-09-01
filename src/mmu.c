#include "mmu.h"
#include "cp15.h"
#include "platform.h"

extern uint32_t __ttb_start__[];

static void map_sections(uint32_t va_start, uint32_t va_end, uint32_t pa_start, uint32_t attr)
{
    uint32_t *ttb = __ttb_start__;
    uint32_t va = va_start & 0xFFF00000u;
    uint32_t end = va_end & 0xFFF00000u;
    uint32_t pa = pa_start & 0xFFF00000u;
    for (;;) {
        ttb[va >> 20] = pa | attr;
        if (va == end) break;
        va += 0x00100000u;
        pa += 0x00100000u;
    }
}

uint32_t *mmu_table_base(void) { return __ttb_start__; }

void mmu_build_identity_table(unsigned case_id)
{
    uint32_t *ttb = __ttb_start__;

    /* Safe default: all 4 GB identity-mapped, non-cacheable/non-bufferable. */
    for (uint32_t i = 0; i < 4096u; ++i)
        ttb[i] = (i << 20) | RW_NCNB;

    if (case_id == 1u) {
        /* CASE 1: all SDRAM areas NC/NB. */
        map_sections(0x30000000u, 0x33F00000u, 0x30000000u, RW_NCNB);
    } else if (case_id == 2u) {
        /* CASE 2 from the course slides. */
        map_sections(0x30000000u, 0x31F00000u, 0x30000000u, RW_CB);
        map_sections(0x32000000u, 0x33E00000u, 0x32000000u, RW_NCNB);
        map_sections(0x33F00000u, 0x33F00000u, 0x33F00000u, RW_CB);
    } else if (case_id == 3u) {
        /* CASE 3: entire 64 MiB SDRAM C/B. */
        map_sections(0x30000000u, 0x33F00000u, 0x30000000u, RW_CB);
    }
}

void mmu_enable_case(unsigned case_id)
{
    if (case_id == 0u) {
        cp15_disable_mmu_caches();
        return;
    }

    cp15_disable_mmu_caches();
    mmu_build_identity_table(case_id);
    cp15_set_ttbr((uint32_t)(uintptr_t)__ttb_start__);

    /* Domain 0 = client. AP bits in descriptors therefore apply. */
    cp15_set_domain(0x00000001u);
    cp15_invalidate_tlb();
    cp15_invalidate_caches();

    uint32_t c1 = cp15_get_control();
    c1 |= (1u << 0);   /* MMU */
    c1 |= (1u << 2);   /* D-cache */
    c1 |= (1u << 12);  /* I-cache */
    cp15_set_control(c1);
    cp15_flush_prefetch();
}

const char *mmu_case_name(unsigned case_id)
{
    switch (case_id) {
        case 0: return "CASE0 MMU/CACHE OFF";
        case 1: return "CASE1 ALL SDRAM NC/NB";
        case 2: return "CASE2 AREA1+3 C/B, AREA2 NC/NB";
        case 3: return "CASE3 ALL SDRAM C/B";
        default:return "UNKNOWN";
    }
}
