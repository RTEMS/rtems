/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

/*PAGE
 *
 *  Get System Page Size (from SVR4 and 4.2+ BSD)
 *
 *  This is not a functional version but the SPARC backend for at least
 *  gcc 2.8.1 plus gnat 3.13p and gcc 3.0.1 require it to be there and
 *  return a reasonable value.
 */

size_t getpagesize(void)
{
  return 4096;
}
