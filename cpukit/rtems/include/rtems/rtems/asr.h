/**
 * @file rtems/rtems/asr.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Asynchronous Signal Handler.  This Handler provides the low-level
 *  support required by the Signal Manager.
 */

/*  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_ASR_H
#define _RTEMS_RTEMS_ASR_H

#include <rtems/rtems/modes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @defgroup ClassicASR ASR Support
 *
 *  @ingroup ClassicRTEMS
 *
 *  This encapsulates functionality which XXX
 */
/**@{*/

/**
 *  The following type defines the control block used to manage
 *  each signal set.
 */
typedef uint32_t   rtems_signal_set;

/**
 *  Return type for ASR Handler
 */
typedef void rtems_asr;

/**
 *  The following type corresponds to the applications asynchronous
 *  signal processing routine.
 */
typedef rtems_asr ( *rtems_asr_entry )(
                 rtems_signal_set
             );

/**
 *  The following defines the control structure used to manage
 *  signals.  Each thread has a copy of this record.
 */
typedef struct {
  /** This field indicates if are ASRs enabled currently. */
  bool              is_enabled;
  /** This field indicates if address of the signal handler function. */
  rtems_asr_entry   handler;
  /** This field indicates if the task mode the signal will run with. */
  Modes_Control     mode_set;
  /** This field indicates the signal set that is posted. */
  rtems_signal_set  signals_posted;
  /** This field indicates the signal set that is pending. */
  rtems_signal_set  signals_pending;
  /** This field indicates if nest level of signals being processed */
  uint32_t          nest_level;
}   ASR_Information;

/*
 *  The following constants define the individual signals which may
 *  be used to compose a signal set.
 */

/** This defines the bit in the signal set associated with signal 0. */
#define RTEMS_SIGNAL_0    0x00000001
/** This defines the bit in the signal set associated with signal 1. */
#define RTEMS_SIGNAL_1    0x00000002
/** This defines the bit in the signal set associated with signal 2. */
#define RTEMS_SIGNAL_2    0x00000004
/** This defines the bit in the signal set associated with signal 3. */
#define RTEMS_SIGNAL_3    0x00000008
/** This defines the bit in the signal set associated with signal 4. */
#define RTEMS_SIGNAL_4    0x00000010
/** This defines the bit in the signal set associated with signal 5. */
#define RTEMS_SIGNAL_5    0x00000020
/** This defines the bit in the signal set associated with signal 6. */
#define RTEMS_SIGNAL_6    0x00000040
/** This defines the bit in the signal set associated with signal 7. */
#define RTEMS_SIGNAL_7    0x00000080
/** This defines the bit in the signal set associated with signal 8. */
#define RTEMS_SIGNAL_8    0x00000100
/** This defines the bit in the signal set associated with signal 9. */
#define RTEMS_SIGNAL_9    0x00000200
/** This defines the bit in the signal set associated with signal 10. */
#define RTEMS_SIGNAL_10   0x00000400
/** This defines the bit in the signal set associated with signal 11. */
#define RTEMS_SIGNAL_11   0x00000800
/** This defines the bit in the signal set associated with signal 12. */
#define RTEMS_SIGNAL_12   0x00001000
/** This defines the bit in the signal set associated with signal 13. */
#define RTEMS_SIGNAL_13   0x00002000
/** This defines the bit in the signal set associated with signal 14. */
#define RTEMS_SIGNAL_14   0x00004000
/** This defines the bit in the signal set associated with signal 15. */
#define RTEMS_SIGNAL_15   0x00008000
/** This defines the bit in the signal set associated with signal 16. */
#define RTEMS_SIGNAL_16   0x00010000
/** This defines the bit in the signal set associated with signal 17. */
#define RTEMS_SIGNAL_17   0x00020000
/** This defines the bit in the signal set associated with signal 18. */
#define RTEMS_SIGNAL_18   0x00040000
/** This defines the bit in the signal set associated with signal 19. */
#define RTEMS_SIGNAL_19   0x00080000
/** This defines the bit in the signal set associated with signal 20. */
#define RTEMS_SIGNAL_20   0x00100000
/** This defines the bit in the signal set associated with signal 21. */
#define RTEMS_SIGNAL_21   0x00200000
/** This defines the bit in the signal set associated with signal 22. */
#define RTEMS_SIGNAL_22   0x00400000
/** This defines the bit in the signal set associated with signal 23. */
#define RTEMS_SIGNAL_23   0x00800000
/** This defines the bit in the signal set associated with signal 24. */
#define RTEMS_SIGNAL_24   0x01000000
/** This defines the bit in the signal set associated with signal 25. */
#define RTEMS_SIGNAL_25   0x02000000
/** This defines the bit in the signal set associated with signal 26. */
#define RTEMS_SIGNAL_26   0x04000000
/** This defines the bit in the signal set associated with signal 27. */
#define RTEMS_SIGNAL_27   0x08000000
/** This defines the bit in the signal set associated with signal 28. */
#define RTEMS_SIGNAL_28   0x10000000
/** This defines the bit in the signal set associated with signal 29. */
#define RTEMS_SIGNAL_29   0x20000000
/** This defines the bit in the signal set associated with signal 30. */
#define RTEMS_SIGNAL_30   0x40000000
/** This defines the bit in the signal set associated with signal 31. */
#define RTEMS_SIGNAL_31   0x80000000

#ifndef __RTEMS_APPLICATION__
#include <rtems/rtems/asr.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
