/*  system.h
 *
 *  This include file contains information that is included in every
 *  function in the executive.  This must be the first include file
 *  included in all internal RTEMS files.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __RTEMS_SYSTEM_h
#define __RTEMS_SYSTEM_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The cpu options include file defines all cpu dependent 
 *  parameters for this build of RTEMS.  It must be included
 *  first so the basic macro definitions are in place.
 */

#include <rtems/score/cpuopts.h>

/*
 *  The following ensures that all data is declared in the space
 *  of the initialization routine for either the Initialization Manager
 *  or the initialization file for the appropriate API.  It is 
 *  referenced as "external" in every other file.
 */

#ifdef SCORE_INIT
#undef  SCORE_EXTERN
#define SCORE_EXTERN
#else
#undef  SCORE_EXTERN
#define SCORE_EXTERN  extern
#endif

#ifdef SAPI_INIT
#undef  SAPI_EXTERN
#define SAPI_EXTERN
#else
#undef  SAPI_EXTERN
#define SAPI_EXTERN  extern
#endif

#ifdef RTEMS_API_INIT
#undef  RTEMS_EXTERN
#define RTEMS_EXTERN
#else
#undef  RTEMS_EXTERN
#define RTEMS_EXTERN  extern
#endif

#ifdef POSIX_API_INIT
#undef  POSIX_EXTERN
#define POSIX_EXTERN
#else
#undef  POSIX_EXTERN
#define POSIX_EXTERN  extern
#endif

#ifdef ITRON_API_INIT
#undef  ITRON_EXTERN
#define ITRON_EXTERN
#else
#undef  ITRON_EXTERN
#define ITRON_EXTERN  extern
#endif

/*
 *  The following (in conjunction with compiler arguments) are used
 *  to choose between the use of static inline functions and macro
 *  functions.   The static inline implementation allows better
 *  type checking with no cost in code size or execution speed.
 */

#ifdef USE_INLINES
# ifdef __GNUC__
#  define RTEMS_INLINE_ROUTINE static __inline__
# else
#  define RTEMS_INLINE_ROUTINE static inline
# endif
#else
# define RTEMS_INLINE_ROUTINE
#endif

/*
 *  The following are used by the POSIX implementation to catch bad paths.
 */

#ifdef RTEMS_POSIX_API
int POSIX_MP_NOT_IMPLEMENTED( void );
int POSIX_NOT_IMPLEMENTED( void );
int POSIX_BOTTOM_REACHED( void );
#endif

/*
 *  Include a base set of files.
 */

/*
 * XXX: Eventually proc_ptr needs to disappear!!!
 */

typedef void * proc_ptr;

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

#include <rtems/score/cpu.h>        /* processor specific information */

#define stringify( _x ) # _x

#define RTEMS_offsetof(type, field) \
	((unsigned32) &(((type *) 0)->field))

/*
 *  The following is the extern for the RTEMS version string.
 *  The contents of this string are CPU specific.
 */

extern const char _RTEMS_version[];         /* RTEMS version string */
extern const char _Copyright_Notice[];      /* RTEMS copyright string */

/*
 *  The following defines the CPU dependent information table.
 */

SCORE_EXTERN rtems_cpu_table _CPU_Table;               /* CPU dependent info */

/*
 *  Macros to access CPU Table fields required by ALL ports.
 *
 *  NOTE: Similar macros to access port specific fields in in the
 *        appropriate cpu.h file.
 */

#define rtems_cpu_configuration_get_table() \
   (&_CPU_Table)

#define rtems_cpu_configuration_get_pretasking_hook() \
   (_CPU_Table.pretasking_hook)

#define rtems_cpu_configuration_get_predriver_hook() \
   (_CPU_Table.predriver_hook)

#define rtems_cpu_configuration_get_postdriver_hook() \
   (_CPU_Table.postdriver_hook)

#define rtems_cpu_configuration_get_idle_task() \
   (_CPU_Table.idle_task)

#define rtems_cpu_configuration_get_do_zero_of_workspace() \
   (_CPU_Table.do_zero_of_workspace)

#define rtems_cpu_configuration_get_idle_task_stack_size() \
   (_CPU_Table.idle_task_stack_size)

#define rtems_cpu_configuration_get_interrupt_stack_size() \
   (_CPU_Table.interrupt_stack_size)

#define rtems_cpu_configuration_get_extra_mpci_receive_server_stack() \
   (_CPU_Table.extra_mpci_receive_server_stack)

#define rtems_cpu_configuration_get_stack_allocate_hook() \
   (_CPU_Table.stack_allocate_hook)

#define rtems_cpu_configuration_get_stack_free_hook() \
   (_CPU_Table.stack_free_hook)

/*
 *  XXX weird RTEMS stuff that probably should be somewhere else.
 */

#define RTEMS_MAXIMUM_NAME_LENGTH sizeof(rtems_name)

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
