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

#include <sys/stat.h>
#include <regex.h>
#include <string.h>

#include <dlfcn.h>

#include <rtems/printer.h>
#include <rtems/rtl/rtl.h>
#include <rtems/rtl/rtl-archive.h>
#include <rtems/rtl/rtl-shell.h>
#include <rtems/rtl/rtl-trace.h>
#include "rtl-chain-iterator.h"

/**
 * The type of the shell handlers we have.
 */
typedef int (*rtems_rtl_shell_handler) (const rtems_printer* printer, int argc, char *argv[]);

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
rtems_rtl_shell_status (const rtems_printer* printer,
                        int                  argc,
                        char*                argv[])
{
  rtems_rtl_obj_summary summary;
  size_t                total_memory;
  rtems_rtl_data*       rtl;

  rtl = rtems_rtl_lock ();
  if (rtl == NULL)
  {
    rtems_printf (printer, "error: cannot lock the linker\n");
    return 1;
  }

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

  rtems_printf (printer, "Runtime Linker Status:\n");
  rtems_printf (printer, "        paths: %s\n", rtl->paths);
  rtems_printf (printer, "      objects: %d\n", summary.count);
  rtems_printf (printer, " total memory: %zi\n", total_memory);
  rtems_printf (printer, "  exec memory: %zi\n", summary.exec);
  rtems_printf (printer, "   sym memory: %zi\n", summary.symbols);
  rtems_printf (printer, "      symbols: %d\n", rtems_rtl_count_symbols (rtl));

  rtems_rtl_unlock ();

  return 0;
}

/**
 * Object print data.
 */
typedef struct
{
  const rtems_printer* printer;      /**< The RTEMS printer. */
  rtems_rtl_data*      rtl;          /**< The RTL data. */
  int                  indent;       /**< Spaces to indent. */
  bool                 oname;        /**< Print object names. */
  bool                 names;        /**< Print details of all names. */
  bool                 stats;        /**< Print stats. */
  bool                 memory_map;   /**< Print the memory map. */
  bool                 symbols;      /**< Print the global symbols. */
  bool                 dependencies; /**< Print any dependencies. */
  bool                 trampolines;  /**< Print trampoline stats. */
  bool                 base;         /**< Include the base object file. */
  const char*          re_name;      /**< Name regx to filter on. */
  const char*          re_symbol;    /**< Symbol regx to filter on. */
} rtems_rtl_obj_print;

/**
 * Parse an argument.
 */
static bool
rtems_rtl_parse_opt (const char opt, int argc, char *argv[])
{
  size_t arg;
  for (arg = 1; arg < argc; ++arg)
  {
    if (argv[arg][0] == '-')
    {
      size_t len = strlen (argv[arg]);
      size_t i;
      for (i = 1; i < len; ++i)
        if (argv[arg][i] == opt)
          return true;
    }
  }
  return false;
}

static bool
rtems_rtl_check_opts (const rtems_printer* printer,
                      const char*          opts,
                      int                  argc,
                      char*                argv[])
{
  size_t olen = strlen (opts);
  size_t arg;
  for (arg = 1; arg < argc; ++arg)
  {
    if (argv[arg][0] == '-')
    {
      size_t len = strlen (argv[arg]);
      size_t i;
      for (i = 1; i < len; ++i)
      {
        bool found = false;
        size_t       o;
        for (o = 0; o < olen; ++o)
        {
          if (argv[arg][i] == opts[o])
          {
            found = true;
            break;
          }
        }
        if (!found)
        {
          rtems_printf (printer, "error: invalid option: %c (%s)\n",
                        argv[arg][i], argv[arg]);
          return false;
        }
      }
    }
  }
  return true;
}

static ssize_t
rtems_rtl_parse_arg_index (const char  opt,
                           const char* skip_opts,
                           int         argc,
                           char*       argv[])
{
  ssize_t arg;
  for (arg = 1; arg < argc; ++arg)
  {
    if (argv[arg][0] == '-')
    {
      /*
       * We can check the next char because there has to be a valid char or a
       * nul.
       */
      if (argv[arg][1] != '\0')
      {
        size_t len = skip_opts != NULL ? strlen (skip_opts) : 0;
        size_t i;
        for (i = 0; i < len; ++i)
        {
          if (skip_opts[i] == argv[arg][1])
          {
            ++arg;
            break;
          }
        }
      }
    }
    else
    {
      if (opt == ' ')
        return arg;
    }
    /*
     * Is this an option and does it match what we are looking for?
     */
    if (argv[arg][0] == '-' && argv[arg][1] == opt && arg < argc)
      return arg + 1;
  }
  return -1;
}

static const char*
rtems_rtl_parse_arg (const char  opt,
                     const char* skip_opts,
                     int         argc,
                     char*       argv[])
{
  ssize_t arg = rtems_rtl_parse_arg_index (opt, skip_opts, argc, argv);
  if (arg < 0)
    return NULL;
  return argv[arg];
}

/**
 * Regx matching.
 */
static bool
rtems_rtl_regx_compile (const rtems_printer* printer,
                        const char*          label,
                        regex_t*             rege,
                        const char*          expression)
{
  int r = regcomp (rege, expression, REG_EXTENDED | REG_NOSUB);
  if (r != 0)
  {
    char rerror[128];
    regerror (r, rege, rerror, sizeof(rerror));
    rtems_printf (printer, "error: %s: %s\n", label, rerror);
    return false;
  }
  return true;
}

static int
rtems_rtl_regx_match (const rtems_printer* printer,
                      const char*          label,
                      regex_t*             rege,
                      const char*          string)
{
  int r = regexec (rege, string, 0, NULL, 0);
  if (r != 0 && r != REG_NOMATCH)
  {
    char rerror[128];
    regerror (r, rege, rerror, sizeof(rerror));
    rtems_printf (printer, "error: %s: %s\n", label, rerror);
    regfree (rege);
    return -1;
  }
  return r == 0 ? 1 : 0;
}

/**
 * Print the obj name.
 */
static void
rtems_rtl_print_obj_name (const rtems_rtl_obj_print* print, rtems_rtl_obj* obj)
{
  rtems_printf (print->printer, "%-*c", print->indent, ' ');
  if (rtems_rtl_obj_aname (obj) != NULL)
    rtems_printf (print->printer, "%s:", rtems_rtl_obj_aname (obj));
  rtems_printf (print->printer, "%s\n", rtems_rtl_obj_oname (obj));
}

/**
 * Print symbols.
 */
static bool
rtems_rtl_print_symbols (rtems_rtl_obj_print* print,
                         rtems_rtl_obj*       obj,
                         int                  indent,
                         bool                 show_name)
{
  regex_t rege;
  int     max_len = 0;
  int     s;

  if (print->re_symbol != NULL &&
      !rtems_rtl_regx_compile (print->printer,
                               "symbol filter",
                               &rege, print->re_symbol))
  {
    return false;
  }

  for (s = 0; s < obj->global_syms; ++s)
  {
    const char* sym = obj->global_table[s].name;
    int         len;

    if (print->re_symbol != NULL)
    {
      int r = rtems_rtl_regx_match (print->printer, "symbol match", &rege, sym);
      if (r < 0)
        return false;
      if (!r)
        continue;
    }

    len = strlen (obj->global_table[s].name);
    if (len > max_len)
      max_len = len;
  }

  for (s = 0; s < obj->global_syms; ++s)
  {
    const char* sym = obj->global_table[s].name;
    if (print->re_symbol != NULL)
    {
      int r = rtems_rtl_regx_match (print->printer, "symbol match", &rege, sym);
      if (r < 0)
        return false;
      if (r == 0)
        continue;
    }
    if (show_name)
    {
      show_name = false;
      rtems_rtl_print_obj_name (print, obj);
    }
    rtems_printf (print->printer, "%-*c%-*s = %p\n", indent + 2, ' ',
                  max_len, sym, obj->global_table[s].value);
  }

  regfree (&rege);

  return true;
}

/**
 * Dependencies printer.
 */
typedef struct
{
  const rtems_rtl_obj_print* print;     /**< The print data. */
  bool                       first;     /**< Is this the first line printed. */
  bool                       show_name; /**< Show the object name. */
  size_t                     indent;    /**< The indent. */
} rtems_rtl_dep_data;

static bool
rtems_rtl_dependencies (rtems_rtl_obj* obj, rtems_rtl_obj* dependent, void* data)
{
  rtems_rtl_dep_data* dd = (rtems_rtl_dep_data*) data;
  if (dd->first)
  {
    dd->first = false;
    if (dd->show_name)
    {
      dd->show_name = false;
      rtems_rtl_print_obj_name (dd->print, obj);
    }
    rtems_printf (dd->print->printer, "%-*cdependencies  : ", dd->indent, ' ');
    dd->indent += strlen ("dependencies :");
  }
  else
  {
    rtems_printf (dd->print->printer, "\n%-*c: ", (int) dd->indent, ' ');
  }
  rtems_printf (dd->print->printer, "%s", dependent->oname);
  return false;
}

/**
 * Object printer.
 */
static bool
rtems_rtl_obj_printer (rtems_rtl_obj_print* print, rtems_rtl_obj* obj)
{
  char flags_str[33];
  int  indent = print->indent + 1;
  bool show_name = true;

  /*
   * Skip the base module unless asked to show it.
   */
  if (!print->base && (obj == print->rtl->base))
      return true;

  if (print->re_name != NULL)
  {
    regex_t rege;
    int     r = 0;

    if (!rtems_rtl_regx_compile (print->printer,
                                 "name filter",
                                 &rege, print->re_name))
    {
      return false;
    }

    if (rtems_rtl_obj_aname (obj) != NULL)
    {
      r = rtems_rtl_regx_match (print->printer,
                                "aname match",
                                &rege,
                                rtems_rtl_obj_aname (obj));
      if (r < 0)
        return false;
    }

    if (r == 0)
    {
      r = rtems_rtl_regx_match (print->printer,
                                "oname match",
                                &rege,
                                rtems_rtl_obj_oname (obj));
      if (r < 0)
        return false;
    }

    regfree (&rege);

    if (r == 0)
      return true;
  }

  if (print->names || print->memory_map || print->stats ||
      (!print->names && !print->memory_map && !print->stats &&
       !print->symbols && !print->dependencies))
  {
    show_name = false;
    rtems_rtl_print_obj_name (print, obj);
  }

  if (print->names)
  {
    rtems_printf (print->printer,
                  "%-*cfile name     : %s\n",
                  indent, ' ', rtems_rtl_obj_fname (obj));
    rtems_printf (print->printer,
                  "%-*carchive name  : %s\n",
                  indent, ' ', rtems_rtl_obj_aname (obj));
    strcpy (flags_str, "--");
    if (obj->flags & RTEMS_RTL_OBJ_LOCKED)
      flags_str[0] = 'L';
    if (obj->flags & RTEMS_RTL_OBJ_UNRESOLVED)
      flags_str[1] = 'U';
    rtems_printf (print->printer,
                  "%-*cflags         : %s\n", indent, ' ', flags_str);
    rtems_printf (print->printer,
                  "%-*cfile offset   : %" PRIdoff_t "\n", indent, ' ', obj->ooffset);
    rtems_printf (print->printer,
                  "%-*cfile size     : %zi\n", indent, ' ', obj->fsize);
  }
  if (print->memory_map)
  {
    rtems_printf (print->printer,
                  "%-*cexec size     : %zi\n", indent, ' ', obj->exec_size);
    rtems_printf (print->printer,
                  "%-*ctext base     : %p (%zi)\n", indent, ' ',
                  obj->text_base, obj->text_size);
    rtems_printf (print->printer,
                  "%-*cconst base    : %p (%zi)\n", indent, ' ',
                  obj->const_base, obj->const_size);
    rtems_printf (print->printer,
                  "%-*cdata base     : %p (%zi)\n", indent, ' ',
                  obj->data_base, obj->data_size);
    rtems_printf (print->printer,
                  "%-*cbss base      : %p (%zi)\n", indent, ' ',
                  obj->bss_base, obj->bss_size);
  }
  if (print->stats)
  {
    rtems_printf (print->printer, "%-*cunresolved    : %zu\n", indent, ' ', obj->unresolved);
    rtems_printf (print->printer, "%-*cusers         : %zu\n", indent, ' ', obj->users);
    rtems_printf (print->printer, "%-*creferences    : %zu\n", indent, ' ', obj->refs);
    rtems_printf (print->printer, "%-*ctrampolines   : %zu\n", indent, ' ',
                  rtems_rtl_obj_trampolines (obj));
    rtems_printf (print->printer, "%-*csymbols       : %zi\n", indent, ' ', obj->global_syms);
    rtems_printf (print->printer, "%-*csymbol memory : %zi\n", indent, ' ', obj->global_size);
  }
  if (print->symbols)
  {
    if (!rtems_rtl_print_symbols (print, obj, indent, show_name))
      return false;
  }
  if (print->dependencies)
  {
    rtems_rtl_dep_data dd = {
      .print = print,
      .first = true,
      .show_name = show_name,
      .indent = indent
    };
    rtems_rtl_obj_iterate_dependents (obj, rtems_rtl_dependencies, &dd);
    if (!dd.first)
      rtems_printf (print->printer, "\n");
  }
  if (print->trampolines)
  {
    if (obj->tramp_size == 0)
    {
      rtems_printf (print->printer, "%-*ctrampolines: not supported\n", indent, ' ');
    }
    else
    {
      size_t slots = rtems_rtl_obj_trampoline_slots (obj);
      size_t used = rtems_rtl_obj_trampolines (obj);
      rtems_printf (print->printer, "%-*ctrampolines:\n", indent, ' ');
      rtems_printf (print->printer, "%-*cslots     : %zu\n", indent + 4, ' ',
                    slots);
      rtems_printf (print->printer, "%-*csize      : %zu\n", indent + 4, ' ',
                    obj->tramps_size);
      rtems_printf (print->printer, "%-*cslot size : %zu\n", indent + 4, ' ',
                    obj->tramp_size);
      rtems_printf (print->printer, "%-*cused      : %zu\n", indent + 4, ' ',
                    used);
      rtems_printf (print->printer, "%-*crelocs    : %zu\n", indent + 4, ' ',
                    obj->tramp_relocs);
      rtems_printf (print->printer, "%-*cunresolved: %zu\n", indent + 4, ' ',
                    slots - obj->tramp_relocs);
      rtems_printf (print->printer, "%-*cyield     : %zu%%\n", indent + 4, ' ',
                    slots ? (used * 100) / slots : 0);
    }
  }
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
  if (rec->type == rtems_rtl_unresolved_symbol)
    rtems_printf (print->printer,
                  "%-*c%s\n", print->indent + 2, ' ', rec->rec.name.name);
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

int
rtems_rtl_shell_list (const rtems_printer* printer, int argc, char* argv[])
{
  rtems_rtl_obj_print print = { 0 };
  if (!rtems_rtl_check_opts (printer, "anlmsdbt", argc, argv))
    return 1;
  print.printer = printer;
  print.indent = 1;
  print.oname = true;
  if (rtems_rtl_parse_opt ('a', argc, argv))
  {
    print.names = true;
    print.stats = true;
    print.memory_map = true;
    print.symbols = true;
    print.dependencies = true;
    print.trampolines = true;
  }
  else
  {
    print.names = rtems_rtl_parse_opt ('n', argc, argv);
    print.stats = rtems_rtl_parse_opt ('l', argc, argv);;
    print.memory_map = rtems_rtl_parse_opt ('m', argc, argv);;
    print.symbols = rtems_rtl_parse_opt ('s', argc, argv);
    print.dependencies = rtems_rtl_parse_opt ('d', argc, argv);;
    print.trampolines = rtems_rtl_parse_opt ('t', argc, argv);;
    print.base = rtems_rtl_parse_opt ('b', argc, argv);;
    print.re_name = rtems_rtl_parse_arg (' ', NULL, argc, argv);
  }
  print.re_symbol = NULL;
  print.rtl = rtems_rtl_lock ();
  if (print.rtl == NULL)
  {
    rtems_printf (print.printer, "error: cannot lock the linker\n");
    return 1;
  }
  rtems_rtl_chain_iterate (&print.rtl->objects,
                           rtems_rtl_obj_print_iterator,
                           &print);
  rtems_rtl_unlock ();
  return 0;
}

int
rtems_rtl_shell_sym (const rtems_printer* printer, int argc, char* argv[])
{
  rtems_rtl_obj_print print = { 0 };
  if (!rtems_rtl_check_opts (printer, "buo", argc, argv))
    return 1;
  print.printer = printer;
  print.indent = 1;
  print.oname = true;
  print.names = false;
  print.stats = false;
  print.memory_map = false;
  print.symbols = !rtems_rtl_parse_opt ('u', argc, argv);;
  print.dependencies = false;
  print.base = rtems_rtl_parse_opt ('b', argc, argv);
  print.re_name = rtems_rtl_parse_arg ('o', NULL, argc, argv);;
  print.re_symbol = rtems_rtl_parse_arg (' ', "ou", argc, argv);
  print.rtl = rtems_rtl_lock ();
  if (print.rtl == NULL)
  {
    rtems_printf (print.printer, "error: cannot lock the linker\n");
    return 1;
  }
  if (print.symbols)
  {
    rtems_rtl_chain_iterate (&print.rtl->objects,
                             rtems_rtl_obj_print_iterator,
                             &print);
  }
  if (rtems_rtl_parse_opt ('u', argc, argv))
  {
    rtems_printf (printer, "Unresolved:\n");
    rtems_rtl_unresolved_iterate (rtems_rtl_unresolved_printer, &print);
  }
  rtems_rtl_unlock ();
  return 0;
}

int
rtems_rtl_shell_object (const rtems_printer* printer, int argc, char* argv[])
{
  size_t arg;

  --argc;
  ++argv;

  for (arg = 0; arg < argc; ++arg)
  {
    if (argv[arg][0] == '-')
    {
      switch (argv[arg][1])
      {
        case 'h':
        case '?':
          rtems_printf (printer, "obj commands:\n");
          rtems_printf (printer, " load <file>\n");
          rtems_printf (printer, " unload <file>\n");
          break;
        default:
          rtems_printf (printer, "error: invalid option: %s\n", argv[arg]);
          return 1;
      }
    }
    else
    {
      break;
    }
  }

  if (arg >= argc)
  {
    rtems_printf (printer, "error: no obj command\n");
    return 1;
  }

  if (strcmp (argv[arg], "load") == 0)
  {
    void* handle;
    int   unresolved;

    ++arg;
    if (arg >= argc)
    {
      rtems_printf (printer, "error: no object file to load\n");
      return 1;
    }

    handle = dlopen (argv[arg], RTLD_NOW | RTLD_GLOBAL);
    if (handle == NULL)
    {
      rtems_printf (printer, "error: load: %s: %s\n", argv[arg], dlerror ());
      return 1;
    }

    if (dlinfo (RTLD_SELF, RTLD_DI_UNRESOLVED, &unresolved) < 0)
    {
      rtems_printf (printer, "error: %s: %s\n", argv[arg], dlerror ());
      return 1;
    }

    if (unresolved != 0)
    {
      rtems_printf (printer, "warning: unresolved symbols present\n");
      return 1;
    }
  }
  else if (strcmp (argv[arg], "unload") == 0)
  {
    rtems_rtl_data* rtl;
    rtems_rtl_obj*  obj;

    ++arg;
    if (arg >= argc)
    {
      rtems_printf (printer, "error: no object file to load\n");
      return 1;
    }

    rtl = rtems_rtl_lock ();
    if (rtl == NULL)
    {
      rtems_printf (printer, "error: cannot lock RTL\n");
      return 1;
    }

    obj = rtems_rtl_find_obj (argv[arg]);
    if (obj == NULL)
    {
      rtems_rtl_unlock ();
      rtems_printf (printer, "error: unload: %s: %s\n", argv[arg], dlerror ());
      return 1;
    }

    if (!rtems_rtl_unload (obj))
    {
      rtems_rtl_unlock ();
      rtems_printf (printer, "error: unload: %s: %s\n", argv[arg], dlerror ());
      return 1;
    }

    rtems_rtl_unlock ();
  }
  else
  {
    rtems_printf (printer, "error: unknown obj command: %s\n", argv[arg]);
    return 1;
  }

  return 0;
}

int
rtems_rtl_shell_archive (const rtems_printer* printer, int argc, char* argv[])
{
  rtems_rtl_data*   rtl;
  rtems_chain_node* node;
  const char*       re_name;
  bool              details;
  bool              symbols;
  bool              duplicates;
  regex_t           rege;

  if (!rtems_rtl_check_opts (printer, "dsl", argc, argv))
    return 1;

  details = rtems_rtl_parse_opt ('l', argc, argv);
  symbols = rtems_rtl_parse_opt ('s', argc, argv);
  duplicates = rtems_rtl_parse_opt ('d', argc, argv);

  re_name = rtems_rtl_parse_arg (' ', NULL, argc, argv);

  if (re_name != NULL)
  {
    if (!rtems_rtl_regx_compile (printer,
                                 "name filter",
                                 &rege,
                                 re_name))
    {
      return false;
    }
  }

  rtl = rtems_rtl_lock ();
  if (rtl == NULL)
  {
    rtems_printf (printer, "error: cannot lock the linker\n");
    return 1;
  }

  node = rtems_chain_first (&rtl->archives.archives);

  while (!rtems_chain_is_tail (&rtl->archives.archives, node))
  {
    #define SYM_DUPLICATE (1 << ((8 * sizeof (size_t)) - 1))

    rtems_rtl_archive* archive = (rtems_rtl_archive*) node;

    if (re_name != NULL)
    {
      int r = rtems_rtl_regx_match (printer,
                                    "name match",
                                    &rege,
                                    archive->name);
      if (r < 0)
      {
        rtems_rtl_unlock ();
        return false;
      }

      if (r == 0)
      {
        node = rtems_chain_next (node);
        continue;
      }
    }

    rtems_printf (printer, "%s%c\n",
                  archive->name,
                  details | symbols | duplicates ? ':' : ' ');

    if (details)
    {
      rtems_printf (printer, "  size    : %zu\n", archive->size);
      rtems_printf (printer, "  symbols : %zu\n", archive->symbols.entries);
      rtems_printf (printer, "  refs    : %zu\n", archive->refs);
      rtems_printf (printer, "  flags   : %" PRIx32 "\n", archive->flags);
    }

    if (symbols)
    {
      const char* symbol = archive->symbols.names;
      int         indent = 0;
      size_t      s;

      rtems_printf (printer, "  symbols :");

      for (s = 0; s < archive->symbols.entries; ++s)
      {
        if (archive->symbols.symbols != NULL)
          symbol = archive->symbols.symbols[s].label;

        rtems_printf (printer, "%-*c%s\n", indent, ' ', symbol);
        indent = 12;

        if (archive->symbols.symbols == NULL)
          symbol += strlen (symbol) + 1;
      }

      if (indent == 0)
        rtems_printf (printer, "\n");
    }

    if (duplicates)
    {
      rtems_chain_node* match_node;
      int               indent = 0;
      bool              show_dups = true;

      match_node = rtems_chain_first (&rtl->archives.archives);

      while (!rtems_chain_is_tail (&rtl->archives.archives, match_node))
      {
        rtems_rtl_archive* match_archive = (rtems_rtl_archive*) match_node;
        const char*        symbol = archive->symbols.names;
        size_t             s;

        for (s = 0; s < archive->symbols.entries; ++s)
        {
          if (archive->symbols.symbols == NULL ||
              (archive->symbols.symbols[s].entry & SYM_DUPLICATE) == 0)
          {
            const char* match_symbol = match_archive->symbols.names;
            size_t      ms;

            if (archive->symbols.symbols != NULL)
              symbol = archive->symbols.symbols[s].label;

            for (ms = 0; ms < match_archive->symbols.entries; ++ms)
            {
              if (match_archive->symbols.symbols != NULL)
                match_symbol = match_archive->symbols.symbols[ms].label;

              if (symbol != match_symbol && strcmp (symbol, match_symbol) == 0)
              {
                if (show_dups)
                {
                  show_dups = false;
                  rtems_printf (printer, "  dups    :");
                }
                rtems_printf (printer, "%-*c%s (%s)\n",
                              indent, ' ', symbol, archive->name);
                indent = 12;

                if (match_archive->symbols.symbols != NULL)
                  match_archive->symbols.symbols[ms].entry |= SYM_DUPLICATE;
              }

              if (match_archive->symbols.symbols == NULL)
                match_symbol += strlen (match_symbol) + 1;
            }
          }

          if (archive->symbols.symbols == NULL)
            symbol += strlen (symbol) + 1;
        }

        match_node = rtems_chain_next (match_node);
      }

      if (indent == 0)
        rtems_printf (printer, "\n");
    }

    node = rtems_chain_next (node);
  }

  regfree (&rege);

  node = rtems_chain_first (&rtl->archives.archives);

  while (!rtems_chain_is_tail (&rtl->archives.archives, node))
  {
    rtems_rtl_archive* archive = (rtems_rtl_archive*) node;
    if (archive->symbols.symbols != NULL)
    {
      size_t s;
      for (s = 0; s < archive->symbols.entries; ++s)
        archive->symbols.symbols[s].entry &= ~SYM_DUPLICATE;
    }
    node = rtems_chain_next (node);
  }

  rtems_rtl_unlock ();

  return 0;
}

int
rtems_rtl_shell_call (const rtems_printer* printer, int argc, char* argv[])
{
  #define CALL_ARG_COUNT (4)

  typedef void (*csig_none)(void);
  typedef void (*csig_argv)(int argc, const char* argv[]);
  typedef void (*csig_s)(const char* str);
  typedef void (*csig_u)(unsigned int i1, unsigned int i2, unsigned int i3, unsigned int i4);
  typedef void (*csig_i)(int i1, int i2, int i3, int i4);

  union {
    char         s[64 + 1];
    unsigned int u[CALL_ARG_COUNT];
    int          i[CALL_ARG_COUNT];
  } values = { 0 };
  bool               keep_locked = false;
  bool               args_s = false;
  bool               args_i = false;
  bool               args_u = false;
  ssize_t            label;
  rtems_rtl_data*    rtl;
  rtems_rtl_obj_sym* sym;
  rtems_rtl_obj*     obj;


  if (!rtems_rtl_check_opts (printer, "lsui", argc, argv))
    return 1;

  keep_locked = rtems_rtl_parse_opt ('l', argc, argv);
  args_s = rtems_rtl_parse_opt ('s', argc, argv);
  args_u = rtems_rtl_parse_opt ('u', argc, argv);
  args_i = rtems_rtl_parse_opt ('i', argc, argv);

  if (args_s || args_u || args_i)
  {
    int c = 0;
    c += args_s ? 1 : 0;
    c += args_u ? 1 : 0;
    c += args_i ? 1 : 0;
    if (c > 1)
    {
      rtems_printf (printer,
                    "error: too many options, only one -sul at a time\n");
      return 1;
    }
  }

  label = rtems_rtl_parse_arg_index (' ', NULL, argc, argv);
  if (label < 0)
  {
    rtems_printf (printer, "error: no symbol found on command line\n");
    return 1;
  }

  if ((label + 1) < argc)
  {
    if (args_s)
    {
      size_t arg;
      for (arg = label + 1; arg < argc; ++arg)
      {
        size_t o = strlen (values.s);
        if (strlen (argv[arg]) + 1 >= (sizeof (values.s) - o))
        {
          rtems_printf (printer, "error: string args too big\n");
          return 1;
        }
        if (o > 0)
          values.s[o++] = ' ';
        strcat (values.s, argv[arg]);
      }
    }
    else if (args_u || args_i)
    {
      size_t arg;
      size_t i;
      if (argc > (label + 1 + CALL_ARG_COUNT))
      {
        rtems_printf (printer, "error: too many args\n");
        return 1;
      }
      for (i = 0, arg = label + 1; arg < argc; ++arg)
      {
        if (args_u)
          values.u[i] = strtoul (argv[arg], 0, 0);
        else
          values.i[i] = strtol (argv[arg], 0, 0);
        ++i;
      }
    }
  }

  rtl = rtems_rtl_lock ();
  if (rtl == NULL)
  {
    rtems_printf (printer, "error: cannot lock the linker\n");
    return 1;
  }

  sym = rtems_rtl_symbol_global_find (argv[label]);
  if (sym == NULL)
  {
    rtems_rtl_unlock ();
    rtems_printf (printer, "error: symbol not found: %s\n", argv[label]);
    return 1;
  }

  obj = rtems_rtl_find_obj_with_symbol (sym);
  if (obj == NULL)
  {
    rtems_rtl_unlock ();
    rtems_printf (printer, "error: symbol obj not found: %s\n", argv[label]);
    return 1;
  }

  if (!rtems_rtl_obj_text_inside (obj, (const void*) sym->value))
  {
    rtems_rtl_unlock ();
    rtems_printf (printer, "error: symbol not in obj text: %s\n", argv[label]);
    return 1;
  }

  /*
   * Lock the object file while it is being called.
   */
  rtems_rtl_obj_inc_reference (obj);

  rtems_rtl_unlock ();

  if (args_s)
  {
    csig_s call = (csig_s) sym->value;
    call (values.s);
  }
  else if (args_u)
  {
    csig_u call = (csig_u) sym->value;
    call (values.u[0], values.u[1], values.u[2], values.u[3]);
  }
  else if (args_i)
  {
    csig_i call = (csig_i) sym->value;
    call (values.i[0], values.i[1], values.i[2], values.i[3]);
  }
  else
  {
    int cargc = argc - (label + 1);
    if (cargc == 0)
    {
      csig_none call = (csig_none) sym->value;
      call ();
    }
    else
    {
      csig_argv   call = (csig_argv) sym->value;
      const char* cargv = argv[label + 1];
      call (cargc, &cargv);
    }
  }

  if (!keep_locked)
  {
    rtl = rtems_rtl_lock ();
    if (rtl == NULL)
    {
      rtems_printf (printer, "error: cannot lock the linker\n");
      return 1;
    }

    obj = rtems_rtl_find_obj_with_symbol (sym);
    if (obj == NULL)
    {
      rtems_rtl_unlock ();
      rtems_printf (printer, "error: symbol obj not found: %s\n", argv[label]);
      return 1;
    }

    rtems_rtl_obj_dec_reference (obj);

    rtems_rtl_unlock ();
  }

  return 0;
}

static void
rtems_rtl_shell_usage (const rtems_printer* printer, const char* arg)
{
  rtems_printf (printer, "%s: Runtime Linker\n", arg);
  rtems_printf (printer, "  %s [-hl] <command>\n", arg);
  rtems_printf (printer, "   where:\n");
  rtems_printf (printer, "     command: A n RTL command. See -l for a list plus help.\n");
  rtems_printf (printer, "     -h:      This help\n");
  rtems_printf (printer, "     -l:      The command list.\n");
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
      "\tDisplay the object details, obj <name>" },
    { "call", rtems_rtl_shell_call,
      "\tCall a symbol" },
    { "ar", rtems_rtl_shell_archive,
      "\tDisplay the archive details, ar [-ls] <name>" },
    { "trace", rtems_rtl_trace_shell_command,
      "\tControl the RTL trace flags, trace [-h]" }
  };

  rtems_printer printer;
  int           arg;
  int           t;

  rtems_print_printer_printf (&printer);

  for (arg = 1; arg < argc; arg++)
  {
    if (argv[arg][0] != '-')
      break;

    switch (argv[arg][1])
    {
      case 'h':
        rtems_rtl_shell_usage (&printer, argv[0]);
        return 0;
      case 'l':
        rtems_printf (&printer, "%s: commands are:\n", argv[0]);
        for (t = 0;
             t < (sizeof (table) / sizeof (const rtems_rtl_shell_cmd));
             ++t)
          rtems_printf (&printer, "  %s\t%s\n", table[t].name, table[t].help);
        return 0;
      default:
        rtems_printf (&printer, "error: unknown option: %s\n", argv[arg]);
        return 1;
    }
  }

  if ((argc - arg) < 1)
    rtems_printf (&printer, "error: you need to provide a command, try %s -h\n",
                  argv[0]);
  else
  {
    for (t = 0;
         t < (sizeof (table) / sizeof (const rtems_rtl_shell_cmd));
         ++t)
    {
      if (strncmp (argv[arg], table[t].name, strlen (argv[arg])) == 0)
        return table[t].handler (&printer, argc - 1, argv + 1);
    }
    rtems_printf (&printer, "error: command not found: %s (try -h)\n", argv[arg]);
  }

  return 1;
}
