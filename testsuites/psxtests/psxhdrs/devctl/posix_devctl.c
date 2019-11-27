/*
 *  COPYRIGHT (c) 2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#define _POSIX_26_C_SOURCE

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <devctl.h>

int test( void );

int test( void )
{
  int     result;
  int     fd;
  int     dcmd;
  void   *dev_data_ptr;
  size_t  nbyte;
  int     dev_info;

  fd           = -1;
  dcmd         = 0;
  dev_data_ptr = NULL;
  nbyte        = 0;

  result = posix_devctl(fd, dcmd, dev_data_ptr, nbyte, &dev_info);

  return result;
}
