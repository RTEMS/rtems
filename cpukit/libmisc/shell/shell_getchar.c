/*
 *
 *  Handle keys for the shell.
 *
 *  Author:
 *
 *   Chris Johns (chrisj@rtems.org)
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <time.h>

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/system.h>
#include <rtems/shell.h>
#include <rtems/shellconfig.h>
#include "internal.h"

/*
 * Taken from the monitor code.
 */

/*
 * Translation tables. Not sure if this is the best way to
 * handle this, how-ever I wish to avoid the overhead of
 * including a more complete and standard environment such
 * as ncurses.
 */

struct translation_table
{
  char                     expecting;
  const struct translation_table *branch;
  unsigned int             key;
};

static const struct translation_table trans_one[] =
{
  { '\x7e', 0, RTEMS_SHELL_KEYS_HOME },
  { 0,      0, 0 }
};

static const struct translation_table trans_two[] =
{
  { '~', 0, RTEMS_SHELL_KEYS_INS },
  { 0,   0, 0 }
};

static const struct translation_table trans_three[] =
{
  { '~', 0, RTEMS_SHELL_KEYS_DEL },
  { 0,   0, 0 }
};

static const struct translation_table trans_tab_csi[] =
{
  { '1', trans_one,   0 },
  { '2', trans_two,   0 },
  { '3', trans_three, 0 },
  { 'A', 0,           RTEMS_SHELL_KEYS_UARROW },
  { 'B', 0,           RTEMS_SHELL_KEYS_DARROW },
  { 'D', 0,           RTEMS_SHELL_KEYS_LARROW },
  { 'C', 0,           RTEMS_SHELL_KEYS_RARROW },
  { 'F', 0,           RTEMS_SHELL_KEYS_END },
  { 'H', 0,           RTEMS_SHELL_KEYS_HOME },
  { 0,   0,           0 }
};

static const struct translation_table trans_tab_O[] =
{
  { '1', 0, RTEMS_SHELL_KEYS_F1 },
  { '2', 0, RTEMS_SHELL_KEYS_F2 },
  { '3', 0, RTEMS_SHELL_KEYS_F3 },
  { '4', 0, RTEMS_SHELL_KEYS_F4 },
  { '5', 0, RTEMS_SHELL_KEYS_F5 },
  { '6', 0, RTEMS_SHELL_KEYS_F6 },
  { '7', 0, RTEMS_SHELL_KEYS_F7 },
  { '8', 0, RTEMS_SHELL_KEYS_F8 },
  { '9', 0, RTEMS_SHELL_KEYS_F9 },
  { ':', 0, RTEMS_SHELL_KEYS_F10 },
  { 'F', 0, RTEMS_SHELL_KEYS_END },
  { 'P', 0, RTEMS_SHELL_KEYS_F1 },
  { 'Q', 0, RTEMS_SHELL_KEYS_F2 },
  { 'R', 0, RTEMS_SHELL_KEYS_F3 },
  { 'S', 0, RTEMS_SHELL_KEYS_F4 },
  { 'T', 0, RTEMS_SHELL_KEYS_F5 },
  { 'U', 0, RTEMS_SHELL_KEYS_F6 },
  { 'V', 0, RTEMS_SHELL_KEYS_F7 },
  { 'W', 0, RTEMS_SHELL_KEYS_F8 },
  { 'X', 0, RTEMS_SHELL_KEYS_F9 },
  { 'Y', 0, RTEMS_SHELL_KEYS_F10 },
  { 0,   0, 0 }
};

static const struct translation_table trans_tab[] =
{
  { '[', trans_tab_csi, 0 },    /* CSI command sequences */
  { 'O', trans_tab_O,   0 },    /* O are the fuction keys */
  { 0,   0,             0 }
};

/*
 * Perform a basic tranlation for some ANSI/VT100 key codes.
 * This code could do with a timeout on the ESC as it is
 * now lost from the input stream. It is not* used by the
 * line editor below so considiered not worth the effort.
 */

unsigned int
rtems_shell_getchar (FILE *in)
{
  const struct translation_table *translation = 0;
  for (;;)
  {
    int c = fgetc (in);
    if (c == EOF)
      return EOF;
    if (c == 27)
      translation = trans_tab;
    else
    {
      /*
       * If no translation happing just pass through
       * and return the key.
       */
      if (translation)
      {
        /*
         * Scan the current table for the key, and if found
         * see if this key is a fork. If so follow it and
         * wait else return the extended key.
         */
        int index    = 0;
        int branched = 0;
        while ((translation[index].expecting != '\0') ||
               (translation[index].key != '\0'))
        {
          if (translation[index].expecting == c)
          {
            /*
             * A branch is take if more keys are to come.
             */
            if (translation[index].branch == 0)
              return RTEMS_SHELL_KEYS_EXTENDED | translation[index].key;
            else
            {
              translation = translation[index].branch;
              branched    = 1;
              break;
            }
          }
          index++;
        }
        /*
         * Who knows what these keys are, just drop them.
         */
        if (!branched)
          translation = 0;
      }
      else
        return c;
    }
  }
}

