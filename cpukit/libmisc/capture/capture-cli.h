/**
 * @file rtems/capture-cli.h
 *
 * This is the Target Interface Command Line Interface. You need
 * start the RTEMS monitor.
 */

/*
  ------------------------------------------------------------------------

  Copyright Objective Design Systems Pty Ltd, 2002
  All rights reserved Objective Design Systems Pty Ltd, 2002
  Chris Johns (ccj@acm.org)

  COPYRIGHT (c) 1989-2014.
  On-Line Applications Research Corporation (OAR).

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution.

  This software with is provided ``as is'' and with NO WARRANTY.

  ------------------------------------------------------------------------

  RTEMS Performance Monitoring and Measurement Framework.

  This is the Target Interface Command Line Interface. You need
  start the RTEMS monitor.

*/

#ifndef __CAPTURE_CLI_H_
#define __CAPTURE_CLI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/capture.h>

/**
 * rtems_capture_cli_init
 *
 * This function initialises the command line interface to the capture
 * engine.
 */
rtems_status_code
rtems_capture_cli_init (rtems_capture_timestamp timestamp);

#ifdef __cplusplus
}
#endif

#endif
