/**
 * @file
 *
 * @brief Shell login prompt functions.
 */

/*
 * Authorship
 * ----------
 * Parts of this software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2003-2007
 *         Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * Parts of this software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 *         under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 *
 * Copyright (c) 2009 embedded brains GmbH and others.
 *
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * Based on work from Chris Johns, Fernando Ruiz and Till Straumann.
 *
 * Derived from files "cpukit/libmisc/shell/shell.c" and
 * "cpukit/telnetd/check_passwd.c".
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>

#include <rtems/shell.h>

static int rtems_shell_discard( int c, FILE *stream)
{
  return c;
}

static bool rtems_shell_get_text(
  FILE *in,
  FILE *out,
  char *line,
  size_t size
)
{
  int fd_in = fileno( in);
  int (*put)( int, FILE *) =
    out != NULL && isatty( fd_in)
      ? fputc
      : rtems_shell_discard;
  size_t i = 0;

  if (size < 1) {
    return false;
  }

  tcdrain( fd_in);
  if (out != NULL){
    tcdrain( fileno(out) );
  }

  while (true) {
    int c = fgetc(in);

    switch (c) {
      case EOF:
        clearerr( in );
        return false;
      case '\n':
      case '\r':
        put('\n', out);
        line [i] = '\0';
        return true;
      case  127:
      case '\b':
        if (i > 0) {
          put('\b', out);
          put(' ', out);
          put('\b', out);
          --i;
        } else {
          put('\a', out);
        }
        break;
      default:
        if (!iscntrl( c)) {
          if (i < size - 1) {
            line [i] = (char) c;
            ++i;
            put( c, out);
          } else {
            put('\a', out);
          }
        } else {
          put('\a', out);
        }
        break;
    }
  }
  return true;
}

bool rtems_shell_login_prompt(
  FILE *in,
  FILE *out,
  const char *device,
  rtems_shell_login_check_t check
)
{
  int fd_in = fileno(in);
  struct termios termios_previous;
  bool restore_termios = false;
  int i = 0;
  bool result = false;

  if (tcgetattr( fd_in, &termios_previous) == 0) {
    struct termios termios_new = termios_previous;

    /*
     *  Stay in canonical mode so we can tell EOF and dropped connections.
     *  But read one character at a time and do not echo it.
     */
    termios_new.c_lflag &= (unsigned char) ~ECHO;
    termios_new.c_cc [VTIME] = 0;
    termios_new.c_cc [VMIN] = 1;

    restore_termios = tcsetattr( fd_in, TCSANOW, &termios_new) == 0;
  }

  for (i = 0; i < 3; ++i) {
    char user [32];
    char passphrase [128];

    fprintf( out, "%s login: ", device );
    fflush( out );
    result = rtems_shell_get_text( in, out, user, sizeof(user) );
    if ( !result )
      break;

    fflush( in);
    fprintf( out, "Password: ");
    fflush( out);
    result = rtems_shell_get_text( in, NULL, passphrase, sizeof(passphrase) );
    if ( !result )
      break;
    fputc( '\n', out);

    result = check( user, passphrase );
    if (result)
      break;

    fprintf( out, "Login incorrect\n\n");
    sleep( 2);
  }

  if (restore_termios) {
    /* What to do if restoring the flags fails? */
    tcsetattr( fd_in, TCSANOW, &termios_previous);
  }

  return result;
}
