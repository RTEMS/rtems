/* devctl.h
 *
 */

#ifndef __POSIX_DEVICE_CONTROL_h
#define __POSIX_DEVICE_CONTROL_h

#include <rtems/posix/features.h>

#if defined(_POSIX_DEVICE_CONTROL)

#include <sys/types.h>
#include <unistd.h>

/*
 *  21.2.1 Control a Device, P1003.4b/D8, p. 65
 */

int devctl(
  int     filedes,
  void   *dev_data_ptr,
  size_t  nbyte,
  int    *dev_info_ptr
);

#endif

#endif
/* end of include file */
