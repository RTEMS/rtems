/* bsp_irq_asm.S
 *
 *  This file contains the implementation of the IRQ handler
 *  for a specific BSP
 *
 *  CopyRight (C) 2000 Canon Research France SA.
 *  Emmanuel Raguet,  mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define __asm__
#include  <registers.h>
		
/* 
 * Function to obtain, execute an IT handler and acknowledge the IT 
 */

	.globl ExecuteITHandler
	
ExecuteITHandler :	

  /*
   * Here is the code to execute the appropriate INT handler 
   */
	
	mov pc, r0

#if 0	
/* 
 * Function to acknowledge the IT controller 
 */
	.globl AckControler
	
AckControler:	  

  /*
   * Here is the code to acknowledge the PIC
   */
	
  b	ReturnFromAck		/* return to ISR handler */

#endif
