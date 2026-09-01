.syntax unified
.arch armv5te
.arm

.section .text.start, "ax", %progbits
.global _start
.type _start, %function
.extern main
.extern __bss_start__
.extern __bss_end__
.extern __stack_top__

_start:
    /* SVC mode, IRQ/FIQ disabled. Bootloader already initialized SDRAM/clocks. */
    msr     cpsr_c, #0xD3
    ldr     sp, =__stack_top__

    /* Clear .bss */
    ldr     r0, =__bss_start__
    ldr     r1, =__bss_end__
    mov     r2, #0
1:
    cmp     r0, r1
    strlo   r2, [r0], #4
    blo     1b

    bl      main
2:
    b       2b
.size _start, .-_start
