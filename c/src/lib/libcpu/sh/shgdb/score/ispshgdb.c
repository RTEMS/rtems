/*
 * This file contains the isp frames for the user interrupts.
 * From these procedures __ISR_Handler is called with the vector number
 * as argument.
 *
 * __ISR_Handler is kept in a separate file (cpu_asm.c), because a bug in
 * some releases of gcc doesn't properly handle #pragma interrupt, if a
 * file contains both isrs and normal functions.
 *
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
 *
 *
 *  COPYRIGHT (c) 1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modified to reflect dummy isp entries for GDB SH simulator by Joel.
 */

#include <rtems/system.h>
#include <rtems/score/types.h>

/*
 * This is a exception vector table
 *
 * It has the same structure as the actual vector table (vectab)
 */

void _dummy_isp(uint32_t);

proc_ptr _Hardware_isr_Table[256]={
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,	/* PWRon Reset, Maual Reset,...*/
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
/* trapa 0 -31 */
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp,
_dummy_isp, _dummy_isp, _dummy_isp, _dummy_isp, /* irq 152-155*/
_dummy_isp
};

#define Str(a)#a

/*
 * Some versions of gcc and all version of egcs at least until egcs-1.1b
 * are not able to handle #pragma interrupt correctly if more than 1 isr is
 * contained in a file and when optimizing.
 * We try to work around this problem by using the macro below.
 */
#define isp( name, number, func)\
__asm__ (".global _"Str(name)"\n\t"\
     "_"Str(name)":       \n\t"\
     "    mov.l r0,@-r15   \n\t"\
     "    mov.l r1,@-r15   \n\t"\
     "    mov.l r2,@-r15   \n\t"\
     "    mov.l r3,@-r15   \n\t"\
     "    mov.l r4,@-r15   \n\t"\
     "    mov.l r5,@-r15   \n\t"\
     "    mov.l r6,@-r15   \n\t"\
     "    mov.l r7,@-r15   \n\t"\
     "    mov.l r14,@-r15  \n\t"\
     "    sts.l pr,@-r15   \n\t"\
     "    sts.l mach,@-r15 \n\t"\
     "    sts.l macl,@-r15 \n\t"\
     "    mov r15,r14      \n\t"\
     "    mov.l "Str(name)"_v, r2 \n\t"\
     "    mov.l "Str(name)"_k, r1\n\t"\
     "    jsr @r1           \n\t"\
     "    mov   r2,r4      \n\t"\
     "    mov   r14,r15    \n\t"\
     "    lds.l @r15+,macl \n\t"\
     "    lds.l @r15+,mach \n\t"\
     "    lds.l @r15+,pr   \n\t"\
     "    mov.l @r15+,r14  \n\t"\
     "    mov.l @r15+,r7   \n\t"\
     "    mov.l @r15+,r6   \n\t"\
     "    mov.l @r15+,r5   \n\t"\
     "    mov.l @r15+,r4   \n\t"\
     "    mov.l @r15+,r3   \n\t"\
     "    mov.l @r15+,r2   \n\t"\
     "    mov.l @r15+,r1   \n\t"\
     "    mov.l @r15+,r0   \n\t"\
     "    rte              \n\t"\
     "    nop              \n\t"\
     "    .align 2         \n\t"\
     #name"_k: \n\t"\
     ".long "Str(func)"\n\t"\
     #name"_v: \n\t"\
     ".long "Str(number));

/************************************************
 * Dummy interrupt service procedure for
 * interrupts being not allowed --> Trap 34
 ************************************************/
__asm__ (" .section .text\n\
.global __dummy_isp\n\
__dummy_isp:\n\
      mov.l r14,@-r15\n\
      mov   r15, r14\n\
      trapa #34\n\
      mov.l @r15+,r14\n\
      rte\n\
      nop");

