/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __ITRON_h_
#define __ITRON_h_

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Include the basic requirements
 */

#include <rtems/system.h>        /* basic RTEMS types defined by */
                                 /*   each port */
#include <itronsys/types.h>      /* define all the TRON types */
                                 /*   in terms of the RTEMS base types */
#include <itronsys/status.h>     /* define all ITRON status codes */

/*
 *  Now include the per manager include files
 */

#include <itronsys/eventflags.h>
#include <itronsys/fmempool.h>
#include <itronsys/intr.h>
#include <itronsys/mbox.h>
#include <itronsys/msgbuffer.h>
#include <itronsys/sysmgmt.h>  /* network depends on this file */
#include <itronsys/network.h>
#include <itronsys/port.h>
#include <itronsys/semaphore.h>
#include <itronsys/task.h>
#include <itronsys/time.h>
#include <itronsys/vmempool.h>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */

