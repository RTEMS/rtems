/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the executive.  This must be the first include file
 *  included in all internal RTEMS files.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#ifndef __RTEMS_SYSTEM_h
#define __RTEMS_SYSTEM_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The following define the CPU Family and Model within the family
 *
 *  NOTE: The string "REPLACE_THIS_WITH_USE_INLINE_OR_MACROS" is replaced
 *        with either "USE_INLINES" or "USE_MACROS" based upon the
 *        whether this target configuration uses the inline or
 *        macro implementations of the inlined RTEMS routines.
 */
 

#define REPLACE_THIS_WITH_USE_INLINE_OR_MACROS

/*
 *  The following insures that all data is declared in the space
 *  of the Initialization Manager.  It is referenced as "external"
 *  in every other file.
 */

#ifdef INIT
#undef EXTERN
#define EXTERN
#else
#undef EXTERN
#define EXTERN  extern
#endif

/*
 *  The following (in conjunction with compiler arguments) are used
 *  to choose between the use of static inline functions and macro
 *  functions.   The static inline implementation allows better
 *  type checking with no cost in code size or execution speed.
 */

#ifdef USE_INLINES
#define STATIC static
#define INLINE __inline__
#else
/*
#error Only the GNU C compiler is currently supported!!!
*/
#define STATIC
#define INLINE
#endif

/*
 *  Include a base set of files.
 */

/*
 * XXX: Eventually proc_ptr needs to disappear!!!
 */

typedef void * proc_ptr;

#include <rtems/cpu.h>             /* processor specific information */
#include <rtems/status.h>          /* RTEMS status codes */

/*
 *  Define NULL
 */

#ifndef NULL
#define NULL      0          /* NULL value */
#endif

/*
 *  Boolean constants
 */

#if !defined( TRUE ) || (TRUE != 1)
#undef TRUE
#define TRUE     (1)
#endif

#if !defined( FALSE ) || (FALSE != 0)
#undef FALSE
#define FALSE     (0)
#endif

#define stringify( _x ) # _x

/*
 *  The following is the extern for the RTEMS version string.
 *  The contents of this string are CPU specific.
 */

extern const char _RTEMS_version[];         /* RTEMS version string */
extern const char _Copyright_Notice[];      /* RTEMS copyright string */

/*
 *  The jump table of entry points into RTEMS directives.
 */

#define NUMBER_OF_ENTRY_POINTS  79
extern const void * _Entry_points[ NUMBER_OF_ENTRY_POINTS + 1 ];

/*
 *  The following defines the CPU dependent information table.
 */

EXTERN rtems_cpu_table _CPU_Table;               /* CPU dependent info */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
