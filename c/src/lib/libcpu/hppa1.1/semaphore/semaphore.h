/*
 *  File:       $RCSfile$
 *  Project:    PixelFlow
 *  Created:    94/11/29
 *  RespEngr:   tony e bennett
 *  Revision:   $Revision$
 *  Last Mod:   $Date$
 *
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of Division Incorporated not be
 *      used in advertising or publicity pertaining to distribution
 *      of the software without specific, written prior permission.
 *      Division Incorporated makes no representations about the
 *      suitability of this software for any purpose.
 *
 *  Description:
 *      HPPA fast spinlock semaphores based on LDCWX instruction.
 *      These semaphores are not known to RTEMS.
 *
 *  TODO:
 *
 *  $Id$
 */

#ifndef _INCLUDE_SEMAPHORE_H
#define _INCLUDE_SEMAPHORE_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This structure has hardware requirements.
 * LDCWX opcode requires 16byte alignment for the lock
 * 'lock' must be first member of structure.
 */

#define SEM_ALIGN  16

typedef volatile struct {

    rtems_unsigned32 lock __attribute__ ((aligned (SEM_ALIGN)));

    rtems_unsigned32 flags;

    void *owner_tcb;            /* for debug/commentary only */

    rtems_unsigned32 user;      /* for use by user */

} hppa_semaphore_t;

/*
 * Values for flags
 */

#define HPPA_SEM_IN_USE            0x0001  /* semaphore owned by somebody */
#define HPPA_SEM_NO_SPIN           0x0002  /* don't spin if unavailable */
#define HPPA_SEM_INITIALLY_FREE    0x0004  /* init it to be free */

/*
 * Caller specifiable flags
 */

#define HPPA_SEM_CALLER_FLAGS (HPPA_SEM_NO_SPIN | HPPA_SEM_INITIALLY_FREE)

void hppa_semaphore_pool_initialize(void *pool_base, int pool_size);

rtems_unsigned32  hppa_semaphore_acquire(hppa_semaphore_t *sp, int flag);

void hppa_semaphore_release(hppa_semaphore_t *sp);

hppa_semaphore_t *hppa_semaphore_allocate(rtems_unsigned32 which, int flag);

#ifdef __cplusplus
}
#endif

#endif /* ! _INCLUDE_SEMAPHORE_H */
