/**
 *  @file
 *
 *  @brief Read a Directory
 *  @ingroup libcsupport
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef HAVE_READDIR_R

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>

/**
 *  The RTEMS version of readdir is already thread-safe.
 *  This routine is reentrant version of readdir().
 */
int readdir_r(
  DIR *__restrict dirp,
  struct dirent *__restrict entry,
  struct dirent **__restrict result
)
{
     *result = readdir(dirp);
     if (*result)
         *entry = **result;
     return *result ? 0 : errno;
}

#endif
