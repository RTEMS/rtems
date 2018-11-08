/**
 * @file
 *
 * @ingroup ClassicEventImpl
 *
 * @brief Classic Event Manager Data Structures
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_EVENTDATA_H
#define _RTEMS_RTEMS_EVENTDATA_H

#include <rtems/rtems/event.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ClassicEventImpl
 *
 * @{
 */

typedef struct {
  rtems_event_set pending_events;
} Event_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
