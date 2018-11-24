/**
 *  @file
 *
 *  @brief RTEMS Report Rate Monotonic Statistics
 *  @ingroup ClassicRateMon
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/ratemonimpl.h>
#include <rtems/rtems/object.h>
#include <rtems/printer.h>

#include <inttypes.h>
#include <rtems/inttypes.h>

/* We print to 1/10's of milliseconds */
#define NANOSECONDS_DIVIDER 1000L
#define PERCENT_FMT     "%04" PRId32
#define NANOSECONDS_FMT "%06ld"

void rtems_rate_monotonic_report_statistics_with_plugin(
  const rtems_printer *printer
)
{
  rtems_status_code                      status;
  rtems_id                               maximum_id;
  rtems_id                               id;
  rtems_rate_monotonic_period_statistics the_stats;
  rtems_rate_monotonic_period_status     the_status;
  char                                   name[5];

  rtems_printf( printer, "Period information by period\n" );
  rtems_printf( printer, "--- CPU times are in seconds ---\n" );
  rtems_printf( printer, "--- Wall times are in seconds ---\n" );
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
  rtems_printf( printer,
      "   ID     OWNER COUNT MISSED     "
      "     CPU TIME                  WALL TIME\n"
      "                               "
      "     MIN/MAX/AVG                MIN/MAX/AVG\n"
  );

  /*
   * Cycle through all possible ids and try to report on each one.  If it
   * is a period that is inactive, we just get an error back.  No big deal.
   */
  maximum_id = _Rate_monotonic_Information.maximum_id;
  for (
    id = _Objects_Get_minimum_id( maximum_id ) ;
    id <= maximum_id ;
    ++id
  ) {
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
    rtems_printf( printer,
      "0x%08" PRIx32 " %4s %5" PRId32 " %6" PRId32 " ",
      id, name,
      the_stats.count, the_stats.missed_count
    );

    /*
     *  If the count is zero, don't print statistics
     */
    if (the_stats.count == 0) {
      rtems_printf( printer, "\n" );
      continue;
    }

    /*
     *  print CPU Usage part of statistics
     */
    {
      struct timespec  cpu_average;
      struct timespec *min_cpu = &the_stats.min_cpu_time;
      struct timespec *max_cpu = &the_stats.max_cpu_time;
      struct timespec *total_cpu = &the_stats.total_cpu_time;

      _Timespec_Divide_by_integer( total_cpu, the_stats.count, &cpu_average );
      rtems_printf( printer,
        "%" PRIdtime_t "."  NANOSECONDS_FMT "/"        /* min cpu time */
        "%" PRIdtime_t "."  NANOSECONDS_FMT "/"        /* max cpu time */
        "%" PRIdtime_t "."  NANOSECONDS_FMT " ",       /* avg cpu time */
        _Timespec_Get_seconds( min_cpu ),
	  _Timespec_Get_nanoseconds( min_cpu ) / NANOSECONDS_DIVIDER,
        _Timespec_Get_seconds( max_cpu ),
	  _Timespec_Get_nanoseconds( max_cpu ) / NANOSECONDS_DIVIDER,
        _Timespec_Get_seconds( &cpu_average ),
	  _Timespec_Get_nanoseconds( &cpu_average ) / NANOSECONDS_DIVIDER
       );
    }

    /*
     *  print wall time part of statistics
     */
    {
      struct timespec  wall_average;
      struct timespec *min_wall = &the_stats.min_wall_time;
      struct timespec *max_wall = &the_stats.max_wall_time;
      struct timespec *total_wall = &the_stats.total_wall_time;

      _Timespec_Divide_by_integer(total_wall, the_stats.count, &wall_average);
      rtems_printf( printer,
        "%" PRIdtime_t "." NANOSECONDS_FMT "/"        /* min wall time */
        "%" PRIdtime_t "." NANOSECONDS_FMT "/"        /* max wall time */
        "%" PRIdtime_t "." NANOSECONDS_FMT "\n",      /* avg wall time */
        _Timespec_Get_seconds( min_wall ),
          _Timespec_Get_nanoseconds( min_wall ) / NANOSECONDS_DIVIDER,
        _Timespec_Get_seconds( max_wall ),
          _Timespec_Get_nanoseconds( max_wall ) / NANOSECONDS_DIVIDER,
        _Timespec_Get_seconds( &wall_average ),
          _Timespec_Get_nanoseconds( &wall_average ) / NANOSECONDS_DIVIDER
      );
    }
  }
}

void rtems_rate_monotonic_report_statistics( void )
{
  rtems_printer printer;
  rtems_print_printer_printk( &printer );
  rtems_rate_monotonic_report_statistics_with_plugin( &printer );
}
