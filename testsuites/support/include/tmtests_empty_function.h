/*
 *  Prototypes for RTEMS tmtests_empty_function.c.
 *
 *  COPYRIGHT (c) 2011, Ralf Corsépius, Ulm/Germany.
 * 
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _TMTESTS_EMPTY_FUNCTION_H
#define _TMTESTS_EMPTY_FUNCTION_H

#include <rtems/rtems/status.h>

extern rtems_status_code benchmark_timer_empty_function( void );

extern void benchmark_timer_empty_operation(
  int    iteration,
  void  *argument
);

#endif /* _TMTESTS_EMPTY_FUNCTION_H */
