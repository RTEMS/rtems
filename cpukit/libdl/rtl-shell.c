/*
 *  COPYRIGHT (c) 2012, 2018 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtld
 *
 * @brief RTEMS Run-Time Link Editor Shell Commands
 *
 * A simple RTL command to aid using the RTL.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>
#include <rtems/inttypes.h>

#include <stdio.h>
#include <string.h>

#include <rtems/rtl/rtl.h>
#include "rtl-chain-iterator.h"
#include "rtl-shell.h"
#include <rtems/rtl/rtl-trace.h>

/**
 * The type of the shell handlers we have.
 */
typedef int (*rtems_rtl_shell_handler) (rtems_rtl_data* rtl, int argc, char *argv[]);

/**
 * Table of handlers we parse to invoke the command.
 */
typedef struct
{
  const char*             name;    /**< The sub-command's name. */
  rtems_rtl_shell_handler handler; /**< The sub-command's handler. */
  const char*             help;    /**< The sub-command's help. */
} rtems_rtl_shell_cmd;

/**
 * Object summary data.
 */
typedef struct
{
  int    count;   /**< The number of object files. */
  size_t exec;    /**< The amount of executable memory allocated. */
  size_t symbols; /**< The amount of symbol memory allocated. */
} rtems_rtl_obj_summary;

/**
 * Object summary iterator.
 */
static bool
rtems_rtl_obj_summary_iterator (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj_summary* summary = data;
  rtems_rtl_obj*         obj = (rtems_rtl_obj*) node;
  ++summary->count;
  summary->exec += obj->exec_size;
  summary->symbols += obj->global_size;
  return true;
}

/**
 * Count the number of symbols.
 */
static int
rtems_rtl_count_symbols (rtems_rtl_data* rtl)
{
  int count;
  int bucket;
  for (count = 0, bucket = 0; bucket < rtl->globals.nbuckets; ++bucket)
    count += rtems_rtl_chain_count (&rtl->globals.buckets[bucket]);
  return count;
}

static int
rtems_rtl_shell_status (rtems_rtl_data* rtl, int argc, char *argv[])
{
  rtems_rtl_obj_summary summary;
  size_t                total_memory;

  summary.count   = 0;
  summary.exec    = 0;
  summary.symbols = 0;
  rtems_rtl_chain_iterate (&rtl->objects,
                           rtems_rtl_obj_summary_iterator,
                           &summary);
  /*
   * Currently does not include the name strings in the obj struct.
   */
  total_memory =
    sizeof (*rtl) + (summary.count * sizeof (rtems_rtl_obj)) +
    summary.exec + summary.symbols;

  printf ("Runtime Linker Status:\n");
  printf ("        paths: %s\n", rtl->paths);
  printf ("      objects: %d\n", summary.count);
  printf (" total memory: %zi\n", total_memory);
  printf ("  exec memory: %zi\n", summary.exec);
  printf ("   sym memory: %zi\n", summary.symbols);
  printf ("      symbols: %d\n", rtems_rtl_count_symbols (rtl));

  return 0;
}

/**
 * Object print data.
 */
typedef struct
{
  rtems_rtl_data* rtl;          /**< The RTL data. */
  int             indent;       /**< Spaces to indent. */
  bool            oname;        /**< Print object names. */
  bool            names;        /**< Print details of all names. */
  bool            memory_map;   /**< Print the memory map. */
  bool            symbols;      /**< Print the global symbols. */
  bool            dependencies; /**< Print any dependencies. */
  bool            base;         /**< Include the base object file. */
} rtems_rtl_obj_print;

/**
 * Return the different between 2 void*.
 */
static size_t
rtems_rtl_delta_voids (void* higher, void* lower)
{
  char* ch = higher;
  char* cl = lower;
  return ch - cl;
}

/**
 * Parse an argument.
 */
static bool
rtems_rtl_parse_arg (const char* opt, int argc, char *argv[])
{
  int arg;
  for (arg = 0; arg < argc; ++arg)
    if (strncmp (opt, argv[arg], 2) == 0)
      return true;
  return false;
}

/**
 * See if -b for base is set.
 */
static bool
rtems_rtl_base_arg (int argc, char *argv[])
{
  return rtems_rtl_parse_arg ("-b", argc, argv);
}

/**
 * See if -s for base is set.
 */
static bool
rtems_rtl_symbols_arg (int argc, char *argv[])
{
  return rtems_rtl_parse_arg ("-s", argc, argv);
}

/**
 * Dependenncies printer.
 */
typedef struct
{
  bool   first;   /**< Is this the first line printed. */
  size_t indent;  /**< The indent. */
} rtems_rtl_dep_data;

static bool
rtems_rtl_dependencies (rtems_rtl_obj* obj,
                        rtems_rtl_obj* dependent,
                        void*          data)
{
  rtems_rtl_dep_data* dd = (rtems_rtl_dep_data*) data;
  if (!dd->first)
    printf ("\n%-*c: ", dd->indent, ' ');
  else
    dd->first = false;
  printf ("%s", dependent->oname);
  return false;
}

/**
 * Object printer.
 */
static bool
rtems_rtl_obj_printer (rtems_rtl_obj_print* print, rtems_rtl_obj* obj)
{
  char flags_str[33];

  /*
   * Skip the base module unless asked to show it.
   */
  if (!print->base && (obj == print->rtl->base))
      return true;

  if (print->oname)
  {
    printf ("%-*cobject name   : %s\n",
            print->indent, ' ', rtems_rtl_obj_oname (obj));
  }
  if (print->names)
  {
    printf ("%-*cfile name     : %s\n",
            print->indent, ' ', rtems_rtl_obj_fname (obj));
    printf ("%-*carchive name  : %s\n",
            print->indent, ' ', rtems_rtl_obj_aname (obj));
    strcpy (flags_str, "--");
    if (obj->flags & RTEMS_RTL_OBJ_LOCKED)
      flags_str[0] = 'L';
    if (obj->flags & RTEMS_RTL_OBJ_UNRESOLVED)
      flags_str[1] = 'U';
    printf ("%-*cflags         : %s\n", print->indent, ' ', flags_str);
    printf ("%-*cfile offset   : %" PRIdoff_t "\n", print->indent, ' ', obj->ooffset);
    printf ("%-*cfile size     : %zi\n", print->indent, ' ', obj->fsize);
  }
  if (print->memory_map)
  {
    printf ("%-*cexec size     : %zi\n", print->indent, ' ', obj->exec_size);
    printf ("%-*ctext base     : %p (%zi)\n", print->indent, ' ',
            obj->text_base, rtems_rtl_delta_voids (obj->const_base, obj->text_base));
    printf ("%-*cconst base    : %p (%zi)\n", print->indent, ' ',
            obj->const_base, rtems_rtl_delta_voids (obj->data_base, obj->const_base));
    printf ("%-*cdata base     : %p (%zi)\n", print->indent, ' ',
            obj->data_base, rtems_rtl_delta_voids (obj->bss_base, obj->data_base));
    printf ("%-*cbss base      : %p (%zi)\n", print->indent, ' ',
            obj->bss_base, obj->bss_size);
  }
  printf ("%-*cunresolved    : %zu\n", print->indent, ' ', obj->unresolved);
  printf ("%-*cusers         : %zu\n", print->indent, ' ', obj->users);
  printf ("%-*creferences    : %zu\n", print->indent, ' ', obj->refs);
  printf ("%-*csymbols       : %zi\n", print->indent, ' ', obj->global_syms);
  printf ("%-*csymbol memory : %zi\n", print->indent, ' ', obj->global_size);
  if (print->symbols)
  {
    int max_len = 0;
    int s;
    for (s = 0; s < obj->global_syms; ++s)
    {
      int len = strlen (obj->global_table[s].name);
      if (len > max_len)
        max_len = len;
    }
    for (s = 0; s < obj->global_syms; ++s)
      printf ("%-*c%-*s = %p\n", print->indent + 2, ' ',
              max_len, obj->global_table[s].name, obj->global_table[s].value);
  }
  if (print->dependencies)
  {
    rtems_rtl_dep_data dd = {
      .first = true,
      .indent = strlen ("dependencies :") + print->indent
    };
    printf ("%-*cdependencies  : ", print->indent, ' ');
    rtems_rtl_obj_iterate_dependents (obj, rtems_rtl_dependencies, &dd);
    printf ("\n");
  }
  printf ("\n");
  return true;
}

/**
 * Object unresolved symbols printer.
 */
static bool
rtems_rtl_unresolved_printer (rtems_rtl_unresolv_rec* rec,
                              void*                   data)
{
  rtems_rtl_obj_print* print = (rtems_rtl_obj_print*) data;
  if (rec->type == rtems_rtl_unresolved_name)
    printf ("%-*c%s\n", print->indent + 2, ' ', rec->rec.name.name);
  return false;
}

/**
 * Object print iterator.
 */
static bool
rtems_rtl_obj_print_iterator (rtems_chain_node* node, void* data)
{
  rtems_rtl_obj_print* print = data;
  rtems_rtl_obj*       obj = (rtems_rtl_obj*) node;
  return rtems_rtl_obj_printer (print, obj);
}

static int
rtems_rtl_shell_list (rtems_rtl_data* rtl, int argc, char *argv[])
{
  rtems_rtl_obj_print print;
  print.rtl = rtl;
  print.indent = 1;
  print.oname = true;
  print.names = true;
  print.memory_map = true;
  print.symbols = rtems_rtl_symbols_arg (argc, argv);
  print.dependencies = true;
  print.base = false;
  rtems_rtl_chain_iterate (&rtl->objects,
                           rtems_rtl_obj_print_iterator,
                           &print);
  return 0;
}

static int
rtems_rtl_shell_sym (rtems_rtl_data* rtl, int argc, char *argv[])
{
  rtems_rtl_obj_print print;
  print.rtl = rtl;
  print.indent = 1;
  print.oname = true;
  print.names = false;
  print.memory_map = false;
  print.symbols = true;
  print.dependencies = false;
  print.base = rtems_rtl_base_arg (argc, argv);
  rtems_rtl_chain_iterate (&rtl->objects,
                           rtems_rtl_obj_print_iterator,
                           &print);
  printf ("Unresolved:\n");
  rtems_rtl_unresolved_interate (rtems_rtl_unresolved_printer, &print);
  return 0;
}

static int
rtems_rtl_shell_object (rtems_rtl_data* rtl, int argc, char *argv[])
{
  return 0;
}

static void
rtems_rtl_shell_usage (const char* arg)
{
  printf ("%s: Runtime Linker\n", arg);
  printf ("  %s [-hl] <command>\n", arg);
  printf ("   where:\n");
  printf ("     command: A n RTL command. See -l for a list plus help.\n");
  printf ("     -h:      This help\n");
  printf ("     -l:      The command list.\n");
}

int
rtems_rtl_shell_command (int argc, char* argv[])
{
  const rtems_rtl_shell_cmd table[] =
  {
    { "status", rtems_rtl_shell_status,
      "Display the status of the RTL" },
    { "list", rtems_rtl_shell_list,
      "\tList the object files currently loaded" },
    { "sym", rtems_rtl_shell_sym,
      "\tDisplay the symbols, sym [<name>], sym -o <obj> [<name>]" },
    { "obj", rtems_rtl_shell_object,
      "\tDisplay the object details, obj <name>" }
  };

  int arg;
  int t;

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] != '-')
      break;

    switch (argv[arg][1])
    {
      case 'h':
        rtems_rtl_shell_usage (argv[0]);
        return 0;
      case 'l':
        printf ("%s: commands are:\n", argv[0]);
        for (t = 0;
             t < (sizeof (table) / sizeof (const rtems_rtl_shell_cmd));
             ++t)
          printf ("  %s\t%s\n", table[t].name, table[t].help);
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
    for (t = 0;
         t < (sizeof (table) / sizeof (const rtems_rtl_shell_cmd));
         ++t)
    {
      if (strncmp (argv[arg], table[t].name, strlen (argv[arg])) == 0)
      {
        rtems_rtl_data* rtl = rtems_rtl_lock ();
        int             r;
        if (!rtl)
        {
          printf ("error: cannot lock the linker\n");
          return 1;
        }
        r = table[t].handler (rtl, argc - 1, argv + 1);
        rtems_rtl_unlock ();
        return r;
      }
    }
    printf ("error: command not found: %s (try -h)\n", argv[arg]);
  }

  return 1;
}
