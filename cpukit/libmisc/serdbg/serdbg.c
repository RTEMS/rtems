/*===============================================================*\
| Project: RTEMS remote gdb over serial line                      |
+-----------------------------------------------------------------+
| File: serdbg.c                                                  |
+-----------------------------------------------------------------+
|                    Copyright (c) 2002 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|               <Thomas.Doerfler@imd-systems.de>                  |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file contains intialization and utility functions to add   |
| a gdb remote debug stub to an RTEMS system                      |
|                                                                 |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 04.04.02  creation                                         doe  |
\*===============================================================*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <rtems/serdbg.h>


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int serdbg_init_dbg
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize remote gdb session over serial line                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    rtems_status_code                                                      |
\*=========================================================================*/
{
  static bool is_initialized = false;

  rtems_status_code rc = RTEMS_SUCCESSFUL;

  if (is_initialized) {
    return RTEMS_SUCCESSFUL;
  }
  is_initialized = true;
  /*
   * try to open serial device
   */
  if (rc == RTEMS_SUCCESSFUL) {
    if ((serdbg_conf.open_io != NULL) &&
	(0 > serdbg_conf.open_io(serdbg_conf.devname,serdbg_conf.baudrate))) {
      fprintf(stderr,
	      "remote_gdb_init: cannot open device %s "
	      "for gdb connection:%s\n",serdbg_conf.devname,strerror(errno));
      rc = RTEMS_IO_ERROR;
    }
  }
  /*
   * initialize gdb stub
   */
  if (rc == RTEMS_SUCCESSFUL) {
    set_debug_traps();
  }
  /*
   * now activate gdb stub
   */
  if ((rc == RTEMS_SUCCESSFUL) &&
      !serdbg_conf.skip_init_bkpt) {
    breakpoint();
  }

  /*
   * return to original function
   * this may be already unter gdb control
   */
  return rc;
}
