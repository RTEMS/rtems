/**
 * @file qreslib.h
 *
 *  This include file contains all the constants and structures associated
 *  with the QoS RES library in RTEMS.
 *
 * @note The library is available only together with CBS scheduler.
 */

/*
 *  Copyright (C) 2011 Petr Benes.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef CONFIGURE_SCHEDULER_CBS
  #error "qreslib.h available only with CONFIGURE_SCHEDULER_CBS"
#endif

#ifndef _QRESLIB_H
#define _QRESLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <rtems/score/schedulercbs.h>

/** Return values. */
typedef int qos_rv;

/* Return codes. */
#define QOS_OK                   SCHEDULER_CBS_OK
#define QOS_E_GENERIC            SCHEDULER_CBS_ERROR_GENERIC
#define QOS_E_NO_MEMORY          SCHEDULER_CBS_ERROR_NO_MEMORY
#define QOS_E_INVALID_PARAM      SCHEDULER_CBS_ERROR_INVALID_PARAMETER
#define QOS_E_UNAUTHORIZED       SCHEDULER_CBS_ERROR_UNAUTHORIZED
#define QOS_E_UNIMPLEMENTED      SCHEDULER_CBS_ERROR_UNIMPLEMENTED
#define QOS_E_MISSING_COMPONENT  SCHEDULER_CBS_ERROR_MISSING_COMPONENT
#define QOS_E_INCONSISTENT_STATE SCHEDULER_CBS_ERROR_INCONSISTENT_STATE
#define QOS_E_SYSTEM_OVERLOAD    SCHEDULER_CBS_ERROR_SYSTEM_OVERLOAD
#define QOS_E_INTERNAL_ERROR     SCHEDULER_CBS_ERROR_INTERNAL_ERROR
#define QOS_E_NOT_FOUND          SCHEDULER_CBS_ERROR_NOT_FOUND
#define QOS_E_FULL               SCHEDULER_CBS_ERROR_FULL
#define QOS_E_EMPTY              SCHEDULER_CBS_ERROR_EMPTY
#define QOS_E_NOSERVER           SCHEDULER_CBS_ERROR_NOSERVER

/** Server id. */
typedef Scheduler_CBS_Server_id qres_sid_t;

/** Task id. */
typedef rtems_id tid_t;

/** Time value. */
typedef time_t qres_time_t;

/** Absolute time value */
typedef time_t qres_atime_t;

/** Server parameters. */
typedef struct {
  /** Relative deadline of the server. */
  uint32_t P;
  /** Budget (computation time) of the server. */
  uint32_t Q;
} qres_params_t;

#include <qreslib.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
