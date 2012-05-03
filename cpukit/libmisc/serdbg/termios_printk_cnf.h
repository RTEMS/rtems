/*===============================================================*\
| Project: RTEMS configure remote gdb over serial line            |
+-----------------------------------------------------------------+
| File: termios_printk_cnf.h                                      |
+-----------------------------------------------------------------+
|                    Copyright (c) 2002 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|               <Thomas.Doerfler@imd-systems.de>                  |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file declares intialization functions to add               |
| printk support via polled termios                               |
|                                                                 |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 13.05.02  creation                                         doe  |
\*===============================================================*/
#ifndef _TERMIOS_PRINTK_CNF_H
#define _TERMIOS_PRINTK_CNF_H

#include <rtems/termios_printk.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CONFIGURE_INIT

/*
 * fallback for baud rate to use
 */
#ifndef CONFIGURE_TERMIOS_PRINTK_BAUDRATE
#define CONFIGURE_TERMIOS_PRINTK_BAUDRATE 9600
#endif

/*
 * fallback for device name to use
 */
#ifndef CONFIGURE_TERMIOS_PRINTK_DEVNAME
#define CONFIGURE_TERMIOS_PRINTK_DEVNAME "/dev/console"
#endif

#ifdef CONFIGURE_USE_TERMIOS_PRINTK
/*
 * fill in termios_printk_conf structure
 */
termios_printk_conf_t termios_printk_conf = {
  CONFIGURE_TERMIOS_PRINTK_BAUDRATE,

#ifdef CONFIGURE_TERMIOS_PRINTK_CALLOUT
  CONFIGURE_TERMIOS_PRINTK_CALLOUT,
#else
  NULL,
#endif
  CONFIGURE_TERMIOS_PRINTK_DEVNAME,
};
#endif

int termios_printk_init(void) {
#ifdef CONFIGURE_USE_TERMIOS_PRINTK
  return termios_printk_open(termios_printk_conf.devname,
			     termios_printk_conf.baudrate);
#else
  return 0;
#endif
}

#endif /* CONFIGURE_INIT */

#ifdef __cplusplus
}
#endif

#endif /* _TERMIOS_PRINTK_CNF_H */
