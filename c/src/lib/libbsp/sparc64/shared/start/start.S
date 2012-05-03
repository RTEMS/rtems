#
# Copyright (c) 2006 Martin Decky
# Copyright (c) 2006 Jakub Jermar
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# - Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# - Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# - The name of the author may not be used to endorse or promote products
#   derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

/*
 * start.S
 *
 * Start code for Sparc64 RTEMS
 *
 * This is based on the boot.S file of the HelenOS sparc64 architecture.
 * http://www.helenos.org/
 * ${helenos}/boot/arch/sparc64/loader/boot.S
 *
 * Modifications for the RTEMS executive are 
 *   COPYRIGHT (c) 2010 Gedare Bloom.
 *   COPYRIGHT (c) 2010 Eugen Leontie.
 */

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

#define PSTATE_PRIV_BIT	4

.register %g2, #scratch
.register %g3, #scratch
.register %g6, #scratch
.register %g7, #scratch

.section BOOTSTRAP, "ax"

PUBLIC(_start)
  .global _start
  SYM(_start):
    start:
    b 1f
    nop

    /*
     * This header forces SILO to load the image at 0x4000.
     * More precisely, SILO will think this is an old version of Linux.
     */
    .ascii "HdrS"
    .word 0
    .half 0
    .half 0
    .half 0
    .half 0
    .global silo_ramdisk_image
    silo_ramdisk_image:
    .word 0
    .global silo_ramdisk_size
    silo_ramdisk_size:
    .word 0

    .align 8
    1:	
    /*
     * Disable interrupts and disable address masking.
     */
    wrpr %g0, PSTATE_PRIV_BIT, %pstate

    wrpr %g0, SPARC_NUMBER_OF_REGISTER_WINDOWS - 2, %cansave	! set maximum saveable windows
    wrpr %g0, 0, %canrestore		! get rid of windows we will never need again
    wrpr %g0, 0, %otherwin			! make sure the window state is consistent
    wrpr %g0, SPARC_NUMBER_OF_REGISTER_WINDOWS - 1, %cleanwin	! prevent needless clean_window traps for kernel

  /* g4 needs to be cleared for gcc */
  mov %g0, %g4

  /* Clear the bss */
	setx	SYM(bss_start), %l0, %g2	! g2 = start of bss
	setx	SYM(_end), %l0, %g3		    ! g3 = end of bss
zerobss:
	stx	%g0, [%g2]
	add	%g2, 8, %g2
	cmp	%g2, %g3
	bleu,a	zerobss
	nop

  /* Install stack */
setstack:
	set SYM(StackStart), %sp
	add %sp, -STACK_BIAS, %sp

  /* BSP-specific pre-bootcard initializations */
  call SYM(_BSP_init)
  nop

	setx ofw_cif, %l0, %l1
 
	call ofw_init		! initialize OpenFirmware
	stx %o4, [%l1]

  call bootstrap
  nop


  /* Set up ISR handler for interrupt enable/disable */
	setx SYM(syscall), %l0, %o1
	setx param_space, %l0, %o2
	call SYM(_CPU_ISR_install_raw_handler)
	mov 0x100, %o0 

  /* Don't need to copy initialized data to RAM--link puts all in RAM already */

	mov	%g0, %o0		! clear command line passed to main
	call SYM(boot_card)
	sub	%sp, 0x60, %sp		! make room on stack (necessary?)

  call SYM(halt)
  nop

halted:	nop
	    b SYM(halted)

.section BOOTSTRAP
  .align CPU_ALIGNMENT
  .space 32768 - CPU_MINIMUM_STACK_FRAME_SIZE ! 32K boot stack grows down
  .global StackStart
  StackStart:
    .space CPU_MINIMUM_STACK_FRAME_SIZE ! initial stack frame

.section BOOTSTRAP
  .align CPU_ALIGNMENT
  param_space:
  	.space 8

.section .fini
  .global _fini
  _fini:
    nop
    retl
    nop

.section .init
  .global _init
  _init:
    nop
    retl
    nop
