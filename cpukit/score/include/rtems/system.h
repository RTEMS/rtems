/** 
 *  @file  rtems/system.h
 *
 *  This include file contains information that is included in every
 *  function in the executive.  This must be the first include file
 *  included in all internal RTEMS files.
 */

/*
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SYSTEM_H
#define _RTEMS_SYSTEM_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  The cpu options include file defines all cpu dependent
 *  parameters for this build of RTEMS.  It must be included
 *  first so the basic macro definitions are in place.
 */
#include <rtems/score/cpuopts.h>

/**
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

/** 
 *  The following ensures that all data is declared in the space
 *  of the initialization routine for either the Initialization Manager
 *  or the initialization file for the appropriate API.  It is
 *  referenced as "external" in every other file.
 */
#ifdef SAPI_INIT
#undef  SAPI_EXTERN
#define SAPI_EXTERN
#else
#undef  SAPI_EXTERN
#define SAPI_EXTERN  extern
#endif

/** 
 *  The following ensures that all data is declared in the space
 *  of the initialization routine for either the Initialization Manager
 *  or the initialization file for the appropriate API.  It is
 *  referenced as "external" in every other file.
 */
#ifdef RTEMS_API_INIT
#undef  RTEMS_EXTERN
#define RTEMS_EXTERN
#else
#undef  RTEMS_EXTERN
#define RTEMS_EXTERN  extern
#endif

/** 
 *  The following ensures that all data is declared in the space
 *  of the initialization routine for either the Initialization Manager
 *  or the initialization file for the appropriate API.  It is
 *  referenced as "external" in every other file.
 */
#ifdef POSIX_API_INIT
#undef  POSIX_EXTERN
#define POSIX_EXTERN
#else
#undef  POSIX_EXTERN
#define POSIX_EXTERN  extern
#endif

/** 
 *  The following ensures that all data is declared in the space
 *  of the initialization routine for either the Initialization Manager
 *  or the initialization file for the appropriate API.  It is
 *  referenced as "external" in every other file.
 */
#ifdef ITRON_API_INIT
#undef  ITRON_EXTERN
#define ITRON_EXTERN
#else
#undef  ITRON_EXTERN
#define ITRON_EXTERN  extern
#endif

/**
 *  The following (in conjunction with compiler arguments) are used
 *  to choose between the use of static inline functions and macro
 *  functions.   The static inline implementation allows better
 *  type checking with no cost in code size or execution speed.
 */
#ifdef __GNUC__
#  define RTEMS_INLINE_ROUTINE static __inline__
#else
#  define RTEMS_INLINE_ROUTINE static inline
#endif

/**
 *  The following macro is a compiler specific way to ensure that memory
 *  writes are not reordered around certian points.  This specifically can
 *  impact interrupt disable and thread dispatching critical sections.
 */
#ifdef __GNUC__
  #define RTEMS_COMPILER_MEMORY_BARRIER() asm volatile("" ::: "memory")
#else
  #define RTEMS_COMPILER_MEMORY_BARRIER()
#endif

#ifdef RTEMS_POSIX_API
/** The following is used by the POSIX implementation to catch bad paths.  */
int POSIX_MP_NOT_IMPLEMENTED( void );

/** The following is used by the POSIX implementation to catch bad paths.  */
int POSIX_NOT_IMPLEMENTED( void );

/** The following is used by the POSIX implementation to catch bad paths.  */
int POSIX_BOTTOM_REACHED( void );
#endif

/*
 *  Include a base set of files.
 */

/**
 * XXX: Eventually proc_ptr needs to disappear!!!
 */
typedef void * proc_ptr;

#include <stddef.h>

#if !defined( TRUE ) || (TRUE != 1)
/**  Boolean constant TRUE */
#undef TRUE
#define TRUE     (1)
#endif

#if !defined( FALSE ) || (FALSE != 0)
/**  Boolean constant FALSE */
#undef FALSE
#define FALSE     (0)
#endif

#include <rtems/stdint.h>
#include <rtems/score/cpu.h>        /* processor specific information */

/** 
 *  This macro is used to obtain the offset of a field in a structure.
 */
#define RTEMS_offsetof(type, field) \
       ((uint32_t  ) &(((type *) 0)->field))

/**
 *  The following is the extern for the RTEMS version string.
 *
 *  @note The contents of this string are CPU specific.
 */
extern const char _RTEMS_version[];

/**
 *  The following is the extern for the RTEMS copyright string.
 */
extern const char _Copyright_Notice[];

/**
 *  The following defines the CPU dependent information table.
 */
SCORE_EXTERN rtems_cpu_table _CPU_Table;

/*
 *  Macros to access CPU Table fields required by ALL ports.
 */

/** This macro assists in accessing the CPU Specific Configuration Table. */
#define rtems_cpu_configuration_get_table() (&_CPU_Table)


/** This macro assists in accessing the pretasking BSP hook. */
#define rtems_cpu_configuration_get_pretasking_hook() \
   (_CPU_Table.pretasking_hook)

/** This macro assists in accessing the predriver BSP hook. */
#define rtems_cpu_configuration_get_predriver_hook() \
   (_CPU_Table.predriver_hook)

/** This macro assists in accessing the postdriver BSP hook. */
#define rtems_cpu_configuration_get_postdriver_hook() \
   (_CPU_Table.postdriver_hook)

/** This macro assists in accessing the BSP specific IDLE task entry point. */
#define rtems_cpu_configuration_get_idle_task() \
   (_CPU_Table.idle_task)

/**
  * This macro assists in accessing the field which indicates whether
  * RTEMS is responsible for zeroing the Executive Workspace.
  */
#define rtems_cpu_configuration_get_do_zero_of_workspace() \
   (_CPU_Table.do_zero_of_workspace)

/** This macro assists in accessing the IDLE task stack point size. */
#define rtems_cpu_configuration_get_idle_task_stack_size() \
   (_CPU_Table.idle_task_stack_size)

/** This macro assists in accessing the interrupt stack size. */
#define rtems_cpu_configuration_get_interrupt_stack_size() \
   (_CPU_Table.interrupt_stack_size)

/** This macro assists in accessing the size of the MPCI receiver server. */
#define rtems_cpu_configuration_get_extra_mpci_receive_server_stack() \
   (_CPU_Table.extra_mpci_receive_server_stack)

/** This macro assists in accessing the BSP stack allocation hook. */
#define rtems_cpu_configuration_get_stack_allocate_hook() \
   (_CPU_Table.stack_allocate_hook)

/** This macro assists in accessing the BSP stack allocation free hook. */
#define rtems_cpu_configuration_get_stack_free_hook() \
   (_CPU_Table.stack_free_hook)

/** This macro defines the maximum length of a Classic API name. */
#define RTEMS_MAXIMUM_NAME_LENGTH sizeof(rtems_name)

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
