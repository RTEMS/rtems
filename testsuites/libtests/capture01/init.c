/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#define CONFIGURE_INIT

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "system.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include <rtems.h>
#include <rtems/capture-cli.h>
#include <rtems/monitor.h>
#include <rtems/shell.h>
#include <assert.h>

#define ASSERT_SC(sc) assert((sc) == RTEMS_SUCCESSFUL)

/* forward declarations to avoid warnings */
rtems_task Init(rtems_task_argument argument);

const char rtems_test_name[] = "CAPTURE 1";

static void cwlist(void);
static void ctrace(void);

static void cwlist ()
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

/*
 * rtems_catpure_cli_print_uptime
 *
 *  DESCRIPTION::148
 *
 *
 * This function prints the nanosecond uptime to stdout.
 */
static void
rtems_capture_cli_print_timestamp (uint64_t uptime)
{
  uint32_t hours;
  uint32_t minutes;
  uint32_t seconds;
  uint32_t nanosecs;

  seconds  = uptime / 1000000000LLU;
  minutes  = seconds / 60;
  hours    = minutes / 60;
  minutes  = minutes % 60;
  seconds  = seconds % 60;
  nanosecs = uptime % 1000000000;

  fprintf (stdout, "%5lu:%02lu:%02lu.%09lu", hours, minutes, seconds, nanosecs);
}
static void
rtems_caputre_cli_print_record_task(rtems_capture_record_t* rec)
{
  rtems_capture_task_record_t* task_rec = (rtems_capture_task_record_t*) rec;

  rtems_capture_cli_print_timestamp (rec->time);
  fprintf (stdout, "           ");
  rtems_monitor_dump_id (rec->task_id);
   fprintf (stdout, " %c%c%c%c",
            (char) (task_rec->name >> 24) & 0xff,
            (char) (task_rec->name >> 16) & 0xff,
            (char) (task_rec->name >> 8) & 0xff,
            (char) (task_rec->name >> 0) & 0xff);
   fprintf (stdout, " %3" PRId32   " %3" PRId32 "\n",
            task_rec->start_priority,
            task_rec->stack_size);
}

static void
rtems_caputure_cli_print_record_std(rtems_capture_record_t* rec, uint64_t diff)
{
  uint32_t                     event;
  int                          e;

  event = rec->events >> RTEMS_CAPTURE_EVENT_START;

  for (e = RTEMS_CAPTURE_EVENT_START; e < RTEMS_CAPTURE_EVENT_END; e++)
  {
    if (event & 1)
    {
      rtems_capture_cli_print_timestamp (rec->time);
      fprintf (stdout, " %9" PRId64 " ", diff);
      rtems_monitor_dump_id (rec->task_id);
      fprintf(stdout, "      %3" PRId32 " %3" PRId32 " %s\n",
             (rec->events >> RTEMS_CAPTURE_REAL_PRIORITY_EVENT) & 0xff,
             (rec->events >> RTEMS_CAPTURE_CURR_PRIORITY_EVENT) & 0xff,
             rtems_capture_event_text (e));
    }
    event >>= 1;
  }
}

static void ctrace()
{
  rtems_status_code       sc;
  bool                    csv = false;
  static int              dump_total = 22;
  int                     total;
  int                     count;
  uint32_t                read;
  rtems_capture_record_t* rec;
  uint8_t*                ptr;
  rtems_capture_time_t    last_t = 0;

  total = dump_total;

  while (total)
  {
    sc = rtems_capture_read (0, 0, &read, &rec);

    if (sc != RTEMS_SUCCESSFUL)
    {
      fprintf (stdout, "error: trace read failed: %s\n", rtems_status_text (sc));
      rtems_capture_flush (0);
      return;
    }

    /*
     * If we have no records then just exist. We still need to release
     * the reader lock.
     */

    if (read == 0)
    {
      rtems_capture_release (read);
      break;
    }

    count = total < read ? total : read;
    ptr = (uint8_t *) rec;
    while (count--)
    {
      rec = (rtems_capture_record_t*) ptr;

      if (csv)
        fprintf (stdout, "%08" PRIu32 ",%03" PRIu32
                   ",%03" PRIu32 ",%04" PRIx32 ",%" PRId64 "\n",
                 rec->task_id,
                 (rec->events >> RTEMS_CAPTURE_REAL_PRIORITY_EVENT) & 0xff,
                 (rec->events >> RTEMS_CAPTURE_CURR_PRIORITY_EVENT) & 0xff,
                 (rec->events >> RTEMS_CAPTURE_EVENT_START),
                 (uint64_t) rec->time);
      else {
        if ((rec->events >> RTEMS_CAPTURE_EVENT_START) == 0)
          rtems_caputre_cli_print_record_task( rec );
        else {
          uint64_t diff = 0;
          if (last_t)
            diff = rec->time - last_t;
          last_t = rec->time;

          rtems_caputure_cli_print_record_std( rec, diff );
        }
      }
      ptr += rec->size;
    }

    count = total < read ? total : read;

    if (count < total)
      total -= count;
    else
      total = 0;

    rtems_capture_release (count);
  }
}

rtems_task Init(
  rtems_task_argument ignored
)
{
#if BSP_SMALL_MEMORY
  printf("NO Capture Engine. MEMORY TOO SMALL");
#else
  rtems_status_code   sc;
  rtems_task_priority old_priority;
  rtems_mode          old_mode;
  rtems_name          to_name = rtems_build_name('I', 'D', 'L', 'E');;

  rtems_test_begin();

#ifdef RTEMS_SMP
  printf("Capture Engine is broken due to recursive interrupt lock usage\n");
  assert(0);
#endif

  rtems_task_set_priority(RTEMS_SELF, 20, &old_priority);
  rtems_task_mode(RTEMS_PREEMPT,  RTEMS_PREEMPT_MASK, &old_mode);

  sc = rtems_capture_open (5000, NULL);
  ASSERT_SC(sc);

  sc = rtems_capture_watch_ceiling (100);
  ASSERT_SC(sc);

  sc = rtems_capture_watch_floor (102);
  ASSERT_SC(sc);

  sc = rtems_capture_watch_global (true);
  ASSERT_SC(sc);

  sc = rtems_capture_set_trigger (
    0, 
    0, 
    to_name, 
    0, 
    rtems_capture_from_any, 
    rtems_capture_switch
  );
  ASSERT_SC(sc);

  cwlist();

  sc = rtems_capture_control (true);
  ASSERT_SC(sc);

  capture_test_1();
  
  ctrace();
  ctrace();

  rtems_test_end();
  exit( 0 );

#endif
}
