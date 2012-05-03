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
 *  COPYRIGHT (c) 1994-2006.
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
#include <rtems/score/no_cpu.h>

#ifndef __USER_LABEL_PREFIX__
/**
 *  Recent versions of GNU cpp define variables which indicate the
 *  need for underscores and percents.  If not using GNU cpp or
 *  the version does not support this, then you will obviously
 *  have to define these as appropriate.
 *
 *  This symbol is prefixed to all C program symbols.
 */
#define __USER_LABEL_PREFIX__ _
#endif

#ifndef __REGISTER_PREFIX__
/**
 *  Recent versions of GNU cpp define variables which indicate the
 *  need for underscores and percents.  If not using GNU cpp or
 *  the version does not support this, then you will obviously
 *  have to define these as appropriate.
 *
 *  This symbol is prefixed to all register names.
 */
#define __REGISTER_PREFIX__
#endif

#include <rtems/concat.h>

/** Use the right prefix for global labels.  */
#define SYM(x) CONCAT1 (__USER_LABEL_PREFIX__, x)

/** Use the right prefix for registers.  */
#define REG(x) CONCAT1 (__REGISTER_PREFIX__, x)

/*
 *  define macros for all of the registers on this CPU
 *
 *  EXAMPLE:     #define d0 REG (d0)
 */

/*
 *  Define macros to handle section beginning and ends.
 */


/** This macro is used to denote the beginning of a code declaration. */
#define BEGIN_CODE_DCL .text
/** This macro is used to denote the end of a code declaration. */
#define END_CODE_DCL
/** This macro is used to denote the beginning of a data declaration section. */
#define BEGIN_DATA_DCL .data
/** This macro is used to denote the end of a data declaration section. */
#define END_DATA_DCL
/** This macro is used to denote the beginning of a code section. */
#define BEGIN_CODE .text
/** This macro is used to denote the end of a code section. */
#define END_CODE
/** This macro is used to denote the beginning of a data section. */
#define BEGIN_DATA
/** This macro is used to denote the end of a data section. */
#define END_DATA
/** This macro is used to denote the beginning of the
 *  unitialized data section.
 */
#define BEGIN_BSS
/** This macro is used to denote the end of the unitialized data section.  */
#define END_BSS
/** This macro is used to denote the end of the assembly file.  */
#define END

/**
 *  This macro is used to declare a public global symbol.
 *
 *  @note This must be tailored for a particular flavor of the C compiler.
 *  They may need to put underscores in front of the symbols.
 */
#define PUBLIC(sym) .globl SYM (sym)

/**
 *  This macro is used to prototype a public global symbol.
 *
 *  @note This must be tailored for a particular flavor of the C compiler.
 *  They may need to put underscores in front of the symbols.
 */
#define EXTERN(sym) .globl SYM (sym)

#endif
