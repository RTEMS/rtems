/*
 *  $Id$
 */

/* creat() "system call" */

#if HAVE_CONFIG_H
#include "config.h"
#endif

/* This is needed by f2c and therefore the SPEC benchmarks.  */

#include <fcntl.h>

int
creat (const char *path, mode_t mode)
{
  return open (path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}
