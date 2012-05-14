/**
 * @file rtems/powerpc/powerpc.h
 */

/*
 *  This file contains definitions for the IBM/Motorola PowerPC
 *  family members.
 *
 *  Author:	Andrew Bray <andy@i-cubed.co.uk>
 *
 *  COPYRIGHT (c) 1995 by i-cubed ltd.
 *
 *  MPC860 support code was added by Jay Monkman <jmonkman@frasca.com>
 *  MPC8260 support added by Andy Dachs <a.dachs@sstl.co.uk>
 *  Surrey Satellite Technology Limited
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 *  Derived from c/src/exec/cpu/no_cpu/no_cpu.h:
 *
 *  COPYRIGHT (c) 1989-1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 * Note:
 *      This file is included by both C and assembler code ( -DASM )
 */


#ifndef _RTEMS_POWERPC_POWERPC_H
#define _RTEMS_POWERPC_POWERPC_H

#ifdef __cplusplus
extern "C" {
#endif


/* Till S. 2008/07/10:
 *
 * Using the macros/definitions which depend on a preprocessor
 * symbol defining the CPU flavor is discouraged.
 * I recommend to not use definitions from this file and
 * in particular - not to add more bits and pieces.
 *
 * Instead, try to use run-time detection (see e.g. cpuIdent.c/cpuIdent.h)
 * of features etc.
 */

#include <rtems/score/powerpc.h>

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the PowerPC family.  It does
 *  this by setting variables to indicate which implementation
 *  dependent features are present in a particular member
 *  of the family.
 *
 *  The following architectural feature definitions are defaulted
 *  unless specifically set by the model definition:
 *
 *    + PPC_INTERRUPT_MAX        - 16
 *    + PPC_CACHE_ALIGNMENT      - 32
 *    + PPC_LOW_POWER_MODE       - PPC_LOW_POWER_MODE_NONE
 *    + PPC_HAS_EXCEPTION_PREFIX - 1
 *    + PPC_USE_MULTIPLE         - 0
 */

/*
 *  Define the low power mode models
 *
 *  Standard:   as defined for 603e
 *  Nap Mode:   nap mode only (604)
 *  XXX 403GB, 603, 603e, 604, 821
 */

#define PPC_LOW_POWER_MODE_NONE      0
#define PPC_LOW_POWER_MODE_STANDARD  1

/**
 * @brief For boards with no cache set PPC_CACHE_ALIGNMENT to this value.
 */
#define PPC_NO_CACHE_ALIGNMENT 4

/**
 * @brief Used to define PPC_CACHE_ALIGN_POWER for boards with no cache (and
 * PPC_NO_CACHE_ALIGNMENT defined).
 */
#define PPC_NO_CACHE_ALIGNMENT_POWER 2

/*
 *  Figure out all CPU Model Feature Flags based upon compiler
 *  predefines.
 */

#if defined(ppc403)
/*
 *  IBM 403
 *
 *  Developed for 403GA.  Book checked for 403GB.
 *
 *  Does not have user mode.
 */

#define PPC_CACHE_ALIGNMENT	16
#define PPC_HAS_RI    	        0
#define PPC_HAS_RFCI    	1
#define PPC_USE_MULTIPLE	1
#define PPC_I_CACHE		2048
#define PPC_D_CACHE		1024

#define PPC_HAS_EXCEPTION_PREFIX 0
#define PPC_HAS_EVPR             1

#elif defined (ppc405)

#define PPC_CACHE_ALIGNMENT	32
#define PPC_HAS_RI    	     0
#define PPC_HAS_RFCI    	1
#define PPC_USE_MULTIPLE	1
#define PPC_I_CACHE		16384
#define PPC_D_CACHE		16384		/* except GP/CR */
#define PPC_HAS_EXCEPTION_PREFIX 0
#define PPC_HAS_EVPR             1

#elif defined (ppc440)

#define PPC_CACHE_ALIGNMENT         32
#define PPC_HAS_RI                   0
#define PPC_HAS_RFCI                 1
#define PPC_USE_MULTIPLE             1
#define PPC_I_CACHE              32768
#define PPC_D_CACHE              32768
#define PPC_HAS_EXCEPTION_PREFIX     0
#define PPC_HAS_EVPR                 1

#elif defined(mpc555)

/* Copied from mpc505 */
#define PPC_CACHE_ALIGNMENT	PPC_NO_CACHE_ALIGNMENT

/* Added by querbach@realtime.bc.ca */
#define PPC_LOW_POWER_MODE	PPC_LOW_POWER_MODE_STANDARD

/* Based on comments by Sergei Organov <osv@Javad.RU> */
#define PPC_I_CACHE        	0
#define PPC_D_CACHE        	0

#elif defined(mpc505) || defined(mpc509)
/*
 *  Submitted by Sergei Organov <osv@Javad.RU> as a patch against
 *  3.6.0 long after 4.0 was released.   This is just an attempt
 *  to get the setting correct.
 */

#define CPU_MODEL_NAME  "PowerPC 505/509"

#define PPC_CACHE_ALIGNMENT     PPC_NO_CACHE_ALIGNMENT
#define PPC_I_CACHE             4096
#define PPC_D_CACHE             0


#elif defined(ppc601)

/*
 *  Submitted with original port -- book checked only.
 */

#define PPC_USE_MULTIPLE	1
#define PPC_I_CACHE		0
#define PPC_D_CACHE		32768

#elif defined(ppc603)
/*
 *  Submitted with original port -- book checked only.
 */

#define PPC_I_CACHE		8192
#define PPC_D_CACHE		8192

#elif defined(ppc603e)

/*
 *  Submitted with original port.
 *
 *  Known to work on real hardware.
 */

#define PPC_I_CACHE		16384
#define PPC_D_CACHE		16384

#define PPC_LOW_POWER_MODE PPC_LOW_POWER_MODE_STANDARD

#elif defined(mpc604)
/*
 *  Submitted with original port -- book checked only.
 */

#define PPC_I_CACHE		16384
#define PPC_D_CACHE		16384

#elif defined(mpc860)
/*
 *  Added by Jay Monkman (jmonkman@frasca.com) 6/28/98
 *  with some changes by Darlene Stewart (Darlene.Stewart@iit.nrc.ca)
 */
#define PPC_I_CACHE             4096
#define PPC_D_CACHE             4096
#define PPC_CACHE_ALIGNMENT	16
#define PPC_INTERRUPT_MAX       71
#define PPC_USE_MULTIPLE	1

#define PPC_MSR_0		0x00009000
#define PPC_MSR_1		0x00001000
#define PPC_MSR_2		0x00001000
#define PPC_MSR_3		0x00000000

#elif defined(mpc821)
/*
 *  Added by Andrew Bray <andy@chaos.org.uk> 6/April/1999
 */
#define PPC_I_CACHE             4096
#define PPC_D_CACHE             4096
#define PPC_CACHE_ALIGNMENT	16
#define PPC_INTERRUPT_MAX       71

#define PPC_MSR_0		0x00009000
#define PPC_MSR_1		0x00001000
#define PPC_MSR_2		0x00001000
#define PPC_MSR_3		0x00000000

#elif defined(mpc750)

#define PPC_I_CACHE		16384
#define PPC_D_CACHE		16384

#elif defined(mpc7400)

#define PPC_I_CACHE		32768
#define PPC_D_CACHE		32768

#elif defined(mpc7455)
/*
 *  Added by S.K. Feng <feng1@bnl.gov> 10/03
 */

#define PPC_CACHE_ALIGNMENT     32
#define PPC_I_CACHE		32768
#define PPC_D_CACHE		32768

#elif defined(mpc8260)
/*
 *  Added by Andy Dachs <a.dachs@sstl.co.uk> 23/11/2000
 */
#define PPC_I_CACHE         	16384
#define PPC_D_CACHE         	16384
#define PPC_CACHE_ALIGNMENT	32
#define PPC_INTERRUPT_MAX       125
#define PPC_USE_MULTIPLE	1

#elif defined(__ppc_generic)
#define PPC_CACHE_ALIGNMENT 32

#else

#error "Unsupported CPU Model"

#endif

/*
 *  If the maximum number of exception sources has not been defined,
 *  then default it to 16.
 */

#ifndef PPC_INTERRUPT_MAX
#define PPC_INTERRUPT_MAX	16
#endif

/*
 *  Unless specified otherwise, the cache line size is defaulted to 32.
 *
 *  The derive the power of 2 the cache line is.
 */

#ifndef PPC_CACHE_ALIGNMENT
#define PPC_CACHE_ALIGNMENT 32
#endif

#if (PPC_CACHE_ALIGNMENT == 16)
#define PPC_CACHE_ALIGN_POWER 4
#elif (PPC_CACHE_ALIGNMENT == 32)
#define PPC_CACHE_ALIGN_POWER 5
#elif (PPC_CACHE_ALIGNMENT == PPC_NO_CACHE_ALIGNMENT)
#define PPC_CACHE_ALIGN_POWER PPC_NO_CACHE_ALIGNMENT_POWER
#else
#error "Undefined power of 2 for PPC_CACHE_ALIGNMENT"
#endif

/*
 *  Unless otherwise specified, assume the model has an IP/EP bit to
 *  set the exception address prefix.
 */

#ifndef PPC_HAS_EXCEPTION_PREFIX
#define PPC_HAS_EXCEPTION_PREFIX 1
#endif
/*
 *  Unless otherwise specified, assume the model has an RI bit to
 *  identify non-recoverable interrupts
 */

#ifndef PPC_HAS_RI
#define PPC_HAS_RI 1
#endif

/*
 *  Unless otherwise specified, assume the model does NOT have
 *  403 style EVPR register to set the exception address prefix.
 */

#ifndef PPC_HAS_EVPR
#define PPC_HAS_EVPR 0
#endif

/*
 *  If no low power mode model was specified, then assume there is none.
 */

#ifndef PPC_LOW_POWER_MODE
#define PPC_LOW_POWER_MODE PPC_LOW_POWER_MODE_NONE
#endif

/*
 *  Unless specified above, then assume the model does NOT have critical
 *  interrupt support.
 */

#ifndef PPC_HAS_RFCI
#define PPC_HAS_RFCI 0
#endif

/*
 *  Unless specified above, do not use the load/store multiple instructions
 *  in a context switch.
 */

#ifndef PPC_USE_MULTIPLE
#define PPC_USE_MULTIPLE 0
#endif

/*
 *  The following exceptions are not maskable, and are not
 *  necessarily predictable, so cannot be offered to RTEMS:
 *    Alignment exception - handled by the CPU module
 *    Data exceptions.
 *    Instruction exceptions.
 */

/*
 *  Base Interrupt vectors supported on all models.
 */
#define PPC_IRQ_SYSTEM_RESET	 0 /* 0x00100 - System reset.              */
#define PPC_IRQ_MCHECK		 1 /* 0x00200 - Machine check              */
#define PPC_IRQ_PROTECT		 2 /* 0x00300 - Protection violation       */
#define PPC_IRQ_ISI              3 /* 0x00400 - Instruction Fetch error    */
#define PPC_IRQ_EXTERNAL	 4 /* 0x00500 - External interrupt         */
#define PPC_IRQ_ALIGNMENT        5 /* 0X00600 - Alignment exception        */
#define PPC_IRQ_PROGRAM		 6 /* 0x00700 - Program exception          */
#define PPC_IRQ_NOFP		 7 /* 0x00800 - Floating point unavailable */
#define PPC_IRQ_DECREMENTER	 8 /* 0x00900 - Decrementer interrupt      */
#define PPC_IRQ_RESERVED_A       9 /* 0x00a00 - Implementation Reserved    */
#define PPC_IRQ_RESERVED_B      10 /* 0x00b00 - Implementation Reserved    */
#define PPC_IRQ_SCALL		11 /* 0x00c00 - System call                */
#define PPC_IRQ_TRACE           12 /* 0x00d00 - Trace Exception            */
#define PPC_IRQ_FP_ASST         13 /* ox00e00 - Floating point assist      */
#define PPC_STD_IRQ_LAST        PPC_IRQ_FP_ASST

#define PPC_IRQ_FIRST           PPC_IRQ_SYSTEM_RESET

#if defined(ppc403) || defined(ppc405) || defined(ppc440)

#define PPC_IRQ_CRIT     PPC_IRQ_SYSTEM_RESET /*0x00100- Critical int. pin */
#define PPC_IRQ_PIT      (PPC_STD_IRQ_LAST+1) /*0x01000- Pgm interval timer*/
#define PPC_IRQ_FIT	 (PPC_STD_IRQ_LAST+2) /*0x01010- Fixed int. timer  */
#define PPC_IRQ_WATCHDOG (PPC_STD_IRQ_LAST+3) /*0x01020- Watchdog timer    */
#define PPC_IRQ_DEBUG	 (PPC_STD_IRQ_LAST+4) /*0x02000- Debug exceptions  */
#define PPC_IRQ_LAST     PPC_IRQ_DEBUG

#elif defined(mpc505) || defined(mpc509)
#define PPC_IRQ_SOFTEMU   (PPC_STD_IRQ_LAST+1)    /* Software emulation. */
#define PPC_IRQ_DATA_BP   (PPC_STD_IRQ_LAST+ 2)
#define PPC_IRQ_INST_BP   (PPC_STD_IRQ_LAST+ 3)
#define PPC_IRQ_MEXT_BP   (PPC_STD_IRQ_LAST+ 4)
#define PPC_IRQ_NMEXT_BP  (PPC_STD_IRQ_LAST+ 5)

#elif defined(mpc555)
#define PPC_IRQ_SOFTEMU   (PPC_STD_IRQ_LAST+1)  /* Software emulation. */
#define PPC_IRQ_INST_PE   (PPC_STD_IRQ_LAST+2)  /* Insn protection error */
#define PPC_IRQ_DATA_PE   (PPC_STD_IRQ_LAST+3)  /* Data protection error */
#define PPC_IRQ_DATA_BP   (PPC_STD_IRQ_LAST+4)  /* Data breakpoint */
#define PPC_IRQ_INST_BP   (PPC_STD_IRQ_LAST+5)  /* Insn breakpoint */
#define PPC_IRQ_MEXT_BP   (PPC_STD_IRQ_LAST+6)  /* Maskable ext bkpt */
#define PPC_IRQ_NMEXT_BP  (PPC_STD_IRQ_LAST+7)  /* Non-maskable ext bkpt */
#define PPC_IRQ_LAST      PPC_IRQ_NMEXT_BP

#elif defined(ppc601)
#define PPC_IRQ_TRACE    (PPC_STD_IRQ_LAST+1) /*0x02000-Run/Trace Exception*/
#define PPC_IRQ_LAST     PPC_IRQ_TRACE

#elif defined(ppc602)
#define PPC_IRQ_LAST     (PPC_STD_IRQ_LAST)

#elif defined(ppc603) || defined(ppc603e)
#define PPC_IRQ_TRANS_MISS (PPC_STD_IRQ_LAST+1) /*0x1000-Ins Translation Miss*/
#define PPC_IRQ_DATA_LOAD  (PPC_STD_IRQ_LAST+2) /*0x1100-Data Load Trans Miss*/
#define PPC_IRQ_DATA_STORE (PPC_STD_IRQ_LAST+3) /*0x1200-Data Store Miss     */
#define PPC_IRQ_ADDR_BRK   (PPC_STD_IRQ_LAST+4) /*0x1300-Instruction Bkpoint */
#define PPC_IRQ_SYS_MGT    (PPC_STD_IRQ_LAST+5) /*0x1400-System Management   */
#define PPC_IRQ_LAST       PPC_IRQ_SYS_MGT

#elif defined(mpc604)
#define PPC_IRQ_ADDR_BRK (PPC_STD_IRQ_LAST+1) /*0x1300- Inst. addr break  */
#define PPC_IRQ_SYS_MGT  (PPC_STD_IRQ_LAST+2) /*0x1400- System Management */
#define PPC_IRQ_LAST     PPC_IRQ_SYS_MGT

#elif defined(mpc860) || defined(mpc821)
#define PPC_IRQ_EMULATE         (PPC_STD_IRQ_LAST+1) /*0x1000-Software emulation  */
#define PPC_IRQ_INST_MISS       (PPC_STD_IRQ_LAST+2) /*0x1100-Instruction TLB miss*/
#define PPC_IRQ_DATA_MISS       (PPC_STD_IRQ_LAST+3) /*0x1200-Data TLB miss */
#define PPC_IRQ_INST_ERR        (PPC_STD_IRQ_LAST+4) /*0x1300-Instruction TLB err */
#define PPC_IRQ_DATA_ERR        (PPC_STD_IRQ_LAST+5) /*0x1400-Data TLB error */
#define PPC_IRQ_DATA_BPNT       (PPC_STD_IRQ_LAST+6) /*0x1C00-Data breakpoint */
#define PPC_IRQ_INST_BPNT       (PPC_STD_IRQ_LAST+7) /*0x1D00-Inst breakpoint */
#define PPC_IRQ_IO_BPNT         (PPC_STD_IRQ_LAST+8) /*0x1E00-Peripheral breakpnt */
#define PPC_IRQ_DEV_PORT        (PPC_STD_IRQ_LAST+9) /*0x1F00-Development port */
#define PPC_IRQ_IRQ0            (PPC_STD_IRQ_LAST + 10)
#define PPC_IRQ_LVL0            (PPC_STD_IRQ_LAST + 11)
#define PPC_IRQ_IRQ1            (PPC_STD_IRQ_LAST + 12)
#define PPC_IRQ_LVL1            (PPC_STD_IRQ_LAST + 13)
#define PPC_IRQ_IRQ2		(PPC_STD_IRQ_LAST + 14)
#define PPC_IRQ_LVL2		(PPC_STD_IRQ_LAST + 15)
#define PPC_IRQ_IRQ3		(PPC_STD_IRQ_LAST + 16)
#define PPC_IRQ_LVL3		(PPC_STD_IRQ_LAST + 17)
#define PPC_IRQ_IRQ4		(PPC_STD_IRQ_LAST + 18)
#define PPC_IRQ_LVL4		(PPC_STD_IRQ_LAST + 19)
#define PPC_IRQ_IRQ5		(PPC_STD_IRQ_LAST + 20)
#define PPC_IRQ_LVL5		(PPC_STD_IRQ_LAST + 21)
#define PPC_IRQ_IRQ6		(PPC_STD_IRQ_LAST + 22)
#define PPC_IRQ_LVL6		(PPC_STD_IRQ_LAST + 23)
#define PPC_IRQ_IRQ7		(PPC_STD_IRQ_LAST + 24)
#define PPC_IRQ_LVL7		(PPC_STD_IRQ_LAST + 25)
#define PPC_IRQ_CPM_ERROR	(PPC_STD_IRQ_LAST + 26)
#define PPC_IRQ_CPM_PC4		(PPC_STD_IRQ_LAST + 27)
#define PPC_IRQ_CPM_PC5		(PPC_STD_IRQ_LAST + 28)
#define PPC_IRQ_CPM_SMC2	(PPC_STD_IRQ_LAST + 29)
#define PPC_IRQ_CPM_SMC1	(PPC_STD_IRQ_LAST + 30)
#define PPC_IRQ_CPM_SPI		(PPC_STD_IRQ_LAST + 31)
#define PPC_IRQ_CPM_PC6		(PPC_STD_IRQ_LAST + 32)
#define PPC_IRQ_CPM_TIMER4	(PPC_STD_IRQ_LAST + 33)
#define PPC_IRQ_CPM_RESERVED_8	(PPC_STD_IRQ_LAST + 34)
#define PPC_IRQ_CPM_PC7		(PPC_STD_IRQ_LAST + 35)
#define PPC_IRQ_CPM_PC8		(PPC_STD_IRQ_LAST + 36)
#define PPC_IRQ_CPM_PC9		(PPC_STD_IRQ_LAST + 37)
#define PPC_IRQ_CPM_TIMER3	(PPC_STD_IRQ_LAST + 38)
#define PPC_IRQ_CPM_RESERVED_D	(PPC_STD_IRQ_LAST + 39)
#define PPC_IRQ_CPM_PC10	(PPC_STD_IRQ_LAST + 40)
#define PPC_IRQ_CPM_PC11	(PPC_STD_IRQ_LAST + 41)
#define PPC_IRQ_CPM_I2C		(PPC_STD_IRQ_LAST + 42)
#define PPC_IRQ_CPM_RISC_TIMER	(PPC_STD_IRQ_LAST + 43)
#define PPC_IRQ_CPM_TIMER2	(PPC_STD_IRQ_LAST + 44)
#define PPC_IRQ_CPM_RESERVED_13	(PPC_STD_IRQ_LAST + 45)
#define PPC_IRQ_CPM_IDMA2	(PPC_STD_IRQ_LAST + 46)
#define PPC_IRQ_CPM_IDMA1	(PPC_STD_IRQ_LAST + 47)
#define PPC_IRQ_CPM_SDMA_ERROR	(PPC_STD_IRQ_LAST + 48)
#define PPC_IRQ_CPM_PC12	(PPC_STD_IRQ_LAST + 49)
#define PPC_IRQ_CPM_PC13	(PPC_STD_IRQ_LAST + 50)
#define PPC_IRQ_CPM_TIMER1	(PPC_STD_IRQ_LAST + 51)
#define PPC_IRQ_CPM_PC14	(PPC_STD_IRQ_LAST + 52)
#define PPC_IRQ_CPM_SCC4	(PPC_STD_IRQ_LAST + 53)
#define PPC_IRQ_CPM_SCC3	(PPC_STD_IRQ_LAST + 54)
#define PPC_IRQ_CPM_SCC2	(PPC_STD_IRQ_LAST + 55)
#define PPC_IRQ_CPM_SCC1	(PPC_STD_IRQ_LAST + 56)
#define PPC_IRQ_CPM_PC15	(PPC_STD_IRQ_LAST + 57)

#define PPC_IRQ_LAST             PPC_IRQ_CPM_PC15

#elif defined(mpc8260)

#define PPC_IRQ_INST_MISS       (PPC_STD_IRQ_LAST+1) /*0x1000-Instruction TLB miss*/
#define PPC_IRQ_DATA_MISS       (PPC_STD_IRQ_LAST+2) /*0x1100-Data TLB miss */
#define PPC_IRQ_DATA_L_MISS  	(PPC_STD_IRQ_LAST+3) /*0x1200-Data TLB load miss */
#define PPC_IRQ_DATA_S_MISS  	(PPC_STD_IRQ_LAST+4) /*0x1300-Data TLB store miss */
#define PPC_IRQ_INST_BPNT       (PPC_STD_IRQ_LAST+5) /*0x1400-Inst address breakpoint */
#define PPC_IRQ_SYS_MGT         (PPC_STD_IRQ_LAST+6) /*0x1500-System Management */
/* 0x1600 - 0x2F00 reserved */
#define PPC_IRQ_CPM_NONE	(PPC_STD_IRQ_LAST + 50)
#define PPC_IRQ_CPM_I2C		(PPC_STD_IRQ_LAST + 51)
#define PPC_IRQ_CPM_SPI		(PPC_STD_IRQ_LAST + 52)
#define PPC_IRQ_CPM_RISC_TIMER	(PPC_STD_IRQ_LAST + 53)
#define PPC_IRQ_CPM_SMC1	(PPC_STD_IRQ_LAST + 54)
#define PPC_IRQ_CPM_SMC2	(PPC_STD_IRQ_LAST + 55)
#define PPC_IRQ_CPM_IDMA1	(PPC_STD_IRQ_LAST + 56)
#define PPC_IRQ_CPM_IDMA2	(PPC_STD_IRQ_LAST + 57)
#define PPC_IRQ_CPM_IDMA3	(PPC_STD_IRQ_LAST + 58)
#define PPC_IRQ_CPM_IDMA4	(PPC_STD_IRQ_LAST + 59)
#define PPC_IRQ_CPM_SDMA	(PPC_STD_IRQ_LAST + 60)
#define PPC_IRQ_CPM_RES_A	(PPC_STD_IRQ_LAST + 61)
#define PPC_IRQ_CPM_TIMER1	(PPC_STD_IRQ_LAST + 62)
#define PPC_IRQ_CPM_TIMER2	(PPC_STD_IRQ_LAST + 63)
#define PPC_IRQ_CPM_TIMER3	(PPC_STD_IRQ_LAST + 64)
#define PPC_IRQ_CPM_TIMER4	(PPC_STD_IRQ_LAST + 65)
#define PPC_IRQ_CPM_TMCNT	(PPC_STD_IRQ_LAST + 66)
#define PPC_IRQ_CPM_PIT		(PPC_STD_IRQ_LAST + 67)
#define PPC_IRQ_CPM_RES_B	(PPC_STD_IRQ_LAST + 68)
#define PPC_IRQ_CPM_IRQ1	(PPC_STD_IRQ_LAST + 69)
#define PPC_IRQ_CPM_IRQ2	(PPC_STD_IRQ_LAST + 70)
#define PPC_IRQ_CPM_IRQ3	(PPC_STD_IRQ_LAST + 71)
#define PPC_IRQ_CPM_IRQ4	(PPC_STD_IRQ_LAST + 72)
#define PPC_IRQ_CPM_IRQ5	(PPC_STD_IRQ_LAST + 73)
#define PPC_IRQ_CPM_IRQ6	(PPC_STD_IRQ_LAST + 74)
#define PPC_IRQ_CPM_IRQ7	(PPC_STD_IRQ_LAST + 75)
#define PPC_IRQ_CPM_RES_C	(PPC_STD_IRQ_LAST + 76)
#define PPC_IRQ_CPM_RES_D	(PPC_STD_IRQ_LAST + 77)
#define PPC_IRQ_CPM_RES_E	(PPC_STD_IRQ_LAST + 78)
#define PPC_IRQ_CPM_RES_F	(PPC_STD_IRQ_LAST + 79)
#define PPC_IRQ_CPM_RES_G	(PPC_STD_IRQ_LAST + 80)
#define PPC_IRQ_CPM_RES_H	(PPC_STD_IRQ_LAST + 81)
#define PPC_IRQ_CPM_FCC1	(PPC_STD_IRQ_LAST + 82)
#define PPC_IRQ_CPM_FCC2	(PPC_STD_IRQ_LAST + 83)
#define PPC_IRQ_CPM_FCC3	(PPC_STD_IRQ_LAST + 84)
#define PPC_IRQ_CPM_RES_I	(PPC_STD_IRQ_LAST + 85)
#define PPC_IRQ_CPM_MCC1	(PPC_STD_IRQ_LAST + 86)
#define PPC_IRQ_CPM_MCC2	(PPC_STD_IRQ_LAST + 87)
#define PPC_IRQ_CPM_RES_J	(PPC_STD_IRQ_LAST + 88)
#define PPC_IRQ_CPM_RES_K	(PPC_STD_IRQ_LAST + 89)
#define PPC_IRQ_CPM_SCC1	(PPC_STD_IRQ_LAST + 90)
#define PPC_IRQ_CPM_SCC2	(PPC_STD_IRQ_LAST + 91)
#define PPC_IRQ_CPM_SCC3	(PPC_STD_IRQ_LAST + 92)
#define PPC_IRQ_CPM_SCC4	(PPC_STD_IRQ_LAST + 93)
#define PPC_IRQ_CPM_RES_L	(PPC_STD_IRQ_LAST + 94)
#define PPC_IRQ_CPM_RES_M	(PPC_STD_IRQ_LAST + 95)
#define PPC_IRQ_CPM_RES_N	(PPC_STD_IRQ_LAST + 96)
#define PPC_IRQ_CPM_RES_O	(PPC_STD_IRQ_LAST + 97)
#define PPC_IRQ_CPM_PC15	(PPC_STD_IRQ_LAST + 98)
#define PPC_IRQ_CPM_PC14	(PPC_STD_IRQ_LAST + 99)
#define PPC_IRQ_CPM_PC13	(PPC_STD_IRQ_LAST + 100)
#define PPC_IRQ_CPM_PC12	(PPC_STD_IRQ_LAST + 101)
#define PPC_IRQ_CPM_PC11	(PPC_STD_IRQ_LAST + 102)
#define PPC_IRQ_CPM_PC10	(PPC_STD_IRQ_LAST + 103)
#define PPC_IRQ_CPM_PC9		(PPC_STD_IRQ_LAST + 104)
#define PPC_IRQ_CPM_PC8		(PPC_STD_IRQ_LAST + 105)
#define PPC_IRQ_CPM_PC7		(PPC_STD_IRQ_LAST + 106)
#define PPC_IRQ_CPM_PC6		(PPC_STD_IRQ_LAST + 107)
#define PPC_IRQ_CPM_PC5		(PPC_STD_IRQ_LAST + 108)
#define PPC_IRQ_CPM_PC4		(PPC_STD_IRQ_LAST + 109)
#define PPC_IRQ_CPM_PC3		(PPC_STD_IRQ_LAST + 110)
#define PPC_IRQ_CPM_PC2		(PPC_STD_IRQ_LAST + 111)
#define PPC_IRQ_CPM_PC1		(PPC_STD_IRQ_LAST + 112)
#define PPC_IRQ_CPM_PC0		(PPC_STD_IRQ_LAST + 113)

#define PPC_IRQ_LAST             PPC_IRQ_CPM_PC0

#endif


/*
 *  If the maximum number of exception sources is too low,
 *  then fix it
 */

#if PPC_INTERRUPT_MAX <= PPC_IRQ_LAST
#undef PPC_INTERRUPT_MAX
#define PPC_INTERRUPT_MAX ((PPC_IRQ_LAST) + 1)
#endif

/*
 *  Machine Status Register (MSR) Constants Used by RTEMS
 */

#if PPC_HAS_RI
#define PPC_MSR_RI       0x000000002 /* bit 30 - recoverable exception */
#endif

#define PPC_MSR_DR       0x000000010 /* bit 27 - data address translation */
#define PPC_MSR_IR       0x000000020 /* bit 26 - instruction addr translation*/

/*
 *  Some PPC model manuals refer to the Exception Prefix (EP) bit as
 *  IP for no apparent reason.
 */
#if (PPC_HAS_EXCEPTION_PREFIX)
#define PPC_MSR_EP       0x000000040 /* bit 25 - exception prefix */
#else
#define PPC_MSR_EP       0x000000000 /* bit 25 - exception prefix */
#endif

#if (PPC_HAS_FPU)
#define PPC_MSR_FP       0x000002000 /* bit 18 - floating point enable */
#else
#define PPC_MSR_FP       0x000000000 /* bit 18 - floating point enable */
#endif

#if (PPC_LOW_POWER_MODE == PPC_LOW_POWER_MODE_NONE)
#define PPC_MSR_POW      0x000000000 /* bit 13 - power management enable */
#else
#define PPC_MSR_POW      0x000040000 /* bit 13 - power management enable */
#endif

#define PPC_MSR_ME       0x000001000 /* bit 19 - machine check enable */
#define PPC_MSR_EE       0x000008000 /* bit 16 - external interrupt enable */

#if (PPC_HAS_RFCI)
#define PPC_MSR_CE       0x000020000 /* bit 14 - critical interrupt enable */
#else
#define PPC_MSR_CE       0x000000000 /* bit 14 - critical interrupt enable */
#endif

#define PPC_MSR_DISABLE_MASK (PPC_MSR_ME|PPC_MSR_EE|PPC_MSR_CE)

#define PPC_MINIMUM_STACK_FRAME_SIZE PPC_STACK_ALIGNMENT

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_POWERPC_POWERPC_H */
