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
#include <rtems/bspIo.h>
#include <rtems/score/cpusetimpl.h>

#ifdef __RTEMS_HAVE_SYS_CPUSET_H__

  void _CPU_set_Show_with_plugin(
    void                  *context,
    rtems_printk_plugin_t  print,
    const char            *description,
    const cpu_set_t       *cpuset
  );

  /*
   * _CPU_set_Show_with_plugin
   *
   * This routine shows cpuset cpuset using a
   * print plugin .
   */
  void _CPU_set_Show_with_plugin(
    void                  *context,
    rtems_printk_plugin_t  print,
    const char            *description,
    const cpu_set_t       *cpuset
  )
  {
    int i;

    if ( !print )
      return;

    (*print)(context ,"%s: ", description);
    for(i=0; i<_NCPUWORDS; i++)
      (*print)(context ,"%x", cpuset->__bits[i]);
    (*print)(context ,"\n");
  }

  /*
   * _CPU_set_Show
   *
   * This routine shows a cpuset using the
   * printk plugin.
   */
  void _CPU_set_Show( const char *description, const cpu_set_t   *cpuset)
  {
    _CPU_set_Show_with_plugin( NULL, printk_plugin, description, cpuset );
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
