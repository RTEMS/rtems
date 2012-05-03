/**
 * @file rtems/asm.h
 *
 *  This include file attempts to address the problems
 *  caused by incompatible flavors of assemblers and
 *  toolsets.  It primarily addresses variations in the
 *  use of leading underscores on symbols and the requirement
 *  that register names be preceded by a %.
 */

/*
 *  NOTE: The spacing in the use of these macros
 *        is critical to them working as advertised.
 *
 *  COPYRIGHT:
 *
 *  This file is based on similar code found in newlib available
 *  from ftp.cygnus.com.  The file which was used had no copyright
 *  notice.  This file is freely distributable as long as the source
 *  of the file is noted.  This file is:
 *
 *  COPYRIGHT (c) 1994-1997.
 *  On-Line Applications Research Corporation (OAR).
 */

#ifndef _RTEMS_ASM_H
#define _RTEMS_ASM_H

/*
 *  Indicate we are in an assembly file and get the basic CPU definitions.
 */

#ifndef ASM
#define ASM
#endif
#include <rtems/score/cpuopts.h>
#include <rtems/score/cpu.h>

/*
 *  Recent versions of GNU cpp define variables which indicate the
 *  need for underscores and percents.  If not using GNU cpp or
 *  the version does not support this, then you will obviously
 *  have to define these as appropriate.
 */

#ifndef __USER_LABEL_PREFIX__
#define __USER_LABEL_PREFIX__ _
#endif

#ifndef __REGISTER_PREFIX__
#define __REGISTER_PREFIX__
#endif

#include <rtems/concat.h>

/* Use the right prefix for global labels.  */

#define SYM(x) CONCAT0 (__USER_LABEL_PREFIX__, x)

/* Use the right prefix for registers.  */

#define REG(x) CONCAT0 (__REGISTER_PREFIX__, x)

#define d0 REG (d0)
#define d1 REG (d1)
#define d2 REG (d2)
#define d3 REG (d3)
#define d4 REG (d4)
#define d5 REG (d5)
#define d6 REG (d6)
#define d7 REG (d7)
#define a0 REG (a0)
#define a1 REG (a1)
#define a2 REG (a2)
#define a3 REG (a3)
#define a4 REG (a4)
#define a5 REG (a5)
#define a6 REG (a6)
#define a7 REG (a7)
#define sp REG (sp)

#define msp REG (msp)
#define usp REG (usp)
#define isp REG (isp)
#define sr  REG (sr)
#define vbr REG (vbr)
#define dfc REG (dfc)
#define sfc REG (sfc)

/* mcf52xx special regs */
#define cacr    REG (cacr)
#define acr0    REG (acr0)
#define acr1    REG (acr1)
#define rambar0 REG (rambar0)
#define mbar    REG (mbar)

/* additional v4e special regs */
#define rambar1   REG (rambar1)
#define macsr     REG (macsr)
#define acc0      REG (acc0)
#define acc1      REG (acc1)
#define acc2      REG (acc2)
#define acc3      REG (acc3)
#define accext01  REG (accext01)
#define accext23  REG (accext23)
#define mask      REG (mask)


#define fp0 REG (fp0)
#define fp1 REG (fp1)
#define fp2 REG (fp2)
#define fp3 REG (fp3)
#define fp4 REG (fp4)
#define fp5 REG (fp5)
#define fp6 REG (fp6)
#define fp7 REG (fp7)

#define fpc REG (fpc)
#define fpi REG (fpi)
#define fps REG (fps)
#define fpsr REG (fpsr)


/*
 *  Define macros to handle section beginning and ends.
 */


#define BEGIN_CODE_DCL .text
#define END_CODE_DCL
#define BEGIN_DATA_DCL .data
#define END_DATA_DCL
#define BEGIN_CODE .text
#define END_CODE
#define BEGIN_DATA .data
#define END_DATA
#define BEGIN_BSS .bss
#define END_BSS
#define END

/*
 *  Following must be tailor for a particular flavor of the C compiler.
 *  They may need to put underscores in front of the symbols.
 */

#define PUBLIC(sym) .globl SYM (sym)
#define EXTERN(sym) .globl SYM (sym)

#endif
