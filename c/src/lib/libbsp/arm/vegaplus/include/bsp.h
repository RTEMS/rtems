/*-------------------------------------------------------------------------+
| bsp.h - ARM BSP 
+--------------------------------------------------------------------------+
| This include file contains definitions related to the ARM BSP.
+--------------------------------------------------------------------------+
|
| Copyright (c) Canon Research France SA.]
| Emmanuel Raguet, mailto:raguet@crf.canon.fr
|
|  The license and distribution terms for this file may be
|  found in found in the file LICENSE in this distribution or at
|  http://www.rtems.com/license/LICENSE.
| 
|  $Id$
+--------------------------------------------------------------------------*/


#ifndef __BSP_H_
#define __BSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <iosupp.h>
#include <console.h>
#include <clockdrv.h>
  
/*
 *  Define the interrupt mechanism for Time Test 27
 *
 *  NOTE: Following are not defined and are board independent
 *
 */

#define MUST_WAIT_FOR_INTERRUPT 0

#define Install_tm27_vector( handler ) 

#define Cause_tm27_intr()  

#define Clear_tm27_intr()  

#define Lower_tm27_intr()

#ifdef __cplusplus
}
#endif

#endif /* __BSP_H_ */
/* end of include file */

