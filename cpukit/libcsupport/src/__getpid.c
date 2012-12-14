/**
 *  @file
 *
 *  @brief Get Process Id
 *  @ingroup libcsupport
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>

/**
 *  Some C Libraries reference this routine since they think getpid is
 *  a real system call.
 */
pid_t __getpid(void)
{
  return getpid();
}
