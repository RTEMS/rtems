/**
 *  @file
 *
 *  @brief User Database Access Routines
 *  @ingroup libcsupport
 */

/*
 *  Copyright (c) 1999-2009 Ralf Corsepius <corsepiu@faw.uni-ulm.de>
 *  Copyright (c) 1999-2013 Joel Sherrill <joel.sherrill@OARcorp.com>
 *  Copyright (c) 2000-2001 Fernando Ruiz Casas <fernando.ruiz@ctv.es>
 *  Copyright (c) 2002 Eric Norum <eric.norum@usask.ca>
 *  Copyright (c) 2003 Till Straumann <strauman@slac.stanford.edu>
 *  Copyright (c) 2012 Alex Ivanov <alexivanov97@gmail.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <pthread.h>

#include <rtems/seterr.h>

#include "pwdgrp.h"

static pthread_once_t pwdgrp_once = PTHREAD_ONCE_INIT;

static void init_file(const char *name, const char *content)
{
  FILE *fp = fopen(name, "wx");

  if (fp != NULL) {
    fputs(content, fp);
    fclose(fp);
  }
}

/**
 *  Initialize useable but dummy databases
 */
static void pwdgrp_init(void)
{
  mkdir("/etc", 0777);

  /*
   *  Initialize /etc/passwd
   */
  init_file(
    "/etc/passwd",
    "root:*:0:0:root::/:/bin/sh\n"
      "rtems:*:1:1:RTEMS Application::/:/bin/sh\n"
      "tty:!:2:2:tty owner::/:/bin/false\n"
  );

  /*
   *  Initialize /etc/group
   */
  init_file(
    "/etc/group",
    "root:x:0:root\n"
      "rtems:x:1:rtems\n"
      "tty:x:2:tty\n"
  );
}

void _libcsupport_pwdgrp_init(void)
{
  pthread_once(&pwdgrp_once, pwdgrp_init);
}

/**
 *  Extract a string value from the database
 */
static int
scanString(FILE *fp, char **name, char **bufp, size_t *nleft, int nlFlag)
{
  int c;

  *name = *bufp;
  for (;;) {
    c = getc(fp);
    if (c == ':') {
        if (nlFlag)
            return 0;
        break;
    }
    if (c == '\n') {
        if (!nlFlag)
            return 0;
        break;
    }
    if (c == EOF)
      return 0;
    if (*nleft < 2)
      return 0;
    **bufp = c;
    ++(*bufp);
    --(*nleft);
  }
  **bufp = '\0';
  ++(*bufp);
  --(*nleft);
  return 1;
}

/**
 *  Extract an integer value from the database
 */
static int
scanInt(FILE *fp, int *val)
{
  int c;
  unsigned int i = 0;
  unsigned int limit = INT_MAX;
  int sign = 0;
  int d;

  for (;;) {
    c = getc(fp);
    if (c == ':')
      break;
    if (sign == 0) {
      if (c == '-') {
        sign = -1;
        limit++;
        continue;
      }
      sign = 1;
    }
    if (!isdigit(c))
      return 0;
    d = c - '0';
    if ((i > (limit / 10))
     || ((i == (limit / 10)) && (d > (limit % 10))))
      return 0;
    i = i * 10 + d;
  }
  if (sign == 0)
    return 0;
  *val = i * sign;
  return 1;
}

/*
 * Extract a single password record from the database
 */
int _libcsupport_scanpw(
  FILE *fp,
  struct passwd *pwd,
  char *buffer,
  size_t bufsize
)
{
  int pwuid, pwgid;

  if (!scanString(fp, &pwd->pw_name, &buffer, &bufsize, 0)
   || !scanString(fp, &pwd->pw_passwd, &buffer, &bufsize, 0)
   || !scanInt(fp, &pwuid)
   || !scanInt(fp, &pwgid)
   || !scanString(fp, &pwd->pw_comment, &buffer, &bufsize, 0)
   || !scanString(fp, &pwd->pw_gecos, &buffer, &bufsize, 0)
   || !scanString(fp, &pwd->pw_dir, &buffer, &bufsize, 0)
   || !scanString(fp, &pwd->pw_shell, &buffer, &bufsize, 1))
    return 0;
  pwd->pw_uid = pwuid;
  pwd->pw_gid = pwgid;
  return 1;
}

static int getpw_r(
  const char     *name,
  int             uid,
  struct passwd  *pwd,
  char           *buffer,
  size_t          bufsize,
  struct passwd **result
)
{
  FILE *fp;
  int match;

  _libcsupport_pwdgrp_init();

  if ((fp = fopen("/etc/passwd", "r")) == NULL)
    rtems_set_errno_and_return_minus_one( EINVAL );

  for(;;) {
    if (!_libcsupport_scanpw(fp, pwd, buffer, bufsize))
      goto error_einval;

    if (name) {
      match = (strcmp(pwd->pw_name, name) == 0);
    } else {
      match = (pwd->pw_uid == uid);
    }

    if (match) {
      fclose(fp);
      *result = pwd;
      return 0;
    }
  }
error_einval:
  fclose(fp);
  rtems_set_errno_and_return_minus_one( EINVAL );
}

int getpwnam_r(
  const char     *name,
  struct passwd  *pwd,
  char           *buffer,
  size_t          bufsize,
  struct passwd **result
)
{
  return getpw_r(name, 0, pwd, buffer, bufsize, result);
}

int getpwuid_r(
  uid_t           uid,
  struct passwd  *pwd,
  char           *buffer,
  size_t          bufsize,
  struct passwd **result
)
{
  return getpw_r(NULL, uid, pwd, buffer, bufsize, result);
}

/**
 *  Extract a single group record from the database
 */
int _libcsupport_scangr(
  FILE *fp,
  struct group *grp,
  char *buffer,
  size_t bufsize
)
{
  int grgid;
  char *grmem, *cp;
  int memcount;

  if (!scanString(fp, &grp->gr_name, &buffer, &bufsize, 0)
   || !scanString(fp, &grp->gr_passwd, &buffer, &bufsize, 0)
   || !scanInt(fp, &grgid)
   || !scanString(fp, &grmem, &buffer, &bufsize, 1))
    return 0;
  grp->gr_gid = grgid;

  /*
   * Determine number of members
   */
  for (cp = grmem, memcount = 1 ; *cp != 0 ; cp++) {
    if(*cp == ',')
      memcount++;
  }

  /*
   * Hack to produce (hopefully) a suitably-aligned array of pointers
   */
  if (bufsize < (((memcount+1)*sizeof(char *)) + 15))
    return 0;
  grp->gr_mem = (char **)(((uintptr_t)buffer + 15) & ~15);

  /*
   * Fill in pointer array
   */
  grp->gr_mem[0] = grmem;
  for (cp = grmem, memcount = 1 ; *cp != 0 ; cp++) {
    if(*cp == ',') {
      *cp = '\0';
      grp->gr_mem[memcount++] = cp + 1;
    }
  }
  grp->gr_mem[memcount] = NULL;
  return 1;
}

static int getgr_r(
  const char     *name,
  int             gid,
  struct group   *grp,
  char           *buffer,
  size_t          bufsize,
  struct group  **result
)
{
  FILE *fp;
  int match;

  _libcsupport_pwdgrp_init();

  if ((fp = fopen("/etc/group", "r")) == NULL)
    rtems_set_errno_and_return_minus_one( EINVAL );

  for(;;) {
    if (!_libcsupport_scangr(fp, grp, buffer, bufsize))
      goto error_einval;

    if (name) {
      match = (strcmp(grp->gr_name, name) == 0);
    } else {
      match = (grp->gr_gid == gid);
    }

    if (match) {
      fclose(fp);
      *result = grp;
      return 0;
    }
  }
error_einval:
  fclose(fp);
  rtems_set_errno_and_return_minus_one( EINVAL );
}

int getgrnam_r(
  const char     *name,
  struct group   *grp,
  char           *buffer,
  size_t          bufsize,
  struct group  **result
)
{
  return getgr_r(name, 0, grp, buffer, bufsize, result);
}

int getgrgid_r(
  gid_t           gid,
  struct group   *grp,
  char           *buffer,
  size_t          bufsize,
  struct group  **result
)
{
  return getgr_r(NULL, gid, grp, buffer, bufsize, result);
}
