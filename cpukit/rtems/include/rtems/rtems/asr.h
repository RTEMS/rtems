/*  asr.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Asynchronous Signal Handler.  This Handler provides the low-level
 *  support required by the Signal Manager.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_ASR_h
#define __RTEMS_ASR_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/rtems/modes.h>

/*
 *
 *  The following type defines the control block used to manage
 *  each signal set.
 */

typedef unsigned32 rtems_signal_set;

/*
 *  Return type for ASR Handler
 */

typedef void rtems_asr;

/*
 *  The following type corresponds to the applications asynchronous
 *  signal processing routine.
 */

typedef rtems_asr ( *rtems_asr_entry )(
                 rtems_signal_set
             );

/*
 *
 *  The following defines the control structure used to manage
 *  signals.  Each thread has a copy of this record.
 */

typedef struct {
  boolean           is_enabled;       /* are ASRs enabled currently? */
  rtems_asr_entry   handler;          /* address of RTEMS_ASR */
  Modes_Control     mode_set;         /* RTEMS_ASR mode */
  rtems_signal_set  signals_posted;   /* signal set */
  rtems_signal_set  signals_pending;  /* pending signal set */
  unsigned32        nest_level;       /* nest level of RTEMS_ASR */
}   ASR_Information;

/*
 *  The following constants define the individual signals which may
 *  be used to compose a signal set.
 */

#define RTEMS_SIGNAL_0    0x00000001
#define RTEMS_SIGNAL_1    0x00000002
#define RTEMS_SIGNAL_2    0x00000004
#define RTEMS_SIGNAL_3    0x00000008
#define RTEMS_SIGNAL_4    0x00000010
#define RTEMS_SIGNAL_5    0x00000020
#define RTEMS_SIGNAL_6    0x00000040
#define RTEMS_SIGNAL_7    0x00000080
#define RTEMS_SIGNAL_8    0x00000100
#define RTEMS_SIGNAL_9    0x00000200
#define RTEMS_SIGNAL_10   0x00000400
#define RTEMS_SIGNAL_11   0x00000800
#define RTEMS_SIGNAL_12   0x00001000
#define RTEMS_SIGNAL_13   0x00002000
#define RTEMS_SIGNAL_14   0x00004000
#define RTEMS_SIGNAL_15   0x00008000
#define RTEMS_SIGNAL_16   0x00010000
#define RTEMS_SIGNAL_17   0x00020000
#define RTEMS_SIGNAL_18   0x00040000
#define RTEMS_SIGNAL_19   0x00080000
#define RTEMS_SIGNAL_20   0x00100000
#define RTEMS_SIGNAL_21   0x00200000
#define RTEMS_SIGNAL_22   0x00400000
#define RTEMS_SIGNAL_23   0x00800000
#define RTEMS_SIGNAL_24   0x01000000
#define RTEMS_SIGNAL_25   0x02000000
#define RTEMS_SIGNAL_26   0x04000000
#define RTEMS_SIGNAL_27   0x08000000
#define RTEMS_SIGNAL_28   0x10000000
#define RTEMS_SIGNAL_29   0x20000000
#define RTEMS_SIGNAL_30   0x40000000
#define RTEMS_SIGNAL_31   0x80000000

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/asr.inl>
#endif

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
