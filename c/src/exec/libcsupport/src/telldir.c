/*
 *  telldir() - XXX
 * 
 *  $Id$
 */

#include <sys/param.h>
#include <assert.h>
#include <dirent.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <rtems.h>
#include "libio.h"

long telldir(
  DIR *dirp
)
{
  errno = ENOSYS;
  return -1;
}
