/**
 * @file
 *
 * @brief CPU Set Print Support Routines
 * @ingroup ScoreCpuset
 */

/*
 *  COPYRIGHT (c) 2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>
#include <rtems/printer.h>
#include <rtems/score/cpusetimpl.h>

#ifdef __RTEMS_HAVE_SYS_CPUSET_H__

  void _CPU_set_Show_with_plugin(
    const rtems_printer *printer,
    const char          *description,
    const cpu_set_t     *cpuset
  );

  /*
   * _CPU_set_Show_with_plugin
   *
   * This routine shows cpuset cpuset using a
   * print plugin .
   */
  void _CPU_set_Show_with_plugin(
    const rtems_printer *printer,
    const char          *description,
    const cpu_set_t     *cpuset
  )
  {
    int i;
    rtems_printf(printer ,"%s: ", description);
    for(i=0; i<_NCPUWORDS; i++)
      rtems_printf(printer ,"%" PRIx32 "", cpuset->__bits[i]);
    rtems_printf(printer ,"\n");
  }

  /*
   * _CPU_set_Show
   *
   * This routine shows a cpuset using the
   * printk plugin.
   */
  void _CPU_set_Show( const char *description, const cpu_set_t   *cpuset)
  {
    rtems_printer printer;
    rtems_print_printer_printk( &printer );
    _CPU_set_Show_with_plugin( &printer, description, cpuset );
  }

  /*
   * _CPU_set_Show_default
   *
   * This routine shows the default cpuset.
   */
 void _CPU_set_Show_default( const char *description )
  {
    const CPU_set_Control *ctl;
    ctl = _CPU_set_Default();
    _CPU_set_Show( description, ctl->set );
  }
#endif
