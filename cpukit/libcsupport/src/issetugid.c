/**
 *  @file
 *
 *  @brief Dummy Version of BSD Routine
 *  @ingroup libcsupport
 */


#if HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(RTEMS_NEWLIB) && !defined(HAVE_ISSETUGID)

/*
 *  Prototype to avoid warnings
 */
int issetugid (void);

/**
 *  Dummy version of BSD routine
 */
int issetugid (void)
{
  return 0;
}
#endif
