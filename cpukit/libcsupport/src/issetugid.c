/*
 *  Dummy version of BSD routine
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE_ISSETUGID)
int issetugid (void)
{
  return 0;
}
#endif
