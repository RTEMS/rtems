/*
 *  POSIX 1003.1b - 9.2.2 - User Database Access Routines
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

/*
 * Static, thread-unsafe, buffers
 */
static FILE *passwd_fp;
static char pwbuf[200];
static struct passwd pwent;
static FILE *group_fp;
static char grbuf[200];
static struct group grent;

/*
 * Initialize useable but dummy databases
 */
void init_etc_passwd_group(void)
{
  FILE *fp;
  static char etc_passwd_initted = 0;

  if (etc_passwd_initted)
    return;
  etc_passwd_initted = 1;
  mkdir("/etc", 0777);

  /*
   *  Initialize /etc/passwd
   */
  if ((fp = fopen("/etc/passwd", "r")) != NULL) {
    fclose(fp);
  }
  else if ((fp = fopen("/etc/passwd", "w")) != NULL) {
    fprintf(fp, "root:*:0:0:root::/:/bin/sh\n"
                 "rtems:*:1:1:RTEMS Application::/:/bin/sh\n"
                 "tty:!:2:2:tty owner::/:/bin/false\n" );
    fclose(fp);
  }

  /*
   *  Initialize /etc/group
   */
  if ((fp = fopen("/etc/group", "r")) != NULL) {
    fclose(fp);
  }
  else if ((fp = fopen("/etc/group", "w")) != NULL) {
    fprintf( fp, "root:x:0:root\n"
                 "rtems:x:1:rtems\n"
                 "tty:x:2:tty\n" );
    fclose(fp);
  }
}

/*
 * Extract a string value from the database
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

/*
 * Extract an integer value from the database
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
static int scanpw(
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

  init_etc_passwd_group();

  if ((fp = fopen("/etc/passwd", "r")) == NULL)
    rtems_set_errno_and_return_minus_one( EINVAL );

  for(;;) {
    if (!scanpw(fp, pwd, buffer, bufsize))
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

struct passwd *getpwnam(
  const char *name
)
{
  struct passwd *p;

  if(getpwnam_r(name, &pwent, pwbuf, sizeof pwbuf, &p))
    return NULL;
  return p;
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

struct passwd *getpwuid(
  uid_t uid
)
{
  struct passwd *p;

  if(getpwuid_r(uid, &pwent, pwbuf, sizeof pwbuf, &p))
    return NULL;
  return p;
}

struct passwd *getpwent(void)
{
  if (passwd_fp == NULL)
    return NULL;
  if (!scanpw(passwd_fp, &pwent, pwbuf, sizeof pwbuf))
    return NULL;
  return &pwent;
}

void setpwent(void)
{
  init_etc_passwd_group();

  if (passwd_fp != NULL)
    fclose(passwd_fp);
  passwd_fp = fopen("/etc/passwd", "r");
}

void endpwent(void)
{
  if (passwd_fp != NULL)
    fclose(passwd_fp);
}

/*
 * Extract a single group record from the database
 */
static int scangr(
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

  init_etc_passwd_group();

  if ((fp = fopen("/etc/group", "r")) == NULL)
    rtems_set_errno_and_return_minus_one( EINVAL );

  for(;;) {
    if (!scangr(fp, grp, buffer, bufsize))
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

struct group *getgrnam(
  const char *name
)
{
  struct group *p;

  if(getgrnam_r(name, &grent, grbuf, sizeof grbuf, &p))
    return NULL;
  return p;
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

struct group *getgrgid(
  gid_t gid
)
{
  struct group *p;

  if(getgrgid_r(gid, &grent, grbuf, sizeof grbuf, &p))
    return NULL;
  return p;
}

struct group *getgrent(void)
{
  if (group_fp == NULL)
    return NULL;
  if (!scangr(group_fp, &grent, grbuf, sizeof grbuf))
    return NULL;
  return &grent;
}

void setgrent(void)
{
  init_etc_passwd_group();

  if (group_fp != NULL)
    fclose(group_fp);
  group_fp = fopen("/etc/group", "r");
}

void endgrent(void)
{
  if (group_fp != NULL)
    fclose(group_fp);
}
