/**
 * @file
 *
 * @author Sebastian Huber <sebastian.huber@embedded-brains.de>
 *
 * @brief Shell login prompt functions.
 */

/*
 * Copyright (c) 2009
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * Based on work from Chris Johns, Fernando Ruiz and Till Straumann.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>

#include <rtems/login.h>

static int rtems_shell_discard( int c, FILE *stream)
{
  return c;
}

static void rtems_shell_get_text(
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
    return;
  }

  tcdrain( fd_in);
  if (out != NULL){
    tcdrain( fileno( out));
  }

  while (true) {
    int c = fgetc( in);

    switch (c) {
      case EOF:
        /* Here comes an ugly hack: The Termios driver's read() handler returns
         * 0 to the C library's fgets() if it times out.  fgets() interprets
         * this (correctly) as EOF, a condition we want to undo since it's not
         * really true since we really have a read error (Termios bug?).
         *
         * As a workaround we push something back and read it again.  This
         * should simply reset the EOF condition.
         */
        if (ungetc( '?', in) == '?') {
          fgetc( in);
        }
        break;
      case '\n':
      case '\r':
        put( '\n', out);
        line [i] = '\0';
        return;
      case  127:
      case '\b':
        if (i > 0) {
          put( '\b', out);
          put( ' ', out);
          put( '\b', out);
          --i;
        } else {
          put( '\a', out);
        }
        break;
      default:
        if (!iscntrl( c)) {
          if (i < size - 1) {
            line [i] = (char) c;
            ++i;
            put( c, out);
          } else {
            put( '\a', out);
          }
        } else {
          put( '\a', out);
        }
        break;
    }
  }
}

bool rtems_shell_login_prompt(
  FILE *in,
  FILE *out,
  const char *device,
  rtems_login_check check
)
{
  int fd_in = fileno( in);
  struct termios termios_previous;
  bool restore_termios = false;
  int i = 0;
  bool result = false;

  if (tcgetattr( fd_in, &termios_previous) == 0) {
    struct termios termios_new = termios_previous;

    termios_new.c_lflag &= ~ECHO;
    termios_new.c_lflag &= ~ICANON;
    termios_new.c_cc [VTIME] = 255;
    termios_new.c_cc [VMIN] = 0;

    restore_termios = tcsetattr( fd_in, TCSANOW, &termios_new) == 0;
  }

  for (i = 0; i < 3; ++i) {
    char user [32];
    char passphrase [128];

    fprintf( out, "%s login: ", device);
    fflush( out);
    rtems_shell_get_text( in, out, user, sizeof( user));

    fflush( in);
    fprintf( out, "Password: ");
    fflush( out);
    rtems_shell_get_text( in, NULL, passphrase, sizeof( passphrase));
    fputc( '\n', out);

    result = check( user, passphrase);
    if (result) {
      break;
    }

    fprintf( out, "Login incorrect\n\n");
    sleep( 2);
  }

  if (restore_termios) {
    /* What to do if restoring the flags fails? */
    tcsetattr( fd_in, TCSANOW, &termios_previous);
  }

  return result;
}
