/*
 *  ttyname_r() - POSIX 1003.1b 4.7.2 - Demetermine Terminal Device Name
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <termios.h>
#include <unistd.h>
#include <paths.h>
#include <_syslist.h>
#include <errno.h>

#include <rtems/libio_.h>
#include <rtems/seterr.h>

int ttyname_r(
  int   fd,
  char *name,
  int   namesize
)
{
  struct stat sb;
  struct termios tty;
  struct dirent *dirp;
  DIR *dp;
  struct stat dsb;
  char *rval;

  /* Must be a terminal. */
  if (tcgetattr (fd, &tty) < 0)
    rtems_set_errno_and_return_minus_one(EBADF);

  /* Must be a character device. */
  if (fstat (fd, &sb) || !S_ISCHR (sb.st_mode))
    rtems_set_errno_and_return_minus_one(EBADF);

  if ((dp = opendir (_PATH_DEV)) == NULL)
    rtems_set_errno_and_return_minus_one(EBADF);

  for (rval = NULL; (dirp = readdir (dp)) != NULL ;)
    {
      if (dirp->d_ino != sb.st_ino)
	continue;
      strcpy (name + sizeof (_PATH_DEV) - 1, dirp->d_name);
      if (stat (name, &dsb) || sb.st_dev != dsb.st_dev ||
	  sb.st_ino != dsb.st_ino)
	continue;
      rval = name;
      break;
    }
  (void) closedir (dp);
  return 0;
}

static char buf[sizeof (_PATH_DEV) + MAXNAMLEN] = _PATH_DEV;

/*
 *  ttyname() - POSIX 1003.1b 4.7.2 - Demetermine Terminal Device Name
 */

char *ttyname(
  int fd
)
{
  if ( !ttyname_r( fd, buf, sizeof(buf) ) )
    return buf;
  return NULL;
}

