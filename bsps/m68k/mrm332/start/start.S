/**
 *  @file
 *
 *  MRM332 Assembly Start Up Code
 */

/*
 *  COPYRIGHT (c) 2000.
 *  Matt Cross <profesor@gweep.net>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 *
 */

#include "mrm332.h"
#include <rtems/asm.h>
#include <rtems/m68k/sim.h>

BEGIN_CODE

  /* Put the header necessary for the modified CPU32bug to automatically
     start up rtems: */
#if 1
.long 0xbeefbeef ;
#endif
.long 0 ;
.long start ;

.global start
	start:

	oriw   #0x0700,sr 			/* Mask off interupts */

	// Set VBR to CPU32Bug vector table address
	movel	#0x0,d0				/* Use the initial vectors until we get going */
	movecl	d0,vbr

	/* Set stack pointer */
	movel	#_ISR_Stack_area_end,d0
	movel	d0,sp
	movel	d0,a6

  /* include in ram_init.S */
  /*
   * Initalize the SIM module.
   * The stack pointer is not usable until the RAM chip select lines
   * are configured. The following code must remain inline.
   */

  /* Module Configuration Register */
  /*    see section(s) 3.1.3-3.1.6 of the SIM Reference Manual */
  /* SIMCR etc and SAM macro all defined in sim.h found at     */
  /* /cpukit/score/cpu/m68k/rtems/m68k/sim.h				   */
  /* The code below does the following:						   */
  /*	- Sets Freeze Software Enable						   */
  /*	- Turns off Show Cycle Enable						   */
  /* 	- Sets the location of SIM module mapping			   */
  /* 	- Sets the SIM Interrupt Arbitration Field			   */
	lea	SIMCR, a0
	movew	#FRZSW,d0
	oriw	#SAM(0,8,SHEN),d0
	oriw	#(MM*SIM_MM),d0
	oriw	#SAM(SIM_IARB,0,IARB),d0
	movew	d0, a0@

	jsr	start_c /* Jump to the C startup code */

END_CODE

