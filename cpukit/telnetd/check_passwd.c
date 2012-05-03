/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2003-2007
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
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
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <termios.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

#include <rtems/telnetd.h>

#include "passwd.h"

char *__des_crypt_r( const char *, const char *, char *, int);

/**
 * @brief Standard Telnet login check that uses DES to encrypt the passphrase.
 *
 * Takes a @a passphrase, encrypts it and compares it to the encrypted
 * passphrase in the @c TELNETD_PASSWD environment variable.  No password is
 * required if @c TELNETD_PASSWD is unset.  The argument @a user is ignored.
 */
bool rtems_telnetd_login_check(
  const char *user,
  const char *passphrase
)
{
  char *pw = getenv( "TELNETD_PASSWD");
  char cryptbuf [21];
  char salt [3];

  if (pw == NULL || strlen( pw) == 0) {
    #ifdef TELNETD_DEFAULT_PASSWD
      pw = TELNETD_DEFAULT_PASSWD;
    #else
      return true;
    #endif
  }

  strncpy( salt, pw, 2);
  salt [2] = '\0';

  return strcmp(
    __des_crypt_r( passphrase, salt, cryptbuf, sizeof( cryptbuf)),
    pw
  ) == 0;
}
