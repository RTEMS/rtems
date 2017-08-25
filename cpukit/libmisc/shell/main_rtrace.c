/*
 *  Copyright (c) 2015 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <rtems/shell.h>
#include <rtems/trace/rtems-trace-buffer-vars.h>

/**
 * The type of the shell handlers we have.
 */
typedef int (*rtems_trace_buffering_shell_handler_t) (int argc, char *argv[]);

/**
 * Table of handlers we parse to invoke the command.
 */
typedef struct
{
  const char*                           name;    /**< The sub-command's name. */
  rtems_trace_buffering_shell_handler_t handler; /**< The sub-command's handler. */
  const char*                           help;    /**< The sub-command's help. */
} rtems_trace_buffering_shell_cmd_t;

static int
rtems_trace_buffering_wrong_number_of_args (void)
{
  printf ("error: wrong number of arguments\n");
  return 1;
}

static int
rtems_trace_buffering_no_trace_buffer_code (void)
{
  printf("No trace buffer generated code in the application; see rtems-tld\n");
  return 1;
}

static void
rtems_trace_buffering_banner (const char* label)
{
  printf("RTEMS Trace Bufferring: %s\n", label);
}

static void
rtems_trace_buffering_print_timestamp (uint64_t uptime)
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

  printf ("%5" PRIu32 ":%02" PRIu32 ":%02" PRIu32".%09" PRIu32,
          hours, minutes, seconds, nanosecs);
}

static int
rtems_trace_buffering_shell_status (int argc, char *argv[])
{
  uint32_t buffer_size;
  uint32_t buffer_in;
  bool     finished;
  bool     triggered;
  uint32_t names;

  if (argc != 1)
    return rtems_trace_buffering_wrong_number_of_args ();

  if (!rtems_trace_buffering_present ())
    return rtems_trace_buffering_no_trace_buffer_code ();

  buffer_size = rtems_trace_buffering_buffer_size ();
  buffer_in = rtems_trace_buffering_buffer_in ();
  finished = rtems_trace_buffering_finished ();
  triggered = rtems_trace_buffering_triggered ();
  names = rtems_trace_names_size ();

  rtems_trace_buffering_banner ("status");
  printf("    Running:  %s\n", finished ? "no" : "yes");
  printf("  Triggered:  %s\n", triggered ? "yes" : "no");
  printf("      Level: %3" PRIu32 "%%\n", (buffer_in * 100) / buffer_size);
  printf("     Traces: %4" PRIu32 "\n", names);

  return 0;
}

static int
rtems_trace_buffering_shell_funcs (int argc, char *argv[])
{
  size_t traces = rtems_trace_names_size ();
  size_t t;
  size_t max = 0;

  if (argc != 1)
    return rtems_trace_buffering_wrong_number_of_args ();

  if (!rtems_trace_buffering_present ())
    return rtems_trace_buffering_no_trace_buffer_code ();

  rtems_trace_buffering_banner ("trace functions");
  printf(" Total: %4zu\n", traces);

  for (t = 0; t < traces; ++t)
  {
    size_t l = strlen (rtems_trace_names (t));
    if (l > max)
      max = l;
  }

  for (t = 0; t < traces; ++t)
  {
    printf(" %4zu: %c%c %-*s\n", t,
           rtems_trace_enable_set(t) ? 'E' : '-',
           rtems_trace_trigger_set(t) ? 'T' : '-',
           (int) max, rtems_trace_names (t));
  }

  return 0;
}

static int
rtems_trace_buffering_shell_start (int argc, char *argv[])
{
  if (argc != 1)
    return rtems_trace_buffering_wrong_number_of_args ();

  if (!rtems_trace_buffering_present ())
    return rtems_trace_buffering_no_trace_buffer_code ();

  rtems_trace_buffering_banner ("resume");

  if (!rtems_trace_buffering_finished ())
  {
    printf("already running\n");
    return 0;
  }

  rtems_trace_buffering_start ();

  return 0;
}

static int
rtems_trace_buffering_shell_stop (int argc, char *argv[])
{
  if (argc != 1)
    return rtems_trace_buffering_wrong_number_of_args ();

  if (!rtems_trace_buffering_present ())
    return rtems_trace_buffering_no_trace_buffer_code ();

  rtems_trace_buffering_banner ("stop");

  if (rtems_trace_buffering_finished ())
  {
    printf("already stopped\n");
    return 0;
  }

  rtems_trace_buffering_stop ();

  return 0;
}

static int
rtems_trace_buffering_shell_resume (int argc, char *argv[])
{
  if (argc != 1)
    return rtems_trace_buffering_wrong_number_of_args ();

  if (!rtems_trace_buffering_present ())
    return rtems_trace_buffering_no_trace_buffer_code ();

  rtems_trace_buffering_banner ("resume");

  if (!rtems_trace_buffering_finished ())
  {
    printf("already running\n");
    return 0;
  }

  rtems_trace_buffering_start ();

  return 0;
}

static void rtems_trace_buffering_print_arg (const rtems_trace_sig_arg* arg,
                                             const uint8_t*             argv)
{
  if (arg->size)
  {
    union
    {
      uint8_t  bytes[sizeof (uint64_t)];
      uint16_t u16;
      uint32_t u32;
      uint64_t u64;
      void*    pointer;
    } variable;

    if (arg->size <= sizeof(uint64_t))
      memcpy (&variable.bytes[0], argv, arg->size);

    printf ("(%s) ", arg->type);

    if (strchr (arg->type, '*') != NULL)
    {
      printf ("%p", variable.pointer);
    }
    else
    {
      size_t b;
      switch (arg->size)
      {
        case 2:
          printf ("%04" PRIx16, variable.u16);
          break;
        case 4:
          printf ("%08" PRIx32, variable.u32);
          break;
        case 8:
          printf ("%016" PRIx64, variable.u64);
          break;
        default:
          for (b = 0; b < arg->size; ++b)
            printf ("%02" PRIx32, (uint32_t) *argv++);
          break;
      }
    }
  }
}

static int
rtems_trace_buffering_shell_trace (int argc, char *argv[])
{
  uint32_t* trace_buffer;
  uint32_t  records;
  uint32_t  traces;
  uint32_t  r;
  size_t    start = 0;
  size_t    end = 40;
  size_t    count;
  uint64_t  last_sample = 0;

  if (!rtems_trace_buffering_present ())
    return rtems_trace_buffering_no_trace_buffer_code ();

  trace_buffer = rtems_trace_buffering_buffer ();
  records = rtems_trace_buffering_buffer_in ();
  traces = rtems_trace_names_size ();

  if (argc > 1)
  {
    if (argc > 3)
      return rtems_trace_buffering_wrong_number_of_args ();

    if (argv[1][0] == '+')
    {
      if (argc > 2)
        return rtems_trace_buffering_wrong_number_of_args ();
      end = strtoul (argv[1] + 1, 0, 0);
      if (end == 0)
      {
        printf("error: invalid number of lines\n");
        return 1;
      }
      ++end;
    }
    else
    {
      start = strtoul (argv[1], 0, 0);
      if (start >= records)
      {
        printf ("error: start record out of range (max %" PRIu32 ")\n", records);
        return 1;
      }
    }

    end += start;

    if (argc == 3)
    {
      if (argv[2][0] == '+')
      {
        end = strtoul (argv[2] + 1, 0, 0);
        if (end == 0)
        {
          printf("error: invalid number of lines\n");
          return 1;
        }
        end += start + 1;
      }
      else
      {
        end = strtoul (argv[2], 0, 0);
        if (end < start)
        {
          printf ("error: end record before start\n");
          return 1;
        }
        else if (end > records)
        {
          printf ("error: end record out of range (max %" PRIu32 ")\n", records);
        }
      }
    }
  }

  rtems_trace_buffering_banner ("trace");

  if (!rtems_trace_buffering_finished ())
  {
    printf("tracing still running\n");
    return 0;
  }

  printf(" Trace buffer: %p\n", trace_buffer);
  printf(" Words traced: %" PRIu32 "\n", records);
  printf("       Traces: %" PRIu32 "\n", traces);

  count = 0;
  r = 0;

  while ((r < records) && (count < end))
  {
    const uint32_t header = trace_buffer[r];
    const uint32_t func_index = header & 0xffff;
    const uint32_t len = (header >> 16) & 0x0fff;
    const uint32_t task_id = trace_buffer[r + 1];
    const uint32_t task_status = trace_buffer[r + 2];
    const uint64_t when = (((uint64_t) trace_buffer[r + 4]) << 32) | trace_buffer[r + 5];
    const uint8_t* argv = (uint8_t*) &trace_buffer[r + 6];
    const bool     ret = (header & (1 << 30)) == 0 ? false : true;
    const bool     irq = (header & (1 << 31)) == 0 ? false : true;

    if (count > start)
    {
      const rtems_trace_sig* sig = rtems_trace_signatures (func_index);

      rtems_trace_buffering_print_timestamp (when);
      printf (" %10" PRIu32 " %c%08" PRIx32 " [%3" PRIu32 "/%3" PRIu32 "] %c %s",
              (uint32_t) (when - last_sample),
              irq ? '*' : ' ', task_id, (task_status >> 8) & 0xff, task_status & 0xff,
              ret ? '<' : '>', rtems_trace_names (func_index));

      if (sig->argc)
      {
        if (ret)
        {
          if (sig->args[0].size)
            printf(" => ");
          rtems_trace_buffering_print_arg (&sig->args[0], argv);
        }
        else
        {
          size_t a;
          printf("(");
          for (a = 1; a < sig->argc; ++a)
          {
            if (a > 1)
              printf (", ");
            rtems_trace_buffering_print_arg (&sig->args[a], argv);
            argv += sig->args[a].size;
          }
          printf(")");
        }
      }

      printf("\n");
    }

    r += ((len - 1) / sizeof (uint32_t)) + 1;
    last_sample = when;
    ++count;
  }

  return 0;
}

static ssize_t
rtems_trace_buffering_file_write (int out, const void* vbuffer, ssize_t length)
{
  const uint8_t* buffer = vbuffer;
  while (length)
  {
    ssize_t w = write (out, buffer, length);
    if (w < 0)
    {
      printf ("error: write failed: %s\n", strerror(errno));
      return false;
    }
    if (w == 0)
    {
      printf ("error: write failed: EOF\n");
      return false;
    }

    length -= w;
    buffer += w;
  }
  return true;
}

static int
rtems_trace_buffering_shell_save (int argc, char *argv[])
{
  uint32_t* trace_buffer;
  uint32_t  records;
  uint32_t  traces;
  uint8_t*  buffer;
  size_t    length;
  uint32_t  r;
  int       out;
  uint8_t*  buf;
  uint8_t*  in;

  if (argc != 2)
    return rtems_trace_buffering_wrong_number_of_args ();

  if (!rtems_trace_buffering_present ())
    return rtems_trace_buffering_no_trace_buffer_code ();

  rtems_trace_buffering_banner ("trace");

  if (!rtems_trace_buffering_finished ())
  {
    printf("tracing still running\n");
    return 0;
  }

  trace_buffer = rtems_trace_buffering_buffer ();
  records = rtems_trace_buffering_buffer_in ();
  traces = rtems_trace_names_size ();

  printf("   Trace File: %s\n", argv[1]);
  printf(" Trace buffer: %p\n", trace_buffer);
  printf(" Words traced: %" PRIu32 "\n", records);
  printf("       Traces: %" PRIu32 "\n", traces);

  out = open (argv[1], O_WRONLY | O_TRUNC | O_CREAT,
              S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
  if (out < 0)
  {
    printf ("error: opening file: %s: %s\n", argv[1], strerror(errno));
    return 1;
  }

  #define SAVE_BUF_SIZE (1024)

  buf = malloc(SAVE_BUF_SIZE);
  if (!buf)
  {
    close (out);
    printf ("error: no memory\n");
  }

  memset (buf, 0, SAVE_BUF_SIZE);

  in = buf;

  /*
   * Header label.
   */
  memcpy (in, "RTEMS-TRACE", sizeof("RTEMS-TRACE"));
  in += 12;

  /*
   * Endian detection.
   */
  *((uint32_t*) in) = 0x11223344;
  in += sizeof(uint32_t);

  /*
   * Number of traces.
   */
  *((uint32_t*) in) = traces;
  in += sizeof(uint32_t);

  /*
   * Write it.
   */
  if (!rtems_trace_buffering_file_write (out, buf, in - buf))
  {
    free (buf);
    close (out);
    return 1;
  }

  /*
   * The trace names.
   */
  for (r = 0; r < traces; ++r)
  {
    const char* name = rtems_trace_names (r);
    if (!rtems_trace_buffering_file_write (out, name, strlen (name) + 1))
    {
      free (buf);
      close (out);
      return 1;
    }
  }

  /*
   * The trace signatures.
   */
  for (r = 0; r < traces; ++r)
  {
    const rtems_trace_sig* sig = rtems_trace_signatures (r);
    size_t                 s;

    in = buf;

    memcpy (in, &sig->argc, sizeof (sig->argc));
    in += sizeof(uint32_t);

    for (s = 0; s < sig->argc; ++s)
    {
      const rtems_trace_sig_arg* arg = &sig->args[s];
      size_t                     arg_len = strlen (arg->type) + 1;

      if ((in - buf) > SAVE_BUF_SIZE)
      {
        printf ("error: save temp buffer to small\n");
        free (buf);
        close (out);
        return 1;
      }

      memcpy (in, &arg->size, sizeof (arg->size));
      in += sizeof(uint32_t);
      memcpy (in, arg->type, arg_len);
      in += arg_len;
    }

    if (!rtems_trace_buffering_file_write (out, buf, in - buf))
    {
      free (buf);
      close (out);
      return 1;
    }
  }

  free (buf);

  buffer = (uint8_t*) trace_buffer;
  length = records * sizeof (uint32_t);

  while (length)
  {
    ssize_t w = write (out, buffer, length);
    if (w < 0)
    {
      printf ("error: write failed: %s\n", strerror(errno));
      close (out);
      return 1;
    }
    if (w == 0)
    {
      printf ("error: write failed: EOF\n");
      close (out);
      return 1;
    }

    length -= w;
    buffer += w;
  }

  close (out);

  return 0;
}

static void
rtems_trace_buffering_shell_usage (const char* arg)
{
  printf ("%s: Trace Buffer Help\n", arg);
  printf ("  %s [-hl] <command>\n", arg);
  printf ("   where:\n");
  printf ("     command: The TBG subcommand. See -l for a list plus help.\n");
  printf ("     -h:      This help\n");
  printf ("     -l:      The command list.\n");
}

static const rtems_trace_buffering_shell_cmd_t table[] =
{
  {
    "status",
    rtems_trace_buffering_shell_status,
    "                       : Show the current status"
  },
  {
    "funcs",
    rtems_trace_buffering_shell_funcs,
    "                       : List the trace functions"
  },
  {
    "start",
    rtems_trace_buffering_shell_start,
    "                       : Start or restart tracing"
  },
  {
    "stop",
    rtems_trace_buffering_shell_stop,
    "                       : Stop tracing"
  },
  {
    "resume",
    rtems_trace_buffering_shell_resume,
    "                       : Resume tracing."
  },
  {
    "trace",
    rtems_trace_buffering_shell_trace,
    " [start] [end/+length] : List the current trace records"
  },
  {
    "save",
    rtems_trace_buffering_shell_save,
    " file                  : Save the trace buffer to a file"
  },
};

#define RTEMS_TRACE_BUFFERING_COMMANDS \
  (sizeof (table) / sizeof (const rtems_trace_buffering_shell_cmd_t))

static int
rtems_shell_main_rtrace (int argc, char* argv[])
{
  int    arg;
  size_t t;

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] != '-')
      break;

    switch (argv[arg][1])
    {
      case 'h':
        rtems_trace_buffering_shell_usage (argv[0]);
        return 0;
      case 'l':
        printf ("%s: commands are:\n", argv[0]);
        for (t = 0; t < RTEMS_TRACE_BUFFERING_COMMANDS; ++t)
          printf ("  %-7s %s\n", table[t].name, table[t].help);
        return 0;
      default:
        printf ("error: unknown option: %s\n", argv[arg]);
        return 1;
    }
  }

  if ((argc - arg) < 1)
    printf ("error: you need to provide a command, try %s -h\n", argv[0]);
  else
  {
    for (t = 0; t < RTEMS_TRACE_BUFFERING_COMMANDS; ++t)
    {
      if (strncmp (argv[arg], table[t].name, strlen (argv[arg])) == 0)
      {
        int r = table[t].handler (argc - arg, argv + 1);
        return r;
      }
    }
    printf ("error: command not found: %s (try -h)\n", argv[arg]);
  }

  return 1;
}

rtems_shell_cmd_t rtems_shell_RTRACE_Command = {
  "rtrace",                      /* name */
  "rtrace [-l]",                 /* usage */
  "misc",                        /* topic */
  rtems_shell_main_rtrace,       /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};
