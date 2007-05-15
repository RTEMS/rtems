/*
 *  Rate Monotonic Manager -- Report Statistics for All Periods
 *
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>

#include <rtems/bspIo.h>

/*
 *  This directive allows a thread to print the statistics information
 *  on ALL period instances which have non-zero counts using printk.
 *
 *  The implementation of this directive straddles the fence between
 *  inside and outside of RTEMS.  It is presented as part of the Manager
 *  but actually uses other services of the Manager.
 */
void rtems_rate_montonic_report_statistics( void )
{
  rtems_status_code                      status;
  rtems_id                               id;
  rtems_rate_monotonic_period_statistics the_stats;
  rtems_rate_monotonic_period_status     the_status;
  char                                   name[5];
  uint32_t                               ival_cpu, fval_cpu;
  uint32_t                               ival_wall, fval_wall;

  printk(
    "Period information by period\n"
   "   ID      OWNER   PERIODS  MISSED    CPU TIME    WALL TIME\n"
  );

  /*
   * Cycle through all possible ids and try to report on each one.  If it
   * is a period that is inactive, we just get an error back.  No big deal.
   */
  for ( id=_Rate_monotonic_Information.minimum_id ;
        id <= _Rate_monotonic_Information.maximum_id ;
        id++ ) {
    status = rtems_rate_monotonic_get_statistics( id, &the_stats );
    if ( status != RTEMS_SUCCESSFUL )
      continue;
    
    /* If the above passed, so should this but check it anyway */
    status = rtems_rate_monotonic_get_status( id, &the_status );
    if ( status != RTEMS_SUCCESSFUL )
      continue;
    
    if ( the_stats.count == 0 )
      continue;

    ival_cpu = the_stats.total_cpu_time * 100 / the_stats.count;

    name[ 0 ] = '\0';

    if ( the_status.owner ) {
      rtems_object_get_name( the_status.owner, sizeof(name), name );
    }

    fval_cpu = ival_cpu % 100;
    ival_cpu /= 100;
    ival_wall = the_stats.total_wall_time * 100 / the_stats.count;
    fval_wall = ival_wall % 100;
    ival_wall /= 100;
    printk(
      "0x%08" PRIx32 "  %4s   %6" PRId32 "   %3" PRId32 "       "
         "%" PRId32 "/%" PRId32 "/%" PRId32 ".%02" PRId32 "    "
         "%" PRId32 "/%" PRId32 "/%" PRId32 ".%02" PRId32 "\n",
      id, name,
      the_stats.count, the_stats.missed_count,
      the_stats.min_cpu_time, the_stats.max_cpu_time, ival_cpu, fval_cpu,
      the_stats.min_wall_time, the_stats.max_wall_time, ival_wall, fval_wall
    );
  }
}
