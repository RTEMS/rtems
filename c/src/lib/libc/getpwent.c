/*
 *  POSIX 1003.1b - 9.2.2 - User Database Access Routines
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <unistd.h>

static struct passwd pw_passwd;  /* password structure */
static FILE *passwd_fp;

/*
 *  The size of these buffers is arbitrary and there is no provision
 *  to protect any of them from overflowing.  The scanf patterns
 *  need to be changed to prevent overflowing.  In addition,
 *  the limits on these needs to be examined.
 */

static char logname[8];
static char password[1024];
static char comment[1024];
static char gecos[1024];
static char dir[1024];
static char shell[1024];

/*
 *  Initialize a useable but dummy /etc/passwd
 *
 *  NOTE: Ignore all errors.
 *
 */

static char etc_passwd_initted = 0;

void init_etc_passwd_group(void)
{
  FILE *fp;

  if ( etc_passwd_initted )
    return;
  etc_passwd_initted = 1;
  
  (void) mkdir( "/etc", S_IRWXU | S_IRWXG | S_IRWXO );

  /*
   *  Initialize /etc/passwd
   */

  if ((fp = fopen ("/etc/passwd", "w")) == NULL)
    return;

  fprintf( fp, "root:*:0:0:root,,,,:/tmp:/bin/false\n"
               "rtems:*:1:1:RTEMS Application,,,,:/tmp:/bin/false\n" );

  fclose( fp );

  /*
   *  Initialize /etc/group
   */

  if ((fp = fopen ("/etc/group", "w")) == NULL)
    return;

  fprintf( fp, "root::0:root\n"
               "rtems::0:rtems\n" );

  fclose( fp );
}

int getpwnam_r(
  const char     *name,
  struct passwd  *pwd,
  char           *buffer,
  size_t          bufsize,
  struct passwd **result
)
{
  FILE *fp;

  init_etc_passwd_group();

  if ((fp = fopen ("/etc/passwd", "r")) == NULL) {
    errno = EINVAL;
    return -1;
  }

  while (fgets (buffer, bufsize, fp)) {
    sscanf (buffer, "%[^:]:%[^:]:%d:%d:%[^:]:%[^:]:%[^:]:%s\n",
        logname, password, &pwd->pw_uid,
        &pwd->pw_gid, comment, gecos,
        dir, shell);
    pwd->pw_name = logname;
    pwd->pw_passwd = password;
    pwd->pw_comment = comment;
    pwd->pw_gecos = gecos;
    pwd->pw_dir = dir;
    pwd->pw_shell = shell;

    if (!strcmp (logname, name)) {
      fclose (fp);
      *result = pwd;
      return 0;
    }
  }
  fclose (fp);
  errno = EINVAL;
  return -1;
}

struct passwd *getpwnam(
  const char *name
)
{
  char   buf[1024];
  struct passwd *p;

  if ( getpwnam_r( name, &pw_passwd, buf, 1024, &p ) )
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
  FILE *fp;

  init_etc_passwd_group();

  if ((fp = fopen ("/etc/passwd", "r")) == NULL) {
    errno = EINVAL;
    return -1;
  }

  while (fgets (buffer, bufsize, fp)) {
    sscanf (buffer, "%[^:]:%[^:]:%d:%d:%[^:]:%[^:]:%[^:]:%s\n",
     logname, password, &pw_passwd.pw_uid,
     &pw_passwd.pw_gid, comment, gecos,
     dir, shell);
    pwd->pw_name = logname;
    pwd->pw_passwd = password;
    pwd->pw_comment = comment;
    pwd->pw_gecos = gecos;
    pwd->pw_dir = dir;
    pwd->pw_shell = shell;

    if (uid == pwd->pw_uid) {
      fclose (fp);
      *result = pwd;
      return 0;
    }
  }
  fclose (fp);
  errno = EINVAL;
  return -1;
}

struct passwd *getpwuid(
  uid_t uid
)
{
  char   buf[1024];
  struct passwd *p;

  if ( getpwuid_r( uid, &pw_passwd, buf, 1024, &p ) )
    return NULL;

  return p;
}

struct passwd *getpwent()
{
  char buf[1024];

  if (passwd_fp == NULL)
    return NULL;

  if (fgets (buf, sizeof (buf), passwd_fp) == NULL)
    return NULL;

  sscanf (buf, "%[^:]:%[^:]:%d:%d:%[^:]:%[^:]:%[^:]:%s\n",
    logname, password, &pw_passwd.pw_uid,
    &pw_passwd.pw_gid, comment, gecos,
    dir, shell);
  pw_passwd.pw_name = logname;
  pw_passwd.pw_passwd = password;
  pw_passwd.pw_comment = comment;
  pw_passwd.pw_gecos = gecos;
  pw_passwd.pw_dir = dir;
  pw_passwd.pw_shell = shell;

  return &pw_passwd;
}

void setpwent( void )
{
  init_etc_passwd_group();

  if (passwd_fp != NULL)
    fclose (passwd_fp);

  passwd_fp = fopen ("/etc/passwd", "r");
}

void endpwent( void )
{
  if (passwd_fp != NULL)
    fclose (passwd_fp);
}
