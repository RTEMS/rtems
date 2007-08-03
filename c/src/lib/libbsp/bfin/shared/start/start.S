

#include <rtems/bfin/bfin.h>   

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


    .text
    .align  2

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
    p0.l = _boot_card;
    p0.h = _boot_card;

    call (p0);

    p0.l = _exit;
    p0.h = _exit; 
    P3 = P4; 
    jump    (p0)        /* Should not return.  */

.global _null_isr
_null_isr:
    rti;

