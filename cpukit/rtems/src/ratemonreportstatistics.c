/*
 *  Rate Monotonic Manager -- Report Statistics for All Periods
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <stdlib.h>
#include <ctype.h>
#include <inttypes.h>

#include <rtems/bspIo.h>
#include <rtems/score/timespec.h>

#ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
  /* We print to 1/10's of milliseconds */
  #define NANOSECONDS_DIVIDER 1000
  #define PERCENT_FMT     "%04" PRId32
  #define NANOSECONDS_FMT "%06" PRId32
#endif

/*
 *  This directive allows a thread to print the statistics information
 *  on ALL period instances which have non-zero counts using printk.
 *
 *  The implementation of this directive straddles the fence between
 *  inside and outside of RTEMS.  It is presented as part of the Manager
 *  but actually uses other services of the Manager.
 */
void rtems_rate_monotonic_report_statistics_with_plugin(
  void                  *context,
  rtems_printk_plugin_t  print
)
{
  rtems_status_code                      status;
  rtems_id                               id;
  rtems_rate_monotonic_period_statistics the_stats;
  rtems_rate_monotonic_period_status     the_status;
  char                                   name[5];

  if ( !print )
    return;

  (*print)( context, "Period information by period\n" );
  #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
    (*print)( context, "--- CPU times are in seconds ---\n" );
    (*print)( context, "--- Wall times are in seconds ---\n" );
  #endif
/*
Layout by columns -- in memory of Hollerith :)

1234567890123456789012345678901234567890123456789012345678901234567890123456789\
   ID     OWNER COUNT MISSED X
ididididid NNNN ccccc mmmmmm X

  Uncomment the following if you are tinkering with the formatting.
  Be sure to test the various cases.
  (*print)( context,"\
1234567890123456789012345678901234567890123456789012345678901234567890123456789\
\n");
*/
  (*print)( context, "   ID     OWNER COUNT MISSED     "
       #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
          "     "
       #endif
          "CPU TIME     "
       #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
          "          "
       #endif
          "   WALL TIME\n"
  );
  (*print)( context, "                               "
       #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
          "     "
       #endif
          "MIN/MAX/AVG    "
       #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
          "          "
       #endif
          "  MIN/MAX/AVG\n"
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
    #if defined(RTEMS_DEBUG)
      status = rtems_rate_monotonic_get_status( id, &the_status );
      if ( status != RTEMS_SUCCESSFUL )
        continue;
    #else
      (void) rtems_rate_monotonic_get_status( id, &the_status );
    #endif

    rtems_object_get_name( the_status.owner, sizeof(name), name );

    /*
     *  Print part of report line that is not dependent on granularity
     */
    (*print)( context,
      "0x%08" PRIx32 " %4s %5" PRId32 " %6" PRId32 " ",
      id, name,
      the_stats.count, the_stats.missed_count
    );

    /*
     *  If the count is zero, don't print statistics
     */
    if (the_stats.count == 0) {
      (*print)( context, "\n" );
      continue;
    }

    /*
     *  print CPU Usage part of statistics
     */
    {
    #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
      struct timespec  cpu_average;
      struct timespec *min_cpu = &the_stats.min_cpu_time;
      struct timespec *max_cpu = &the_stats.max_cpu_time;
      struct timespec *total_cpu = &the_stats.total_cpu_time;

      _Timespec_Divide_by_integer( total_cpu, the_stats.count, &cpu_average );
      (*print)( context,
        "%" PRId32 "."  NANOSECONDS_FMT "/"        /* min cpu time */
        "%" PRId32 "."  NANOSECONDS_FMT "/"        /* max cpu time */
        "%" PRId32 "."  NANOSECONDS_FMT " ",       /* avg cpu time */
        _Timespec_Get_seconds( min_cpu ),
	  _Timespec_Get_nanoseconds( min_cpu ) / NANOSECONDS_DIVIDER,
        _Timespec_Get_seconds( max_cpu ),
	  _Timespec_Get_nanoseconds( max_cpu ) / NANOSECONDS_DIVIDER,
        _Timespec_Get_seconds( &cpu_average ),
	  _Timespec_Get_nanoseconds( &cpu_average ) / NANOSECONDS_DIVIDER
       );
    #else
      uint32_t ival_cpu, fval_cpu;

      ival_cpu = the_stats.total_cpu_time * 100 / the_stats.count;
      fval_cpu = ival_cpu % 100;
      ival_cpu /= 100;

      (*print)( context,
        "%3" PRId32 "/%4" PRId32 "/%3" PRId32 ".%02" PRId32 " ",
        the_stats.min_cpu_time, the_stats.max_cpu_time, ival_cpu, fval_cpu
      );
    #endif
    }

    /*
     *  print wall time part of statistics
     */
    {
    #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
      struct timespec  wall_average;
      struct timespec *min_wall = &the_stats.min_wall_time;
      struct timespec *max_wall = &the_stats.max_wall_time;
      struct timespec *total_wall = &the_stats.total_wall_time;

      _Timespec_Divide_by_integer(total_wall, the_stats.count, &wall_average);
      (*print)( context,
        "%" PRId32 "." NANOSECONDS_FMT "/"        /* min wall time */
        "%" PRId32 "." NANOSECONDS_FMT "/"        /* max wall time */
        "%" PRId32 "." NANOSECONDS_FMT "\n",      /* avg wall time */
        _Timespec_Get_seconds( min_wall ),
          _Timespec_Get_nanoseconds( min_wall ) / NANOSECONDS_DIVIDER,
        _Timespec_Get_seconds( max_wall ),
          _Timespec_Get_nanoseconds( max_wall ) / NANOSECONDS_DIVIDER,
        _Timespec_Get_seconds( &wall_average ),
          _Timespec_Get_nanoseconds( &wall_average ) / NANOSECONDS_DIVIDER
      );
    #else
      uint32_t  ival_wall, fval_wall;

      ival_wall = the_stats.total_wall_time * 100 / the_stats.count;
      fval_wall = ival_wall % 100;
      ival_wall /= 100;
      (*print)( context,
        "%3" PRId32 "/%4" PRId32 "/%3" PRId32 ".%02" PRId32 "\n",
        the_stats.min_wall_time, the_stats.max_wall_time, ival_wall, fval_wall
      );
    #endif
    }
  }
}

void rtems_rate_monotonic_report_statistics( void )
{
  rtems_rate_monotonic_report_statistics_with_plugin( NULL, printk_plugin );
}
