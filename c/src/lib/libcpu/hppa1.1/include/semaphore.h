/*
 *  Description:
 *      HPPA fast spinlock semaphores based on LDCWX instruction.
 *      These semaphores are not known to RTEMS.
 *
 *  TODO:
 *
 *  COPYRIGHT (c) 1994,95 by Division Incorporated
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _INCLUDE_HPPA1_1_SEMAPHORE_H
#define _INCLUDE_HPPA1_1_SEMAPHORE_H

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
