/*
 *  BSP registers declaration
 *
 * Copyright (c) 2000 Canon Research France SA.
 * Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 * The license and distribution terms for this file may be
 * found in found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 *
 */


#ifndef  __REGS_H__
#define __REGS_H__  

/*
 * VARIABLE DECLARATION   
 */

#ifndef __asm__
extern volatile unsigned long *Regs;        /* Chip registers        */
#endif



/*
 * Here must be "defined" each register, to use with Regs as
 * LM_Regs[REGISTER1] = value
 */

#define	REGISTER1      1
#define REGISTER2      2


/*
 * define for UART registers to be able
 * to compile and link arm_bare_bsp
 */

#define RSRBR   0
#define RSTHR	1
#define RSIER	2
#define RSIIR	3
#define RSFCR	4
#define RSLCR	5
#define RSLSR	6
#define RSDLL	7
#define RSDLH	8
#define RSCNT   9

#endif /*__REGS_H__*/

