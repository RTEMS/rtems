/*===============================================================*\
| Project: RTEMS remote gdb over serial line                      |
+-----------------------------------------------------------------+
| File: termios_printk.h                                          |
+-----------------------------------------------------------------+
|                    Copyright (c) 2002 IMD                       |
|      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler      |
|               <Thomas.Doerfler@imd-systems.de>                  |
|                       all rights reserved                       |
+-----------------------------------------------------------------+
| this file declares intialization functions to add               |
| printk polled output via termios polled drivers                 |
|                                                                 |
+-----------------------------------------------------------------+
|   date                      history                        ID   |
| ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ |
| 13.04.02  creation                                         doe  |
\*===============================================================*/
#ifndef _TERMIOS_PRINTK_H
#define _TERMIOS_PRINTK_H

#include <rtems.h>
#include <termios.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  uint32_t   baudrate;      /* debug baud rate, e.g. 57600            */
  void (*callout)(void);    /* callout pointer during polling         */
  const char *devname;      /* debug device, e.g. "/dev/tty01"        */
} termios_printk_conf_t;

/*
 * must be defined in init module...
 */
extern termios_printk_conf_t termios_printk_conf;

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void termios_printk_outputchar
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|    send one character to serial port                                      |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
(
 char c  /* character to print */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    <none>                                                                 |
\*=========================================================================*/

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int termios_printk_inputchar
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|    wait for one character from serial port                                |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
(
 void  /* none */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    received character                                                     |
\*=========================================================================*/


/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
int termios_printk_open

/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|    try to open given serial debug port                                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
(
 const char *dev_name, /* name of device to open */
 uint32_t   baudrate   /* baud rate to use       */
 );
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    0 on success, -1 and errno otherwise                                   |
\*=========================================================================*/

#ifdef __cplusplus
}
#endif

#endif /* _TERMIOS_PRINTK_H */
