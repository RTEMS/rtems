/*-------------------------------------*/
/* i960.h                              */
/* Last change :  3.11.94              */
/*-------------------------------------*/
/*
 *  $Id$
 */

#ifndef _I960_H_
#define _I960_H_

#define REGISTER_SIZE	 4
#define NUM_REGS 	40

#define R0_REGNUM   0	/* First local register	*/
#define SP_REGNUM   1	/* Contains address of top of stack */
#define RIP_REGNUM  2	/* Return instruction pointer (local r2) */
#define R15_REGNUM 15	/* Last local register */
#define G0_REGNUM  16	/* First global register */
#define G13_REGNUM 29	/* g13 - holds struct return address */
#define G14_REGNUM 30	/* g14 - ptr to arg block / leafproc return address */
#define FP_REGNUM  31	/* Contains address of executing stack frame */
#define	PCW_REGNUM 32	/* process control word */
#define	ACW_REGNUM 33	/* arithmetic control word */
#define	TCW_REGNUM 34	/* trace control word */
#define IP_REGNUM  35	/* instruction pointer */
#define FP0_REGNUM 36	/* First floating point register */

#define REGISTER_BYTES ((36*4) + (4*10))

#endif
/*-------------*/
/* End of file */
/*-------------*/
