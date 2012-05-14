/**
 * @file rtems/cbs.h
 *
 *  This include file contains all the constants and structures associated
 *  with the CBS library in RTEMS.
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef CONFIGURE_SCHEDULER_CBS
  #error "cbs.h available only with CONFIGURE_SCHEDULER_CBS"
#endif

#ifndef _RTEMS_CBS_H
#define _RTEMS_CBS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <rtems/score/schedulercbs.h>

/* Return codes. */
#define RTEMS_CBS_OK                       SCHEDULER_CBS_OK
#define RTEMS_CBS_ERROR_GENERIC            SCHEDULER_CBS_ERROR_GENERIC
#define RTEMS_CBS_ERROR_NO_MEMORY          SCHEDULER_CBS_ERROR_NO_MEMORY
#define RTEMS_CBS_ERROR_INVALID_PARAMETER  SCHEDULER_CBS_ERROR_INVALID_PARAM
#define RTEMS_CBS_ERROR_UNAUTHORIZED       SCHEDULER_CBS_ERROR_UNAUTHORIZED
#define RTEMS_CBS_ERROR_UNIMPLEMENTED      SCHEDULER_CBS_ERROR_UNIMPLEMENTED
#define RTEMS_CBS_ERROR_MISSING_COMPONENT  SCHEDULER_CBS_ERROR_MISSING_COMPONENT
#define RTEMS_CBS_ERROR_INCONSISTENT_STATE SCHEDULER_CBS_ERROR_INCONSISTENT_STATE
#define RTEMS_CBS_ERROR_SYSTEM_OVERLOAD    SCHEDULER_CBS_ERROR_SYSTEM_OVERLOAD
#define RTEMS_CBS_ERROR_INTERNAL_ERROR     SCHEDULER_CBS_ERROR_INTERNAL_ERROR
#define RTEMS_CBS_ERROR_NOT_FOUND          SCHEDULER_CBS_ERROR_NOT_FOUND
#define RTEMS_CBS_ERROR_FULL               SCHEDULER_CBS_ERROR_FULL
#define RTEMS_CBS_ERROR_EMPTY              SCHEDULER_CBS_ERROR_EMPTY
#define RTEMS_CBS_ERROR_NOSERVER           SCHEDULER_CBS_ERROR_NOSERVER

/** Callback function invoked when a budget overrun of a task occurs. */
typedef Scheduler_CBS_Budget_overrun rtems_cbs_budget_overrun;

/** Server id. */
typedef Scheduler_CBS_Server_id rtems_cbs_server_id;

/** Server parameters. */
typedef Scheduler_CBS_Parameters rtems_cbs_parameters;

#include <rtems/cbs.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
