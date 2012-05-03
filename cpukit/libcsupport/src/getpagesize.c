#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <sys/param.h>

/*
 *  Get System Page Size (from SVR4 and 4.2+ BSD)
 *
 *  This is not a functional version but the SPARC backend for at least
 *  gcc 2.8.1 plus gnat 3.13p and gcc 3.0.1 require it to be there and
 *  return a reasonable value.
 */
int getpagesize(void)
{
  return PAGE_SIZE;
}
