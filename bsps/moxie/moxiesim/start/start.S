/* Copyright (C) 2011, 2013, 2014  Anthony Green */

/* moxie start up file. */

#include "bspopts.h"

#if defined(HAVE_OLD_MOXIE_ASM)
#define sub sub.l
#endif
	
        .text
        .global _start
_start:
        ldi.l   $sp, _ISR_Stack_area_end /* load up stack pointer */
        xor     $fp, $fp        /* zero fp to allow unwinders to stop */

        /* zero the bss area */
        ldi.l   $r0, __bss_start__
        xor     $r1, $r1
        ldi.l   $r2, __bss_end__
        sub     $r2, $r0
        jsra    memset

        ldi.l   $r0, 0x0        # pass in NULL
        jsra    boot_card
        jmpa    _start          # restart
.Lend:
        .size   _start,(.Lend-_start)
