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

typedef struct
{
  const void*          recs;      /**< Next record to be read. */
  size_t               read;      /**< Number of records read. */
  size_t               printed;   /**< Records been printed. */
  bool                 rec_valid; /**< The record is valid. */
  rtems_capture_record rec;       /**< The record, copied out. */
} ctrace_per_cpu;

/*
 * Task block size.
 */
#define CTRACE_TASK_BLOCK_SIZE (64)

/**
 * Task details from the task records used to print task names.
 */
typedef struct
{
  rtems_name name;
  rtems_id   id;
} ctrace_task_name;

/**
 * Structure to hold the tasks variables.
 */
typedef struct
{
  ctrace_task_name* tasks;
  size_t            size;
  size_t            count;
} ctrace_tasks;

/*
 * Global so the records can span more than one trace.
 */
static ctrace_tasks tasks;

/*
 * Add a name.
 */
static void
ctrace_task_name_add (rtems_id id, const rtems_name* name)
{
  if (tasks.tasks == NULL)
  {
    tasks.size = CTRACE_TASK_BLOCK_SIZE;
    tasks.tasks = calloc (tasks.size, sizeof (ctrace_task_name));
  }
  if (tasks.tasks != NULL)
  {
    if (rtems_object_id_get_api(id) != OBJECTS_POSIX_API)
    {
      size_t t;
      for (t = 0; t < tasks.count; ++t)
      {
        if (tasks.tasks[t].id == id)
        {
          tasks.tasks[t].name = *name;
          break;
        }
      }
      if (t == tasks.count)
      {
        if (tasks.count >= tasks.size)
        {
          tasks.size += CTRACE_TASK_BLOCK_SIZE;
          tasks.tasks = realloc (tasks.tasks,
                                 tasks.size * sizeof (ctrace_task_name));
        }
        if (tasks.tasks != NULL)
        {
          tasks.tasks[tasks.count].name = *name;
          tasks.tasks[tasks.count].id = id;
          ++tasks.count;
        }
      }
    }
  }
}

/*
 * Remove a task name.
 */
static void
ctrace_task_name_remove (rtems_id id)
{
  size_t t;
  for (t = 0; t < tasks.count; ++t)
  {
    if (tasks.tasks[t].id == id)
    {
      size_t count = tasks.count - t - 1;
      if (count != 0)
        memmove (&tasks.tasks[t],
                 &tasks.tasks[t + 1],
                 sizeof (ctrace_task_name) * count);
      --tasks.count;
      break;
    }
  }
}

/*
 * Find a name.
 */
static void
ctrace_task_name_find (rtems_id id, const rtems_name** name)
{
  size_t t;
  *name = NULL;
  for (t = 0; t < tasks.count; ++t)
  {
    if (tasks.tasks[t].id == id)
    {
      *name = &tasks.tasks[t].name;
      break;
    }
  }
}

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
rtems_capture_print_record_task (int                              cpu,
                                 const rtems_capture_record*      rec,
                                 const rtems_capture_task_record* task_rec)
{
  fprintf(stdout,"%2i ", cpu);
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
rtems_capture_print_record_capture(int                         cpu,
                                   const rtems_capture_record* rec,
                                   uint64_t                    diff,
                                   const rtems_name*           name)
{
  uint32_t event;
  int      e;

  event = rec->events >> RTEMS_CAPTURE_EVENT_START;
  for (e = RTEMS_CAPTURE_EVENT_START; e < RTEMS_CAPTURE_EVENT_END; e++)
  {
    if (event & 1)
    {
      fprintf(stdout,"%2i ", cpu);
      rtems_capture_print_timestamp (rec->time);
      fprintf (stdout, " %12" PRId32 " ", (uint32_t) diff);
      rtems_monitor_dump_id (rec->task_id);
      if (name != NULL)
      {
        fprintf (stdout, " %c%c%c%c",
                 (char) (*name >> 24) & 0xff,
                 (char) (*name >> 16) & 0xff,
                 (char) (*name >> 8) & 0xff,
                 (char) (*name >> 0) & 0xff);
      }
      else
      {
        fprintf(stdout, "     ");
      }
      fprintf(stdout, " %3" PRId32 " %3" PRId32 "             %s\n",
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
rtems_capture_print_trace_records (int total, bool csv)
{
  ctrace_per_cpu*    per_cpu;
  ctrace_per_cpu*    cpu;
  int                cpus;
  rtems_capture_time last_time = 0;
  int                i;

  cpus = rtems_scheduler_get_processor_maximum ();

  per_cpu = calloc (cpus, sizeof(*per_cpu));
  if (per_cpu == NULL)
  {
    fprintf(stdout, "error: no memory\n");
    return;
  }

  while (total)
  {
    const rtems_capture_record* rec_out = NULL;
    int                         cpu_out = -1;
    rtems_capture_time          this_time = 0;

    /* Prime the per_cpu data */
    for (i = 0; i < cpus; i++) {
      cpu = &per_cpu[i];

      if (cpu->read == 0)
      {
        rtems_status_code sc;
        sc = rtems_capture_read (i, &cpu->read, &cpu->recs);
        if (sc != RTEMS_SUCCESSFUL)
        {
          fprintf (stdout,
                   "error: trace read failed: %s\n", rtems_status_text (sc));
          rtems_capture_flush (0);
          free (per_cpu);
          return;
        }
        /* Release the buffer if there are no records to read */
        if (cpu->read == 0)
          rtems_capture_release (i, 0);
      }

      /* Read the record out from the capture buffer */
      if (!cpu->rec_valid && (cpu->read != 0))
      {
        cpu->recs = rtems_capture_record_extract (cpu->recs,
                                                  &cpu->rec,
                                                  sizeof (cpu->rec));
        cpu->rec_valid = true;
      }

      /* Find the next record to print, the earliest recond on any core */
      if ((cpu->rec_valid) && ((this_time == 0) || (cpu->rec.time < this_time)))
      {
        rec_out = &cpu->rec;
        cpu_out = i;
        this_time = rec_out->time;
      }
    }

    /*  If we have read all the records abort. */
    if (rec_out == NULL)
      break;

    cpu = &per_cpu[cpu_out];

    /* Print the record */
    if (csv)
    {
      fprintf(stdout,
              "%03i,%08" PRIu32 ",%03" PRIu32
              ",%03" PRIu32 ",%04" PRIx32 ",%" PRId64 "\n",
              cpu_out,
              (uint32_t) rec_out->task_id,
              (rec_out->events >> RTEMS_CAPTURE_REAL_PRIORITY_EVENT) & 0xff,
              (rec_out->events >> RTEMS_CAPTURE_CURR_PRIORITY_EVENT) & 0xff,
              (rec_out->events >> RTEMS_CAPTURE_EVENT_START),
              (uint64_t) rec_out->time);
    }
    else
    {
      if ((rec_out->events >> RTEMS_CAPTURE_EVENT_START) == 0)
      {
        rtems_capture_task_record task_rec;
        cpu->recs = rtems_capture_record_extract (cpu->recs,
                                                  &task_rec,
                                                  sizeof (task_rec));
        ctrace_task_name_add (rec_out->task_id, &task_rec.name);
        rtems_capture_print_record_task (cpu_out, rec_out, &task_rec);
      }
      else
      {
        rtems_capture_time diff;
        const rtems_name*  name = NULL;
        if (last_time != 0)
          diff = rec_out->time - last_time;
        else
          diff = 0;
        last_time = rec_out->time;
        ctrace_task_name_find (rec_out->task_id, &name);
        rtems_capture_print_record_capture (cpu_out, rec_out, diff, name);
        if ((rec_out->events &
             (RTEMS_CAPTURE_DELETED_BY_EVENT | RTEMS_CAPTURE_DELETED_EVENT)) != 0)
          ctrace_task_name_remove (rec_out->task_id);
      }
    }

    /*
     * If we have not printed all the records read increment to the next
     * record. If we have printed all records release the records printed.
     */
    cpu->rec_valid = false;
    ++cpu->printed;
    if (cpu->printed == cpu->read)
    {
      rtems_capture_release (cpu_out, cpu->printed);
      cpu->recs = NULL;
      cpu->read = 0;
      cpu->printed = 0;
    }

    --total;
  }

  /* Finished so release all the records that were printed. */
  for (i = 0; i < cpus; i++)
  {
    cpu = &per_cpu[i];
    if (cpu->read != 0)
    {
      rtems_capture_release (i, cpu->printed);
    }
  }

  free(per_cpu);
}

void
rtems_capture_print_watch_list (void)
{
  rtems_capture_control* control = rtems_capture_get_control_list ();
  rtems_task_priority    ceiling = rtems_capture_watch_get_ceiling ();
  rtems_task_priority    floor = rtems_capture_watch_get_floor ();

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
