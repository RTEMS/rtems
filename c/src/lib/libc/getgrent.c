/*
 *  POSIX 1003.1b - 9.2.1 - Group Database Access Routines
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
#include <grp.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static struct group gr_group;    /* password structure */
static FILE *group_fp;

/*
 *  The size of these buffers is arbitrary and there is no provision
 *  to protect any of them from overflowing.  The scanf patterns
 *  need to be changed to prevent overflowing.  In addition,
 *  the limits on these needs to be examined.
 */

static char groupname[8];
static char password[1024];
static char groups[1024];
static char *gr_mem[16] = { } ;

extern void init_etc_passwd_group(void);

int getgrnam_r(
  const char     *name,
  struct group   *grp,
  char           *buffer,
  size_t          bufsize,
  struct group  **result
)
{
  FILE *fp;

  init_etc_passwd_group();

  if ((fp = fopen ("/etc/group", "r")) == NULL) {
    errno = EINVAL;
    return -1;
  }

  while (fgets (buffer, bufsize, fp)) {
    sscanf (buffer, "%[^:]:%[^:]:%d:%s\n",
      groupname, password, (int *) &grp->gr_gid,
      groups);
    grp->gr_name = groupname;
    grp->gr_passwd = password;
    grp->gr_mem = gr_mem ;
    
    if (!strcmp (groupname, name)) {
      fclose (fp);
      *result = grp;
      return 0;
    }
  }
  fclose (fp);
  errno = EINVAL;
  return -1;
}

struct group *getgrnam(
  const char *name
)
{
  char   buf[1024];
  struct group *g;

  if ( getgrnam_r( name, &gr_group, buf, 1024, &g ) )
    return NULL;

  return g;
}

int getgrgid_r(
  gid_t           gid,
  struct group   *grp,
  char           *buffer,
  size_t          bufsize,
  struct group  **result
)
{
  FILE *fp;

  init_etc_passwd_group();

  if ((fp = fopen ("/etc/group", "r")) == NULL) {
    errno = EINVAL;
    return -1;
  }

  while (fgets (buffer, bufsize, fp)) {
    sscanf (buffer, "%[^:]:%[^:]:%d:%s\n",
      groupname, password, (int *) &gr_group.gr_gid,
      groups);
    gr_group.gr_name = groupname;
    gr_group.gr_passwd = password;
    gr_group.gr_mem = gr_mem ;


    if (gid == gr_group.gr_gid) {
      fclose (fp);
      *result = grp;
      return 0;
    }
  }
  fclose (fp);
  errno = EINVAL;
  return -1;
}

struct group *getgrgid (
  gid_t gid
)
{
  char   buf[1024];
  struct group *g;

  if ( getgrgid_r( gid, &gr_group, buf, 1024, &g ) )
    return NULL;

  return g;
}

struct group *getgrent( void )
{
  char buf[1024];

  if (group_fp == NULL)
    return NULL;

  if (fgets (buf, sizeof (buf), group_fp) == NULL)
    return NULL;

  sscanf (buf, "%[^:]:%[^:]:%d:%s\n",
    groupname, password, (int *) &gr_group.gr_gid,
    groups);
  gr_group.gr_name = groupname;
  gr_group.gr_passwd = password;
  gr_group.gr_mem = gr_mem ;

  return &gr_group;
}

void
setgrent ()
{
  init_etc_passwd_group();

  if (group_fp != NULL)
    fclose (group_fp);

  group_fp = fopen ("/etc/group", "r");
}

void
endgrent ()
{
  if (group_fp != NULL)
    fclose (group_fp);
}
