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
/* @(#)asm.h       03/15/96     1.1 */

#ifndef _RTEMS_ASM_H
#define _RTEMS_ASM_H

/*
 *  Indicate we are in an assembly file and get the basic CPU definitions.
 */

#ifndef ASM
#define ASM
#endif
#include <rtems/system.h>
#include <rtems/score/cpuopts.h>
#include <rtems/score/mips.h>

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

#define SYM(x) CONCAT1 (__USER_LABEL_PREFIX__, x)

/* Use the right prefix for registers.  */

#define REG(x) CONCAT1 (__REGISTER_PREFIX__, x)

/*
 *  define macros for all of the registers on this CPU
 *
 *  EXAMPLE:     #define d0 REG (d0)
 */

/*
 *  Define macros to handle section beginning and ends.
 */


#define BEGIN_CODE_DCL .text
#define END_CODE_DCL
#define BEGIN_DATA_DCL .data
#define END_DATA_DCL
#define BEGIN_CODE .text
#define END_CODE
#define BEGIN_DATA
#define END_DATA
#define BEGIN_BSS
#define END_BSS
#define END

/*
 *  Following must be tailor for a particular flavor of the C compiler.
 *  They may need to put underscores in front of the symbols.
 */

#define PUBLIC(sym) .globl SYM (sym)
#define EXTERN(sym) .globl SYM (sym)

/*
 *  Debugger macros for assembly language routines. Allows the
 *  programmer to set up the necessary stack frame info
 *  required by debuggers to do stack traces.
 */

#ifndef XDS
#define FRAME(name,frm_reg,offset,ret_reg)      \
        .globl  name;                           \
        .ent    name;                           \
name:;                                          \
        .frame  frm_reg,offset,ret_reg
#define ENDFRAME(name)                          \
        .end name
#else
#define FRAME(name,frm_reg,offset,ret_reg)      \
        .globl  _##name;\
_##name:
#define ENDFRAME(name)
#endif /* XDS */

/*
 * Hardware Floating Point Registers
 */

#define R_FP0 	0
#define R_FP1 	1
#define R_FP2 	2
#define R_FP3 	3
#define R_FP4 	4
#define R_FP5 	5
#define R_FP6 	6
#define R_FP7 	7
#define R_FP8 	8
#define R_FP9 	9
#define R_FP10 	10
#define R_FP11 	11
#define R_FP12 	12
#define R_FP13 	13
#define R_FP14 	14
#define R_FP15 	15
#define R_FP16 	16
#define R_FP17 	17
#define R_FP18 	18
#define R_FP19 	19
#define R_FP20 	20
#define R_FP21 	21
#define R_FP22 	22
#define R_FP23	23
#define R_FP24 	24
#define R_FP25 	25
#define R_FP26 	26
#define R_FP27 	27
#define R_FP28 	28
#define R_FP29 	29
#define R_FP30 	30
#define R_FP31 	31

#endif
/* end of include file */

