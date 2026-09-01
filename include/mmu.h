#ifndef MMU_H
#define MMU_H
#include <stdint.h>

/* ARMv5/ARM926 Level-1 section descriptor fields. */
#define MMU_DESC_SECTION       (0x2u)
#define MMU_DESC_B             (1u << 2)
#define MMU_DESC_C             (1u << 3)
#define MMU_DESC_BIT4_ONE      (1u << 4)  /* shown as fixed '1' in the course descriptor diagram */
#define MMU_DESC_DOMAIN0       (0u << 5)
#define MMU_DESC_AP_FULL       (3u << 10)
#define MMU_DESC_BASE          (MMU_DESC_SECTION | MMU_DESC_BIT4_ONE | MMU_DESC_DOMAIN0 | MMU_DESC_AP_FULL)

#define RW_NCNB                (MMU_DESC_BASE)
#define RW_NCB                 (MMU_DESC_BASE | MMU_DESC_B)
#define RW_CNB                 (MMU_DESC_BASE | MMU_DESC_C)
#define RW_CB                  (MMU_DESC_BASE | MMU_DESC_C | MMU_DESC_B)

void mmu_build_identity_table(unsigned case_id);
void mmu_enable_case(unsigned case_id);
const char *mmu_case_name(unsigned case_id);
uint32_t *mmu_table_base(void);
#endif
