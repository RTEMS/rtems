/*
 *  Instantatiate a private user environment for the calling thread.
 *
 *  Submitted by: fernando.ruiz@ctv.es (correo@fernando-ruiz.com)
 *
 *  COPYRIGHT (c) 1989-2000.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>

rtems_status_code rtems_libio_set_private_env(void) {
  rtems_status_code sc;

  sc = rtems_task_variable_add(RTEMS_SELF,(void*)&rtems_current_user_env,free);
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  rtems_current_user_env = malloc(sizeof(rtems_user_env_t));
  if (!rtems_current_user_env)
    return RTEMS_NO_MEMORY;

  *rtems_current_user_env = rtems_global_user_env;
  return RTEMS_SUCCESSFUL;
}
