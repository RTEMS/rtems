/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

/*PAGE
 *
 *  12.2.3 Change Memory Protection, P1003.1b-1996, p. 277.
 *
 *  This is not a functional version but the SPARC backend for at least
 *  gcc 2.8.1 plus gnat 3.13p and gcc 3.0.1 require it to be there and
 *  return 0.
 */

int mprotect(const void *addr, size_t len, int prot)
{
  return 0;
}
