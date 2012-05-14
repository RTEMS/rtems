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
#include <rtems/score/i386.h>

/*
 *  Recent versions of GNU cpp define variables which indicate the
 *  need for underscores and percents.  If not using GNU cpp or
 *  the version does not support this, then you will obviously
 *  have to define these as appropriate.
 */

#ifndef __USER_LABEL_PREFIX__
#define __USER_LABEL_PREFIX__
#endif

/*
 *  Looks like there is a bug in gcc 2.6.2 where this is not
 *  defined correctly when configured as i386-coff and
 *  i386-aout.
 */

#undef __REGISTER_PREFIX__
#define __REGISTER_PREFIX__ %

/*
#ifndef __REGISTER_PREFIX__
#define __REGISTER_PREFIX__
#endif
*/

#include <rtems/concat.h>

/* Use the right prefix for global labels.  */

#define SYM(x) CONCAT0 (__USER_LABEL_PREFIX__, x)

/* Use the right prefix for registers.  */

#define REG(x) CONCAT0 (__REGISTER_PREFIX__, x)

#define eax REG (eax)
#define ebx REG (ebx)
#define ecx REG (ecx)
#define edx REG (edx)
#define esi REG (esi)
#define edi REG (edi)
#define esp REG (esp)
#define ebp REG (ebp)
#define cr0 REG (cr0)
#define cr4 REG (cr4)

#define ax REG (ax)
#define bx REG (bx)
#define cx REG (cx)
#define dx REG (dx)
#define si REG (si)
#define di REG (di)
#define sp REG (sp)
#define bp REG (bp)

#define ah REG (ah)
#define bh REG (bh)
#define ch REG (ch)
#define dh REG (dh)

#define al REG (al)
#define bl REG (bl)
#define cl REG (cl)
#define dl REG (dl)

#define cs REG (cs)
#define ds REG (ds)
#define es REG (es)
#define fs REG (fs)
#define gs REG (gs)
#define ss REG (ss)

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
