/*  internal.h
 *
 *  This include file contains internal information
 *  for the RTEMS stack checker.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __INTERNAL_STACK_CHECK_h
#define __INTERNAL_STACK_CHECK_h

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This structure is used to fill in and compare the "end of stack"
 *  marker pattern.
 *  pattern area must be a multiple of 4 words.
 */

#ifdef CPU_STACK_CHECK_SIZE
#define PATTERN_SIZE_WORDS      (((CPU_STACK_CHECK_SIZE / 4) + 3) & ~0x3)
#else
#define PATTERN_SIZE_WORDS      (4)
#endif

#define PATTERN_SIZE_BYTES      (PATTERN_SIZE_WORDS * sizeof(uint32_t))

typedef struct {
   uint32_t    pattern[ PATTERN_SIZE_WORDS ];
} Stack_check_Control;

/*
 *  The pattern used to fill the entire stack.
 */

#define BYTE_PATTERN 0xA5
#define U32_PATTERN 0xA5A5A5A5

/*
 *  rtems_stack_checker_create_extension
 */

bool rtems_stack_checker_create_extension(
  Thread_Control *running,
  Thread_Control *the_thread
);

/*
 *  rtems_stack_checker_begin_extension
 */

void rtems_stack_checker_begin_extension(
  Thread_Control *the_thread
);

/*
 *  rtems_stack_checker_switch_extension
 */

void rtems_stack_checker_switch_extension(
  Thread_Control *running,
  Thread_Control *heir
);

/*
 *  rtems_stack_checker_fatal_extension
 */

void rtems_stack_checker_fatal_extension(
    Internal_errors_Source  source,
    bool                    is_internal,
    uint32_t                status
);

/*
 *  rtems_stack_checker_report_usage
 */

void rtems_stack_checker_report_usage( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
