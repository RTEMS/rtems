/*
 *  File:       $RCSfile$
 *  Project:    PixelFlow
 *  Created:    94/11/29
 *  RespEngr:   tony bennett
 *  Revision:   $Revision$
 *  Last Mod:   $Date$
 *
 *  Description:
 *      definitions specific to the runway bus
 *
 *  TODO:
 *      Add lots more.
 *
 *  $Id$
 */

#ifndef _INCLUDE_RUNWAY_H
#define _INCLUDE_RUNWAY_H

#ifdef __cplusplus
extern "C" {
#endif

#define HPPA_RUNWAY_PROC_HPA_BASE  ((void *) 0xFFFA0000)

/* given a processor number, where is its HPA? */
#define HPPA_RUNWAY_HPA(cpu)   \
  ((rtems_unsigned32) (HPPA_RUNWAY_PROC_HPA_BASE + ((cpu) * 0x2000)))

#define HPPA_RUNWAY_REG_IO_EIR_OFFSET   0x000

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_RUNWAY_H */
