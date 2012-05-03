
#include <rtems/bfin/bfin.h>

#include <bspopts.h>

#ifndef LO
#define LO(con32) ((con32) & 0xFFFF)
#endif
#ifndef HI
#define HI(con32) (((con32) >> 16) & 0xFFFF)
#endif


        .section .init
        .globl  __init
        .type   __init,@function
__init:
        .section .fini
        .globl  __fini
        .type   __fini,@function
__fini:


#if (BFIN_ON_SKYEYE)
    .section .init
#else
    .section .l1code
#endif
    .align  4

    .global __start
__start:

    /* Start by setting up a stack */
    sp.h = 0xFFB0;
    sp.l = 0x0F00;

    /* Maybe we should zero the memory in the .bss section.  */

    /* This changes to the supervisor mode */
    p0.l = START;
    p0.h = START;
    p1.l = LO(EVT15);
    p1.h = HI(EVT15);

    [P1] = P0;

    P0.h = HI(IMASK);
    P0.l = LO(IMASK);
    R0 = [P0];
    /* R1.l = EVT_IVG15 & 0xFFFF; */
    R1.l = 0x8000;

    R0 = R0 | R1;

    [P0] = R0;

    RAISE 15;

    P0.l = WAIT;
    P0.h = WAIT;

    RETI = P0;
    RTI;

    /* endless loop to wait */
    WAIT:
    jump WAIT;

    START:
    [--SP] = RETI;

    p0.h = _bss_start;
    p0.l = _bss_start;
    p1.h = _end;
    p1.l = _end;
    r0 = p0;
    r1 = p1;
    r1 = r1 - r0;
    p1 = r1;
    r0 = 0;

    /* Set _bss_start until _end to zero */
    lsetup(loop1,loop2) LC0 = p1;
    loop1:     b[p0] = r0;
    loop2:  p0 +=1;

    /* call boot_card( 0, 0 ) */
    r0 = 0;
    r1 = 0;
    p0.l = _boot_card;
    p0.h = _boot_card;

    call (p0);

    HLT
    p0.l = _exit;
    p0.h = _exit;
    P3 = P4;
    jump    (p0)        /* Should not return.  */

.global _null_isr
_null_isr:
    rti;

