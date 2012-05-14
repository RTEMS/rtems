/*
 * start.S -- Initialization code for SH7750 generic BSP
 *
 * Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * Based on work:
 *  Authors: Ralf Corsepius (corsepiu@faw.uni-ulm.de) and
 *           Bernd Becker (becker@faw.uni-ulm.de)
 *
 *  COPYRIGHT (c) 1997-1998, FAW Ulm, Germany
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  Modified to reflect Hitachi EDK SH7045F:
 *  John M. Mills (jmills@tga.com)
 *  TGA Technologies, Inc.
 *  100 Pinnacle Way, Suite 140
 *  Norcross, GA 30071 U.S.A.
 *
 *
 *  This modified file may be copied and distributed in accordance
 *  the above-referenced license. It is provided for critique and
 *  developmental purposes without any warranty nor representation
 *  by the authors or by TGA Technologies.
 *
 *  COPYRIGHT (c) 1999-2001.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>
#include "rtems/score/sh4_regs.h"
#include "rtems/score/sh7750_regs.h"

        BEGIN_CODE
        PUBLIC(start)

/*
 * Algorithm of the first part of the start():
 *
 * 1. Initialize stack
 * 2. Are we from reset or from gdb? Set value for boot_mode in r9.
 * 3. Initialize hardware if we are from reset. Cache is off.
 * 4. Copy data from flash to ram; set up boot mode and jump to real address.
 * 5. Zero out bss.
 * 6. Turn memory cach on.
 */

SYM (start):
        ! install the stack pointer
        mov.l	stack_k,r15

        mov.l   initial_sr_k,r0
        ldc     r0,ssr
        ldc     r0,sr

        ! let us see if we are from gdb stub or from power-on reset
        bsr     fake_func
        nop
fake_func:

        sts     pr, r0
        shlr8   r0
        mov.l   reset_pc_value_shift_8_k, r1
        cmp/eq  r0, r1
        movt    r9      ! r9 == ! boot_mode
        neg     r9, r9
        add     #1, r9  ! r9 == boot_mode

        ! what is in boot_mode?
        cmp/pl  r9      ! r9 > 0  ->  T = 1

        ! if boot_mode != SH4_BOOT_MODE_FLASH
        bt      hw_init_end
        nop

#if defined(START_HW_INIT)	/* from $RTEMS_BSP.cfg */
        ! Initialize minimal hardware
        ! to run hw_init we need to calculate its address
        ! as it is before data coping
        mov.l	hw_init_k, r0
        mov.l   copy_start_k, r1
        mov.l   copy_end_k, r2
        cmp/ge  r0, r1
        bt      0f
        cmp/ge  r0, r2
        bf      0f
        ! if  copy_start <= hw_init <= copy_end  then
        neg     r1, r1
        mov.l   copy_start_in_rom_k, r3
        add     r1,r0
        add     r3, r0
0:
        jsr @r0
        nop		!delay slot
#endif /* START_HW_INIT */
hw_init_end:

        ! copy data from rom to ram
        mov.l   copy_start_k, r0
        mov.l   copy_end_k, r1
        mov.l   copy_start_in_rom_k, r2

        ! if copy_from == copy_to do not copy anything
        cmp/eq  r0, r2
        bt      real_address
        nop

copy_data_cycle:
        cmp/ge  r1, r0
        bt      end_of_copy_data_cycle
        nop
        mov.l   @r2+, r3
        mov.l   r3, @r0
        add     #4, r0
        bra     copy_data_cycle
        nop

end_of_copy_data_cycle:
        ! go to 0x8....... adresses
        mov.l   real_address_k, r0
        lds     r0, pr
        rts
        nop
real_address:
        ! write boot_mode to ram
        mov.l   boot_mode_k, r5
        mov.l   r9, @r5

zero_bss:
        ! zero out bss
        mov.l	__bss_start_k,r0
        mov.l	__bss_end_k,r1
        mov	#0,r2
0:
        mov.l	r2,@r0
        add	#4,r0
        cmp/ge	r0,r1
        bt	0b
        nop

        ! Turn cache on
        mov.l   cache_on_k, r0
        jsr @r0
        nop     !delay slot

        ! Save old value of VBR register. We will need it to allow
        ! debugger agent hook exceptions.
        mov.l   __VBR_Saved_k,r0
        stc     vbr,r5
        mov.l   r5,@r0
        ! Set up VBR register
        mov.l   _vbr_base_k,r0
        ldc     r0,vbr

        ! initialise fpscr for gcc
        mov.l set_fpscr_k, r1
        jsr @r1
        nop

        ! Set FPSCR register
        mov.l   initial_fpscr_k,r0
        lds     r0,fpscr

        ! call the mainline
        mov #0,r4		! argc
        mov.l main_k,r0
        jsr @r0
        nop

        ! call exit
        mov	r0,r4
        mov.l	exit_k,r0
        jsr	@r0
        or	r0,r0

        .global _stop
_stop:
        mov     #11,r0
        mov     #0,r4
        trapa   #0x3f
        nop
__stop:
        bra     __stop
        nop

        END_CODE

        .align 2
copy_start_k:
        .long copy_start
copy_end_k:
        .long copy_end
copy_start_in_rom_k:
        .long copy_start_in_rom

real_address_k:
        .long real_address
set_fpscr_k:
    .long   ___set_fpscr
_vbr_base_k:
        .long   SYM(_vbr_base)
__VBR_Saved_k:
        .long   SYM(_VBR_Saved)
stack_k:
        .long	SYM(stack)
__bss_start_k:
        .long   __bss_start
__bss_end_k:
        .LONG   __bss_end
main_k:
        .long	SYM(boot_card)
exit_k:
        .long	SYM(_exit)

#ifdef	START_HW_INIT	/* from $RTEMS_BSP.cfg */
hw_init_k:
        .long	SYM(early_hw_init)
#endif /* START_HW_INIT */

cache_on_k:
        .long   SYM(bsp_cache_on)

vects_k:
        .long	SYM(vectab)
vects_size:
        .word	255

    .align 2
initial_sr_k:
        .long   SH4_SR_MD | SH4_SR_IMASK
initial_fpscr_k:
#ifdef __SH4__
        .long   SH4_FPSCR_DN | SH4_FPSCR_PR | SH4_FPSCR_RM
#else
        .long   SH4_FPSCR_DN | SH4_FPSCR_RM
#endif

reset_pc_value_shift_8_k:
        .long   0xa00000

boot_mode_k:
        .long   _boot_mode

#ifdef __ELF__
        .section .stack,"aw"
#else
        .section .stack
#endif
SYM(stack):
        .long	0xdeaddead

#ifdef __ELF__
        .section .bss,"aw"
#else
        .section .bss
#endif

        .global __sh4sim_dummy_register
__sh4sim_dummy_register:
        .long	0

        .section    .data
        .global _boot_mode
_boot_mode:
        .long   0
