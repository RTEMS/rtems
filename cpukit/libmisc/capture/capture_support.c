/*
  ------------------------------------------------------------------------

  Copyright 2002, 2015 Chris Johns <chrisj@rtems.org>
  All rights reserved.

  COPYRIGHT (c) 1989-2014.
  On-Line Applications Research Corporation (OAR).

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution.

  This software with is provided ``as is'' and with NO WARRANTY.

  ------------------------------------------------------------------------

  RTEMS Performance Monitoring and Measurement Framework.

  This is a set of print support routines that may be shared between
  the RTEMS monitor and direct callers of the capture engine.

*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/monitor.h>
#include <rtems/captureimpl.h>

/*
 * Structure used during printing of the capture records.
 */

typedef struct {
  rtems_capture_record_t* rec;
  uint32_t                read;
  uint32_t                last_t;
  uint32_t                printed;
} ctrace_per_cpu_t;

/*
 * rtems_catpure_print_uptime
 *
 * This function prints the nanosecond uptime to stdout.
 */
void
rtems_capture_print_timestamp (uint64_t uptime)
{
  uint32_t hours;
  uint32_t minutes;
  uint32_t seconds;
  uint32_t nanosecs;
  uint64_t up_secs;

  up_secs  = uptime / 1000000000LLU;
  minutes  = up_secs / 60;
  hours    = minutes / 60;
  minutes  = minutes % 60;
  seconds  = up_secs % 60;
  nanosecs = uptime % 1000000000;

  fprintf (stdout, "%5" PRIu32 ":%02" PRIu32 ":%02" PRIu32".%09" PRIu32,
             hours, minutes, seconds, nanosecs);
}

void
rtems_capture_print_record_task( uint32_t cpu, rtems_capture_record_t* rec)
{
  rtems_capture_task_record_t* task_rec = (rtems_capture_task_record_t*) rec;

  fprintf(stdout,"%2" PRId32 " ", cpu);
  rtems_capture_print_timestamp (rec->time);
  fprintf (stdout, "              ");
  rtems_monitor_dump_id (rec->task_id);
  if (rtems_object_id_get_api(rec->task_id) != OBJECTS_POSIX_API)
  {
    fprintf (stdout, " %c%c%c%c",
             (char) (task_rec->name >> 24) & 0xff,
             (char) (task_rec->name >> 16) & 0xff,
             (char) (task_rec->name >> 8) & 0xff,
             (char) (task_rec->name >> 0) & 0xff);
  }
  else
  {
    fprintf (stdout, " ____");
  }
  fprintf(stdout, " %3" PRId32 " %3" PRId32 " ",
             (rec->events >> RTEMS_CAPTURE_REAL_PRIORITY_EVENT) & 0xff,
             (rec->events >> RTEMS_CAPTURE_CURR_PRIORITY_EVENT) & 0xff);
   fprintf (stdout, "%3" PRId32   " %6" PRId32 "  TASK_RECORD\n",
            task_rec->start_priority,
            task_rec->stack_size);
}

void
rtems_capture_print_record_capture(
  uint32_t                cpu,
  rtems_capture_record_t* rec,
  uint64_t                diff
){
  uint32_t                     event;
  int                          e;

  event = rec->events >> RTEMS_CAPTURE_EVENT_START;
  for (e = RTEMS_CAPTURE_EVENT_START; e < RTEMS_CAPTURE_EVENT_END; e++)
  {
    if (event & 1)
    {
      fprintf(stdout,"%2" PRId32 " ", cpu);
      rtems_capture_print_timestamp (rec->time);
      fprintf (stdout, " %12" PRId32 " ", (uint32_t) diff);
      rtems_monitor_dump_id (rec->task_id);
      fprintf(stdout, "      %3" PRId32 " %3" PRId32 "             %s\n",
             (rec->events >> RTEMS_CAPTURE_REAL_PRIORITY_EVENT) & 0xff,
             (rec->events >> RTEMS_CAPTURE_CURR_PRIORITY_EVENT) & 0xff,
             rtems_capture_event_text (e));
    }
    event >>= 1;
  }
}

/*
 * rtems_capture_print_trace_records
 *
 * This function is a monitor command that dumps trace records.
 */

void
rtems_capture_print_trace_records ( int total, bool csv )
{
  rtems_status_code       sc;
  int                     count;
  ctrace_per_cpu_t*       per_cpu;
  uint8_t*                ptr;
  uint32_t                i;
  uint32_t                cpu = 0;
  rtems_capture_record_t* rec_out;

  count = rtems_get_processor_count();
  per_cpu = calloc( count, sizeof(*per_cpu) );

  while (total)
  {
    /* Prime the per_cpu data */
    for (i=0; i< count; i++) {
      if ( per_cpu[i].read == 0 ) {
        sc = rtems_capture_read (i, &per_cpu[i].read, &per_cpu[i].rec);
        if (sc != RTEMS_SUCCESSFUL)
        {
          fprintf (stdout, "error: trace read failed: %s\n", rtems_status_text (sc));
          rtems_capture_flush (0);
          free( per_cpu );
          return;
        }
        /* Release the buffer if there are no records to read */
        if (per_cpu[i].read == 0)
          rtems_capture_release (i, 0);
      }
    }

    /* Find the next record to print */
    rec_out = NULL;
    for (i=0; i< count; i++) {

      if ((rec_out == NULL) ||
          ((per_cpu[i].read != 0) && (rec_out->time > per_cpu[i].rec->time))) {
        rec_out = per_cpu[i].rec;
        cpu = i;
      }
    }

    /*  If we have read all the records abort. */
    if (rec_out == NULL)
      break;

    /* Print the record */
    if (csv) {
      fprintf(
        stdout,
        "%03" PRIu32 ",%08" PRIu32 ",%03" PRIu32
           ",%03" PRIu32 ",%04" PRIx32 ",%" PRId64 "\n",
        cpu,
        (uint32_t) rec_out->task_id,
        (rec_out->events >> RTEMS_CAPTURE_REAL_PRIORITY_EVENT) & 0xff,
        (rec_out->events >> RTEMS_CAPTURE_CURR_PRIORITY_EVENT) & 0xff,
        (rec_out->events >> RTEMS_CAPTURE_EVENT_START),
        (uint64_t) rec_out->time
      );
    } else {
      if ((rec_out->events >> RTEMS_CAPTURE_EVENT_START) == 0)
          rtems_capture_print_record_task(cpu, rec_out );
      else {
        uint64_t diff;
        if (per_cpu[cpu].last_t != 0)
          diff = rec_out->time - per_cpu[cpu].last_t;
        else
          diff = 0;
        per_cpu[cpu].last_t = rec_out->time;

        rtems_capture_print_record_capture( cpu, rec_out, diff );
      }
    }

    /*
     * If we have not printed all the records read
     * increment to the next record.  If we have
     * printed all records release the records printed.
     */
    per_cpu[cpu].printed++;
    if (per_cpu[cpu].printed != per_cpu[cpu].read) {
      ptr =  (uint8_t *)per_cpu[cpu].rec;
      ptr += per_cpu[cpu].rec->size;
      per_cpu[cpu].rec = (rtems_capture_record_t *)ptr;
    } else {
      rtems_capture_release (cpu, per_cpu[cpu].printed);
      per_cpu[cpu].read = 0;
      per_cpu[cpu].printed = 0;
    }

    total --;
  }

  /* Finished so release all the records that were printed. */
  for (i=0; i< count; i++) {
    if ( per_cpu[i].read != 0 )  {
      rtems_capture_release( i, per_cpu[i].printed );
    }
  }

  free( per_cpu );
}

void
rtems_capture_print_watch_list ()
{
  rtems_capture_control_t* control = rtems_capture_get_control_list ();
  rtems_task_priority      ceiling = rtems_capture_watch_get_ceiling ();
  rtems_task_priority      floor = rtems_capture_watch_get_floor ();

  fprintf (stdout, "watch priority ceiling is %" PRId32 "\n", ceiling);
  fprintf (stdout, "watch priority floor is %" PRId32 "\n", floor);
  fprintf (stdout, "global watch is %s\n",
          rtems_capture_watch_global_on () ? "enabled" : "disabled");
  fprintf (stdout, "total %" PRId32 "\n", rtems_capture_control_count ());

  while (control)
  {
    uint32_t flags;
    int      f;
    int      fshowed;
    int      lf;

    fprintf (stdout, " ");
    rtems_monitor_dump_id (rtems_capture_control_id (control));
    fprintf (stdout, " ");
    rtems_monitor_dump_name (rtems_capture_control_name (control));
    flags = rtems_capture_control_flags (control);
    fprintf (stdout, " %c%c ",
             rtems_capture_watch_global_on () ? 'g' : '-',
             flags & RTEMS_CAPTURE_WATCH ? 'w' : '-');
    flags = rtems_capture_control_to_triggers (control);
    fprintf (stdout, " T:%c%c%c%c%c%c%c",
             flags & RTEMS_CAPTURE_SWITCH    ? 'S' : '-',
             flags & RTEMS_CAPTURE_CREATE ? 'C' : '-',
             flags & RTEMS_CAPTURE_START ? 'S' : '-',
             flags & RTEMS_CAPTURE_RESTART ? 'R' : '-',
             flags & RTEMS_CAPTURE_DELETE ? 'D' : '-',
             flags & RTEMS_CAPTURE_BEGIN ? 'B' : '-',
             flags & RTEMS_CAPTURE_EXITTED ? 'E' : '-');
    flags = rtems_capture_control_from_triggers (control);
    fprintf (stdout, " F:%c%c%c%c%c",
             flags & RTEMS_CAPTURE_SWITCH  ? 'S' : '-',
             flags & RTEMS_CAPTURE_CREATE  ? 'C' : '-',
             flags & RTEMS_CAPTURE_START   ? 'S' : '-',
             flags & RTEMS_CAPTURE_RESTART ? 'R' : '-',
             flags & RTEMS_CAPTURE_DELETE  ? 'D' : '-');

    for (f = 0, fshowed = 0, lf = 1; f < RTEMS_CAPTURE_TRIGGER_TASKS; f++)
    {
      if (rtems_capture_control_by_valid (control, f))
      {
        if (lf && ((fshowed % 3) == 0))
        {
          fprintf (stdout, "\n");
          lf = 0;
        }

        fprintf (stdout, "  %2i:", f);
        rtems_monitor_dump_name (rtems_capture_control_by_name (control, f));
        fprintf (stdout, "/");
        rtems_monitor_dump_id (rtems_capture_control_by_id (control, f));
        flags = rtems_capture_control_by_triggers (control, f);
        fprintf (stdout, ":%c%c%c%c%c",
                 flags & RTEMS_CAPTURE_SWITCH  ? 'S' : '-',
                 flags & RTEMS_CAPTURE_CREATE  ? 'C' : '-',
                 flags & RTEMS_CAPTURE_START   ? 'S' : '-',
                 flags & RTEMS_CAPTURE_RESTART ? 'R' : '-',
                 flags & RTEMS_CAPTURE_DELETE  ? 'D' : '-');
        fshowed++;
        lf = 1;
      }
    }

    if (lf)
      fprintf (stdout, "\n");

    control = rtems_capture_next_control (control);
  }
}
