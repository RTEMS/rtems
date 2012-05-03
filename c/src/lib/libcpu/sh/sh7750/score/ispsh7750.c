/*
 * SH7750 interrupt support.
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 *  Based on work:
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
 *	Modified to reflect isp entries for sh7045 processor:
 *	John M. Mills (jmills@tga.com)
 *	TGA Technologies, Inc.
 *      100 Pinnacle Way, Suite 140
 *	Norcross, GA 30071 U.S.A.
 *	August, 1999
 *
 *	This modified file may be copied and distributed in accordance
 *	the above-referenced license. It is provided for critique and
 *	developmental purposes without any warranty nor representation
 *	by the authors or by TGA Technologies.
 */

#include <rtems/system.h>
#include <rtems/score/types.h>
#include <rtems/score/isr.h>

/*
 * This is a exception vector table
 *
 * It has the same structure as the actual vector table (vectab)
 */


#include <rtems/score/ispsh7750.h>
#include <rtems/score/sh4_regs.h>
#include <rtems/score/sh7750_regs.h>

/* VBR register contents saved on startup -- used to hook exception by debug
 * agent */
void *_VBR_Saved;

#define __STRINGIFY1__(x) #x
#define __STRINGIFY__(x) __STRINGIFY1__(x)

#define STOP_TIMER \
     "        mov.l TSTR_k,r0      \n"                             \
     "        mov.b @r0,r1         \n"                             \
     "        and   #" __STRINGIFY__(~SH7750_TSTR_STR0) ",r1\n"    \
     "        mov.b r1,@r0         \n"

#define START_TIMER \
     "        mov.l TSTR_k,r0      \n"                             \
     "        mov.b @r0,r1         \n"                             \
     "        or    #" __STRINGIFY__(SH7750_TSTR_STR0) ",r1\n"     \
     "        mov.b r1,@r0         \n"

__asm__ ("        .text\n"
     "        .balign 256\n"
     "        .global __vbr_base\n"
     "__vbr_base:\n"
     "        .org __vbr_base + 0x100\n"
     "vbr_100:\n"
     "        mov.l  r0,@-r15\n"
     "        mov.l  r1,@-r15\n"
     "        mov.l  __VBR_Saved100_k, r0\n"
     "        mov.l  offset100_k,r1\n"
     "        mov.l  @r0,r0\n"
     "        add    r1,r0\n"
     "        mov.l  @r15+,r1\n"
     "        jmp    @r0\n"
     "        mov.l  @r15+,r0\n"
     "        .align 2\n"
     "__VBR_Saved100_k:\n"
     "        .long __VBR_Saved\n"
     "offset100_k:\n"
     "        .long  0x100\n"

     "        .org __vbr_base + 0x400\n"
     "vbr_400:\n"
     "        mov.l  r0,@-r15\n"
     "        mov.l  r1,@-r15\n"
     "        mov.l  __VBR_Saved400_k, r0\n"
     "        mov.l  offset400_k,r1\n"
     "        mov.l  @r0,r0\n"
     "        add    r1,r0\n"
     "        mov.l  @r15+,r1\n"
     "        jmp    @r0\n"
     "        mov.l  @r15+,r0\n"
     "        .align 2\n"
     "__VBR_Saved400_k:\n"
     "        .long __VBR_Saved\n"
     "offset400_k:\n"
     "        .long  0x400\n"

     "        .org __vbr_base + 0x600\n"
     "vbr_600:\n"
     "    mov.l r0,@-r15   \n"
     "    mov.l r1,@-r15   \n"
     "    stc   sr,r0      \n"
     "    mov.l __vbr_600_sr_and_k,r1\n"
     "    and   r1,r0      \n"
     "    mov.l __vbr_600_sr_or_k,r1\n"
     "    or    r1,r0      \n"
     "    ldc   r0,sr      \n"
     "    ldc.l @r15+,r1_bank\n"
     "    ldc.l @r15+,r0_bank\n"
     "    mov.l r0,@-r15   \n"
     "    mov.l r1,@-r15   \n"
     "    mov.l r2,@-r15   \n"
     "    mov.l r3,@-r15   \n"
     "    mov.l r4,@-r15   \n"
     "    mov.l r5,@-r15   \n"
     "    mov.l r6,@-r15   \n"
     "    mov.l r7,@-r15   \n"
#if 0
     "    mov.l r8,@-r15   \n"
     "    mov.l r9,@-r15   \n"
     "    mov.l r10,@-r15  \n"
     "    mov.l r11,@-r15  \n"
     "    mov.l r12,@-r15  \n"
     "    mov.l r13,@-r15  \n"
#endif
     "    mov.l r14,@-r15  \n"
     "    sts.l fpscr,@-r15\n"
     "    sts.l fpul,@-r15 \n"
     "    mov.l __ISR_temp_fpscr_k,r0   \n"
     "    lds   r0,fpscr   \n"
     "    fmov  fr0,@-r15  \n"
     "    fmov  fr1,@-r15  \n"
     "    fmov  fr2,@-r15  \n"
     "    fmov  fr3,@-r15  \n"
     "    fmov  fr4,@-r15  \n"
     "    fmov  fr5,@-r15  \n"
     "    fmov  fr6,@-r15  \n"
     "    fmov  fr7,@-r15  \n"
     "    fmov  fr8,@-r15  \n"
     "    fmov  fr9,@-r15  \n"
     "    fmov  fr10,@-r15 \n"
     "    fmov  fr11,@-r15 \n"
     "    fmov  fr12,@-r15 \n"
     "    fmov  fr13,@-r15 \n"
     "    fmov  fr14,@-r15 \n"
     "    fmov  fr15,@-r15 \n"

     "    sts.l pr,@-r15   \n"
     "    sts.l mach,@-r15 \n"
     "    sts.l macl,@-r15 \n"
     "    stc.l spc,@-r15  \n"
     "    stc.l ssr,@-r15  \n"
     "    mov r15,r14      \n"
#if 0
     "    stc   ssr,r0     \n"
     "    ldc   r0,sr      \n"
#endif
     "    mov.l __ISR_Handler_k, r1\n"
     "    mov.l _INTEVT_k,r4\n"
     "    mov.l @r4,r4     \n"
     "    shlr2 r4         \n"
     "    shlr  r4         \n"

     "    mov.l _ISR_Table_k,r0\n"
     "    mov.l @r0,r0     \n"
     "    add   r4,r0      \n"
     "    mov.l @r0,r0     \n"
     "    cmp/eq #0,r0     \n"
     "    bt    _ipl_hook  \n"


     "    jsr @r1          \n"
     "     shlr2 r4        \n"
     "    mov   r14,r15    \n"
     "    ldc.l @r15+,ssr  \n"
     "    ldc.l @r15+,spc  \n"
     "    lds.l @r15+,macl \n"
     "    lds.l @r15+,mach \n"
     "    lds.l @r15+,pr   \n"
     "    mov.l __ISR_temp_fpscr_k,r0   \n"
     "    lds   r0,fpscr   \n"

     "    fmov  @r15+,fr15 \n"
     "    fmov  @r15+,fr14 \n"
     "    fmov  @r15+,fr13 \n"
     "    fmov  @r15+,fr12 \n"
     "    fmov  @r15+,fr11 \n"
     "    fmov  @r15+,fr10 \n"
     "    fmov  @r15+,fr9  \n"
     "    fmov  @r15+,fr8  \n"
     "    fmov  @r15+,fr7  \n"
     "    fmov  @r15+,fr6  \n"
     "    fmov  @r15+,fr5  \n"
     "    fmov  @r15+,fr4  \n"
     "    fmov  @r15+,fr3  \n"
     "    fmov  @r15+,fr2  \n"
     "    fmov  @r15+,fr1  \n"
     "    fmov  @r15+,fr0  \n"
     "    lds.l @r15+,fpul \n"
     "    lds.l @r15+,fpscr\n"
     "    mov.l @r15+,r14  \n"
#if 0
     "    mov.l @r15+,r13  \n"
     "    mov.l @r15+,r12  \n"
     "    mov.l @r15+,r11  \n"
     "    mov.l @r15+,r10  \n"
     "    mov.l @r15+,r9   \n"
     "    mov.l @r15+,r8   \n"
#endif

     "    mov.l @r15+,r7   \n"
     "    mov.l @r15+,r6   \n"
     "    mov.l @r15+,r5   \n"
     "    mov.l @r15+,r4   \n"
     "    mov.l @r15+,r3   \n"
     "    mov.l @r15+,r2   \n"
     "    mov.l @r15+,r1   \n"
     "    mov.l @r15+,r0   \n"
     "    rte              \n"
     "    nop              \n"
     "    .align 2         \n"
     "__vbr_600_sr_and_k:    \n"
     "    .long " __STRINGIFY__(~(SH4_SR_RB | SH4_SR_BL)) "\n"
     "__vbr_600_sr_or_k:     \n"
     "    .long " __STRINGIFY__(SH4_SR_IMASK) "\n"
     "__ISR_Handler_k:     \n"
     "    .long ___ISR_Handler\n"
     "_INTEVT_k:           \n"
     "    .long " __STRINGIFY__(SH7750_INTEVT) "\n"
     "_ISR_Table_k:        \n"
     "    .long   __ISR_Vector_table\n"

     "_ipl_hook:           \n"
     "    mov   r14,r15    \n"
     "    ldc.l @r15+,ssr  \n"
     "    ldc.l @r15+,spc  \n"
     "    lds.l @r15+,macl \n"
     "    lds.l @r15+,mach \n"
     "    lds.l @r15+,pr   \n"
     "    mov.l __ISR_temp_fpscr_k,r0   \n"
     "    lds   r0,fpscr   \n"
     "    fmov  @r15+,fr15 \n"
     "    fmov  @r15+,fr14 \n"
     "    fmov  @r15+,fr13 \n"
     "    fmov  @r15+,fr12 \n"
     "    fmov  @r15+,fr11 \n"
     "    fmov  @r15+,fr10 \n"
     "    fmov  @r15+,fr9  \n"
     "    fmov  @r15+,fr8  \n"
     "    fmov  @r15+,fr7  \n"
     "    fmov  @r15+,fr6  \n"
     "    fmov  @r15+,fr5  \n"
     "    fmov  @r15+,fr4  \n"
     "    fmov  @r15+,fr3  \n"
     "    fmov  @r15+,fr2  \n"
     "    fmov  @r15+,fr1  \n"
     "    fmov  @r15+,fr0  \n"
     "    lds.l @r15+,fpul \n"
     "    lds.l @r15+,fpscr\n"
     "    mov.l @r15+,r14  \n"

     "    mov.l @r15+,r13  \n"
     "    mov.l @r15+,r12  \n"
     "    mov.l @r15+,r11  \n"
     "    mov.l @r15+,r10  \n"
     "    mov.l @r15+,r9   \n"
     "    mov.l @r15+,r8   \n"


     "    mov.l @r15+,r7   \n"
     "    mov.l @r15+,r6   \n"
     "    mov.l @r15+,r5   \n"
     "    mov.l @r15+,r4   \n"
     "    mov.l @r15+,r3   \n"
     "    mov.l @r15+,r2   \n"
     "    mov.l  __VBR_Saved600_k, r0\n"
     "    mov.l  offset600_k,r1\n"
     "    mov.l  @r0,r0\n"
     "    add    r1,r0\n"
     "    mov.l  @r15+,r1\n"
     "    jmp    @r0\n"
     "    mov.l  @r15+,r0\n"
     "    .align 2\n"
     "__ISR_temp_fpscr_k:  \n"
     "    .long  " __STRINGIFY__(SH4_FPSCR_PR) " \n"
     "__VBR_Saved600_k:\n"
     "        .long __VBR_Saved\n"
     "offset600_k:\n"
     "        .long  0x600\n"

 );


/************************************************
 * Dummy interrupt service procedure for
 * interrupts being not allowed --> Trap 2
 ************************************************/
__asm__ (" .section .text\n\
.global __dummy_isp\n\
__dummy_isp:\n\
      mov.l r14,@-r15\n\
      mov   r15, r14\n\
      trapa #2\n\
      mov.l @r15+,r14\n\
      rte\n\
      nop");
