/*===============================================================*\
| Project: RTEMS configure remote gdb over serial line            |
+-----------------------------------------------------------------+
| File: serdbgcnf.h                                               |
+-----------------------------------------------------------------+
|                    Copyright (c) 2002 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|               <Thomas.Doerfler@imd-systems.de>                  |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file declares intialization functions to add               |
| a gdb remote debug stub to an RTEMS system                      |
|                                                                 |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 13.05.02  creation                                         doe  |
\*===============================================================*/
/* 
 * $Id$
 */
#ifndef _SERDBGCNF_H
#define _SERDBGCNF_H

#include "serdbg.h"

#ifdef CONFIGURE_INIT

/*
 * fallback for baud rate to use
 */
#ifndef CONFIGURE_SERDBG_BAUDRATE
#define CONFIGURE_SERDBG_BAUDRATE 9600
#endif

/*
 * fallback for device name to use
 */
#ifndef CONFIGURE_SERDBG_DEVNAME
#define CONFIGURE_SERDBG_DEVNAME "/dev/tty01"
#endif

/*
 * fill in serdbg_conf structure
 */
serdbg_conf_t serdbg_conf = {
  CONFIGURE_SERDBG_BAUDRATE,

#ifdef CONFIGURE_SERDBG_CALLOUT
  CONFIGURE_SERDBG_CALLOUT,
#else
  NULL,
#endif

#ifdef CONFIGURE_SERDBG_USE_POLLED_TERMIOS
  serdbg_open,
#else
  NULL,
#endif

  CONFIGURE_SERDBG_DEVNAME,
  
#ifdef CONFIGURE_SERDBG_SKIP_INIT_BKPT
  TRUE,
#else
  FALSE,
#endif
};

int serdbg_init(void) {
#ifdef CONFIGURE_USE_SERDBG
  extern int serdbg_init_dbg(void);
  return serdbg_init_dbg();
#else
  return 0;
#endif
}

#endif /* CONFIGURE_INIT */

#endif /* _SERDBGCNF_H */
