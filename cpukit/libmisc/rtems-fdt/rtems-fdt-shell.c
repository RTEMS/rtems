/*
 *  COPYRIGHT (c) 2013-2017 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_fdt
 *
 * @brief RTEMS Flattened Device Tree Shell Command
 *
 * Command to play with the memory in a FDT.
 */

#include <ctype.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <rtems/shell.h>
#include <rtems/rtems-fdt-shell.h>

/**
 * The type of the shell handlers we have.
 */
typedef int (*rtems_fdt_shell_handler) (int argc, char *argv[]);

/**
 * Table of handlers we parse to invoke the command.
 */
typedef struct
{
  const char*             name;    /**< The sub-command's name. */
  rtems_fdt_shell_handler handler; /**< The sub-command's handler. */
  const char*             help;    /**< The sub-command's help. */
} rtems_fdt_shell_cmd;

/**
 * The timeout for the test loop in seconds.
 */
static long rtems_fdt_test_timeout = 5;

/**
 * The FDT handle. Only one user of these command a time.
 */
static rtems_fdt_handle cmd_fdt_handle;

static void
rtems_fdt_write (uint32_t address, uint32_t value)
{
  volatile uint32_t* ap = (uint32_t*) address;
  *ap = value;
}

static uint32_t
rtems_fdt_read (uint32_t address)
{
  volatile uint32_t* ap = (uint32_t*) address;
  return *ap;
}

static int
rtems_fdt_wrong_number_of_args (void)
{
  printf ("error: wrong number of arguments\n");
  return 1;
}

static int
rtems_fdt_invalid_args (const char* arg)
{
  printf ("error: invalid argument: %s\n", arg);
  return 1;
}

static int
rtems_fdt_extra_args (const char* arg)
{
  printf ("error: extra argument is invalid: %s\n", arg);
  return 1;
}

static int
rtems_fdt_check_error (int errval, const char* message, const char* path)
{
  if (errval < 0)
  {
    if (path)
      printf ("error: %s: %s: (%d) %s\n",
              message, path, errval, rtems_fdt_strerror (errval));
    else
      printf ("error: %s: (%d) %s\n",
              message, errval, rtems_fdt_strerror (errval));
    return 1;
  }
  return 0;
}

static bool
rtems_fdt_get_value32 (const char* path,
                       const char* property,
                       size_t      size,
                       uint32_t*   value)
{
  const void* prop;
  int         node;
  int         length;

  node = rtems_fdt_path_offset(&cmd_fdt_handle, path);
  if (node < 0)
  {
    rtems_fdt_check_error (node, "path lookup", path);
    return false;
  }

  prop = rtems_fdt_getprop(&cmd_fdt_handle, node, property, &length);
  if (length < 0)
  {
    rtems_fdt_check_error (length, "get property", path);
    return false;
  }

  if (length != sizeof (uint32_t))
  {
    printf ("error: property is not sizeof(uint32_t): %s\n", path);
    return false;
  }

  *value = rtems_fdt_get_uint32 (prop);

  return true;
}

static int
rtems_fdt_shell_ld (int argc, char *argv[])
{
  if (argc != 2)
    return rtems_fdt_wrong_number_of_args ();

  return rtems_fdt_check_error (rtems_fdt_load (argv[1], &cmd_fdt_handle),
                                "loading FTB", argv[1]);
}

static int
rtems_fdt_shell_uld (int argc, char *argv[])
{
  if (argc != 2)
    return rtems_fdt_wrong_number_of_args ();

  return rtems_fdt_check_error (rtems_fdt_unload (&cmd_fdt_handle),
                                "unloading FTB", argv[1]);
}

static int
rtems_fdt_shell_ls (int argc, char *argv[])
{
  char*  path = NULL;
  bool   recursive = false;
  bool   long_path = false;
  bool   debug = false;
  int    arg = 1;
  size_t path_len = 0;
  int    num_entries = 0;
  int    i = 0;

  while (arg < argc)
  {
    if (argv[arg][0] == '-')
    {
      if (argv[arg][2] != 0)
        return rtems_fdt_invalid_args (argv[arg]);

      switch (argv[arg][1])
      {
        case 'l':
          long_path = true;
          break;
        case 'r':
          recursive = true;
          break;
        case 'd':
          debug = true;
          break;
        default:
          return rtems_fdt_invalid_args (argv[arg]);
      }
    }
    else
    {
      if (path)
        return rtems_fdt_extra_args (argv[arg]);
      if (strcmp (argv[arg], "/") != 0)
        path = argv[arg];
    }
    ++arg;
  }

  if (!path)
  {
    path = "";
  }

  /* Eliminate trailing slashes. */
  path_len = strlen (path);

  if (path_len > 0 && path[path_len - 1] == '/')
      path_len--;

  /* Loop through the entries, looking for matches. */
  num_entries = rtems_fdt_num_entries(&cmd_fdt_handle);
  printf("Total: %d\n", num_entries);
  for (i = 0; i < num_entries; i++)
  {
    /* Add it to the result set. */
    const char *name = rtems_fdt_entry_name(&cmd_fdt_handle, i);
    size_t name_len = strlen(name);

    if ((name_len > path_len) &&
        ((strncmp (path, name, path_len) == 0) && (name[path_len] == '/')) &&
        (recursive || (index(&name[path_len+1], '/') == 0)))
    {
      if (long_path)
      {
        printf ("%s", name);
      }
      else if (name_len != path_len)
      {
        printf ("%s", &name[path_len + 1]);
      }

      if (debug)
      {
        /* Get properties if we're in debug mode. */
        int proplen = 0;
        int offset = rtems_fdt_entry_offset(&cmd_fdt_handle, i);
        const void *prop = rtems_fdt_getprop(&cmd_fdt_handle, offset, "reg", &proplen);
        const void *prop2 = rtems_fdt_getprop(&cmd_fdt_handle, offset, "mask", &proplen);

        if (prop)
        {
            printf(" addr 0x%08" PRIx32, *(uint32_t *)prop);
        }

        proplen = 0;
        if (prop2)
        {
            printf(" mask 0x%08" PRIx32, *(uint32_t *)prop2);
        }
      }

      printf("\n");
    }
  }

  return 0;
}

static int
rtems_fdt_shell_wr (int argc, char *argv[])
{
  uint32_t address;
  uint32_t offset = 0;
  uint32_t value;

  if ((argc < 3) || (argc > 4))
    return rtems_fdt_wrong_number_of_args ();

  if (argc == 3)
  {
    value = strtoul (argv[2], 0, 0);
  }
  else
  {
    offset = strtoul (argv[2], 0, 0);
    value = strtoul (argv[3], 0, 0);
  }

  if (!rtems_fdt_get_value32 (argv[1], "reg", sizeof (uint32_t), &address))
    return 1;

  address += offset;

  printf ("0x%08" PRIx32 " <= 0x%08" PRIx32 "\n", address, value);

  rtems_fdt_write (address, value);

  return 0;
}

static int
rtems_fdt_shell_rd (int argc, char *argv[])
{
  uint32_t address;
  uint32_t offset = 0;

  if ((argc < 1) || (argc > 3))
    return rtems_fdt_wrong_number_of_args ();

  if (argc == 3)
    offset = strtoul (argv[2], 0, 0);

  if (!rtems_fdt_get_value32 (argv[1], "reg", sizeof (uint32_t), &address))
    return 1;

  address += offset;

  printf ("0x%08" PRIx32 " => 0x%08" PRIx32 "\n", address, rtems_fdt_read (address));

  return 0;
}

static int
rtems_fdt_shell_set (int argc, char *argv[])
{
  uint32_t address;
  uint32_t offset = 0;
  uint32_t value;
  int      mask_arg;
  uint32_t mask;

  if ((argc < 3) || (argc > 4))
    return rtems_fdt_wrong_number_of_args ();

  if (argc == 3)
    mask_arg = 2;
  else
  {
    offset = strtoul (argv[2], 0, 0);
    mask_arg = 3;
  }

  if (!rtems_fdt_get_value32 (argv[1], "reg", sizeof (uint32_t), &address))
    return 1;

  if (isdigit (argv[mask_arg][0]))
    mask = strtoul (argv[mask_arg], 0, 0);
  else
  {
    if (!rtems_fdt_get_value32 (argv[mask_arg], "mask", sizeof (uint32_t), &mask))
      return 1;
  }

  address += offset;
  value = rtems_fdt_read (address);

  printf ("0x%08" PRIx32 " <= 0x%08" PRIx32 " = 0x%08" PRIx32 " | 0x%08" PRIx32 "\n",
          address, value | mask, value, mask);

  rtems_fdt_write (address, value | mask);

  return 0;
}

static int
rtems_fdt_shell_cl (int argc, char *argv[])
{
  uint32_t address;
  uint32_t offset = 0;
  uint32_t value;
  int      mask_arg;
  uint32_t mask;

  if ((argc < 3) || (argc > 4))
    return rtems_fdt_wrong_number_of_args ();

  if (argc == 3)
    mask_arg = 2;
  else
  {
    offset = strtoul (argv[2], 0, 0);
    mask_arg = 3;
  }

  if (!rtems_fdt_get_value32 (argv[1], "reg", sizeof (uint32_t), &address))
    return 1;

  if (isdigit (argv[mask_arg][0]))
    mask = strtoul (argv[mask_arg], 0, 0);
  else
  {
    if (!rtems_fdt_get_value32 (argv[mask_arg], "mask", sizeof (uint32_t), &mask))
      return 1;
  }

  address += offset;
  value = rtems_fdt_read (address);

  printf ("0x%08" PRIx32 " <= 0x%08" PRIx32 " = 0x%08" PRIx32 \
          " & ~0x%08" PRIx32 " (0x%08" PRIx32 ")\n",
          address, value & ~mask, value, mask, ~mask);

  rtems_fdt_write (address, value & ~mask);

  return 0;
}

static int
rtems_fdt_shell_up (int argc, char *argv[])
{
  uint32_t address;
  uint32_t offset = 0;
  uint32_t set;
  uint32_t value;
  int      mask_arg;
  uint32_t mask;

  if ((argc < 4) || (argc > 5))
    return rtems_fdt_wrong_number_of_args ();

  if (argc == 4)
    mask_arg = 2;
  else
  {
    offset = strtoul (argv[2], 0, 0);
    mask_arg = 3;
  }

  set = strtoul (argv[mask_arg + 1], 0, 0);

  if (!rtems_fdt_get_value32 (argv[1], "reg", sizeof (uint32_t), &address))
    return 1;

  if (isdigit (argv[mask_arg][0]))
    mask = strtoul (argv[mask_arg], 0, 0);
  else
  {
    if (!rtems_fdt_get_value32 (argv[mask_arg], "mask", sizeof (uint32_t), &mask))
      return 1;
  }

  address += offset;
  value = rtems_fdt_read (address);

  printf ("0x%08" PRIx32 " <= 0x%08" PRIx32 " = (0x%08" PRIx32 \
          " & ~0x%08" PRIx32 " (0x%08" PRIx32 ")) | 0x%08" PRIx32 "\n",
          address, (value & ~mask) | set, value, mask, ~mask, set);

  rtems_fdt_write (address, (value & ~mask) | set);

  return 0;
}

static int
rtems_fdt_shell_tst (int argc, char *argv[])
{
  uint32_t address;
  uint32_t offset = 0;
  uint32_t test;
  uint32_t value = 0;
  int      mask_arg;
  uint32_t mask;
  time_t   start;

  if ((argc < 4) || (argc > 5))
    return rtems_fdt_wrong_number_of_args ();

  if (argc == 4)
    mask_arg = 2;
  else
  {
    offset = strtoul (argv[2], 0, 0);
    mask_arg = 3;
  }

  test = strtoul (argv[mask_arg + 1], 0, 0);

  if (!rtems_fdt_get_value32 (argv[1], "reg", sizeof (uint32_t), &address))
    return 1;

  if (isdigit (argv[mask_arg][0]))
    mask = strtoul (argv[mask_arg], 0, 0);
  else
  {
     if (!rtems_fdt_get_value32 (argv[mask_arg], "mask", sizeof (uint32_t), &mask))
      return 1;
  }

  address += offset;

  start = time (NULL);

  printf ("0x%08" PRIx32 " => (value & 0x%08" PRIx32 ") == 0x%08" PRIx32 \
          " for %ld seconds\n",
          address, mask, test, rtems_fdt_test_timeout);

  while ((time (NULL) - start) < rtems_fdt_test_timeout)
  {
    int i;
    for (i = 0; i < 10000; ++i)
    {
      value = rtems_fdt_read (address);
      if ((value & mask) == test)
        return 0;
    }
  }

  printf ("0x%08" PRIx32 " => 0x%08" PRIx32 ": timeout\n", address, value);

  return 1;
}

static int
rtems_fdt_shell_nap (int argc, char *argv[])
{
  uint32_t time;

  if (argc != 2)
    return rtems_fdt_wrong_number_of_args ();

  time = strtoul (argv[1], 0, 0);

  if (time == 0)
  {
    printf ("error: 0 is not a valid time; check you have a valid number.\n");
    return 1;
  }

  usleep (time * 1000);

  return 0;
}

static int
rtems_fdt_shell_to (int argc, char *argv[])
{
  uint32_t to;

  if (argc == 1)
  {
    printf ("timeout: %ld seconds\n", rtems_fdt_test_timeout);
    return 0;
  }

  if (argc != 2)
    return rtems_fdt_wrong_number_of_args ();

  to = strtoul (argv[1], 0, 0);

  if (to == 0)
  {
    printf ("error: 0 is not a valid timeout; check you have a number.\n");
    return 1;
  }

  rtems_fdt_test_timeout = to;

  return 0;
}

static void
rtems_fdt_shell_usage (const char* arg)
{
  printf ("%s: FDT Help\n", arg);
  printf ("  %s [-hl] <command>\n", arg);
  printf ("   where:\n");
  printf ("     command: The FDT subcommand. See -l for a list plus help.\n");
  printf ("     -h:      This help\n");
  printf ("     -l:      The command list.\n");
}

static const rtems_fdt_shell_cmd table[] =
{
  { "ld",  rtems_fdt_shell_ld,  "<filename> : Load a FDT blob" },
  { "uld", rtems_fdt_shell_uld, "Uload an FDT blob" },
  { "ls",  rtems_fdt_shell_ls,  "<path> : List the nodes at the path and optionally below" },
  { "wr",  rtems_fdt_shell_wr,  "<path> [<offset>] <value> : Write the value." },
  { "rd",  rtems_fdt_shell_rd,  "<path> [<offset>] : Read the value." },
  { "set", rtems_fdt_shell_set, "<path> [<offset>] <mask> : Set the mask bits" },
  { "cl",  rtems_fdt_shell_cl,  "<path> [<offset>] <mask> : Clear the mask bits." },
  { "up",  rtems_fdt_shell_up,  "<path> [<offset>] <mask> <value> : Update the mask bit with value" },
  { "tst", rtems_fdt_shell_tst, "<path> [<offset>] <mask> <value> : Testing loop for masked value." },
  { "nap", rtems_fdt_shell_nap, "<time> : Sleep for the time period. It is in milli-seconds." },
  { "to",  rtems_fdt_shell_to,  "<value> : Set the test timeout (seconds)" },
};

#define RTEMS_FDT_COMMANDS (sizeof (table) / sizeof (const rtems_fdt_shell_cmd))

static int
rtems_fdt_shell_command (int argc, char* argv[])
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
        rtems_fdt_shell_usage (argv[0]);
        return 0;
      case 'l':
        printf ("%s: commands are:\n", argv[0]);
        for (t = 0; t < RTEMS_FDT_COMMANDS; ++t)
          printf ("  %-3s %s\n", table[t].name, table[t].help);
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
    for (t = 0; t < RTEMS_FDT_COMMANDS; ++t)
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

void
rtems_fdt_add_shell_command(void)
{
  rtems_shell_add_cmd ("fdt", "mem",
                       "Flattened device tree", rtems_fdt_shell_command);
}

rtems_fdt_handle*
rtems_fdt_get_shell_handle (void)
{
  return &cmd_fdt_handle;
}
