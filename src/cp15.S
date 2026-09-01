.syntax unified
.arch armv5te
.arm

.text
.align 2

.global cp15_get_control
.type cp15_get_control, %function
cp15_get_control:
    mrc p15, 0, r0, c1, c0, 0
    bx lr

.global cp15_set_control
.type cp15_set_control, %function
cp15_set_control:
    mcr p15, 0, r0, c1, c0, 0
    mov r1, #0
    mcr p15, 0, r1, c7, c5, 4      /* flush prefetch buffer */
    bx lr

.global cp15_set_ttbr
.type cp15_set_ttbr, %function
cp15_set_ttbr:
    mcr p15, 0, r0, c2, c0, 0
    bx lr

.global cp15_get_ttbr
.type cp15_get_ttbr, %function
cp15_get_ttbr:
    mrc p15, 0, r0, c2, c0, 0
    bx lr

.global cp15_set_domain
.type cp15_set_domain, %function
cp15_set_domain:
    mcr p15, 0, r0, c3, c0, 0
    bx lr

.global cp15_invalidate_tlb
.type cp15_invalidate_tlb, %function
cp15_invalidate_tlb:
    mov r0, #0
    mcr p15, 0, r0, c8, c7, 0
    bx lr

.global cp15_invalidate_caches
.type cp15_invalidate_caches, %function
cp15_invalidate_caches:
    mov r0, #0
    mcr p15, 0, r0, c7, c7, 0      /* invalidate I+D caches */
    bx lr

.global cp15_drain_write_buffer
.type cp15_drain_write_buffer, %function
cp15_drain_write_buffer:
    mov r0, #0
    mcr p15, 0, r0, c7, c10, 4
    bx lr

.global cp15_flush_prefetch
.type cp15_flush_prefetch, %function
cp15_flush_prefetch:
    mov r0, #0
    mcr p15, 0, r0, c7, c5, 4
    bx lr

.global cp15_disable_mmu_caches
.type cp15_disable_mmu_caches, %function
cp15_disable_mmu_caches:
    mov r1, #0
    mcr p15, 0, r1, c7, c10, 4     /* drain write buffer */
    mrc p15, 0, r0, c1, c0, 0
    bic r0, r0, #(1 << 12)          /* I-cache off */
    bic r0, r0, #(1 << 2)           /* D-cache off */
    bic r0, r0, #1                  /* MMU off */
    mcr p15, 0, r0, c1, c0, 0
    mcr p15, 0, r1, c7, c7, 0      /* invalidate caches */
    mcr p15, 0, r1, c8, c7, 0      /* invalidate TLB */
    mcr p15, 0, r1, c7, c5, 4      /* flush prefetch */
    bx lr
