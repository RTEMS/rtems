/*
 *  $Id$
 */

#include <stdio.h>
#include <sys/types.h>
#include <grp.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

static struct group gr_group;	/* password structure */
static FILE *group_fp;

static char groupname[8];
static char password[1024];
static char groups[1024];
static char *gr_mem[16] = { } ;

struct group *
getgrnam (name)
     const char *name;
{
  FILE *fp;
  char buf[1024];

  if ((fp = fopen ("/etc/group", "r")) == NULL)
    {
      return NULL;
    }

  while (fgets (buf, sizeof (buf), fp))
    {
      sscanf (buf, "%[^:]:%[^:]:%d:%s\n",
	      groupname, password, &gr_group.gr_gid,
	      groups);
      gr_group.gr_name = groupname;
      gr_group.gr_passwd = password;
      gr_group.gr_mem = gr_mem ;
      
      if (!strcmp (groupname, name))
	{
	  fclose (fp);
	  return &gr_group;
	}
    }
  fclose (fp);
  return NULL;
}

struct group *
getgrgid (gid_t gid)
{
  FILE *fp;
  char buf[1024];

  if ((fp = fopen ("/etc/group", "r")) == NULL)
    {
      return NULL;
    }

  while (fgets (buf, sizeof (buf), fp))
    {
      sscanf (buf, "%[^:]:%[^:]:%d:%s\n",
	      groupname, password, &gr_group.gr_gid,
	      groups);
      gr_group.gr_name = groupname;
      gr_group.gr_passwd = password;
      gr_group.gr_mem = gr_mem ;


      if (gid == gr_group.gr_gid)
	{
	  fclose (fp);
	  return &gr_group;
	}
    }
  fclose (fp);
  return NULL;
}

struct group *
getgrent ()
{
  char buf[1024];

  if (group_fp == NULL)
    return NULL;

  if (fgets (buf, sizeof (buf), group_fp) == NULL)
    return NULL;

  sscanf (buf, "%[^:]:%[^:]:%d:%s\n",
	  groupname, password, &gr_group.gr_gid,
	  groups);
  gr_group.gr_name = groupname;
  gr_group.gr_passwd = password;
  gr_group.gr_mem = gr_mem ;

  return &gr_group;
}

void
setgrent ()
{
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
