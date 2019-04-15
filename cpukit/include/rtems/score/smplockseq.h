/**
 * @file
 *
 * @ingroup RTEMSScoreSMPLock
 *
 * @brief SMP Lock API
 */

/*
 * Copyright (c) 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SMPLOCKSEQ_H
#define _RTEMS_SCORE_SMPLOCKSEQ_H

#include <rtems/score/cpuopts.h>

#if defined(RTEMS_SMP)

#include <rtems/score/assert.h>
#include <rtems/score/atomic.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup RTEMSScoreSMPLock
 *
 * @{
 */

/**
 * @brief SMP sequence lock control.
 *
 * The sequence lock offers a consistent data set for readers in the presence
 * of at most one concurrent writer.  Due to the read-modify-write operation in
 * _SMP_sequence_lock_Read_retry() the data corresponding to the last written
 * sequence number is observed.  To allow multiple writers an additional SMP
 * lock is necessary to serialize writes.
 *
 * See also Hans-J. Boehm, HP Laboratories,
 * "Can Seqlocks Get Along With Programming Language Memory Models?",
 * http://www.hpl.hp.com/techreports/2012/HPL-2012-68.pdf
 */
typedef struct {
  /**
   * @brief The sequence number.
   *
   * An odd value indicates that a write is in progress.
   */
  Atomic_Uint sequence;
} SMP_sequence_lock_Control;

/**
 * @brief SMP sequence lock control initializer for static initialization.
 */
#define SMP_SEQUENCE_LOCK_INITIALIZER { ATOMIC_INITIALIZER_UINT( 0 ) }

/**
 * @brief Initializes an SMP sequence lock.
 *
 * Concurrent initialization leads to unpredictable results.
 *
 * @param[out] lock The SMP sequence lock control.
 */
static inline void _SMP_sequence_lock_Initialize( SMP_sequence_lock_Control *lock )
{
  _Atomic_Init_uint( &lock->sequence, 0 );
}

/**
 * @brief Destroys an SMP sequence lock.
 *
 * Concurrent destruction leads to unpredictable results.
 *
 * @param lock The SMP sequence lock control.
 */
static inline void _SMP_sequence_lock_Destroy( SMP_sequence_lock_Control *lock )
{
  (void) lock;
}

/**
 * @brief Begins an SMP sequence lock write operation.
 *
 * This function will not disable interrupts.  The caller must ensure that the
 * current thread of execution is not interrupted indefinite since this would
 * starve readers.
 *
 * @param[out] lock The SMP sequence lock control.
 *
 * @return The current sequence number.
 */
static inline unsigned int _SMP_sequence_lock_Write_begin(
  SMP_sequence_lock_Control *lock
)
{
  unsigned int seq;

  seq = _Atomic_Load_uint( &lock->sequence, ATOMIC_ORDER_RELAXED );
  _Assert( seq % 2 == 0 );

  _Atomic_Store_uint( &lock->sequence, seq + 1, ATOMIC_ORDER_RELAXED );

  /* There is no atomic store with acquire/release semantics */
  _Atomic_Fence( ATOMIC_ORDER_ACQ_REL );

  return seq;
}

/**
 * @brief Ends an SMP sequence lock write operation.
 *
 * @param[out] lock The SMP sequence lock control.
 * @param seq The sequence number returned by _SMP_sequence_lock_Write_begin().
 */
static inline void _SMP_sequence_lock_Write_end(
  SMP_sequence_lock_Control *lock,
  unsigned int               seq
)
{
  _Atomic_Store_uint( &lock->sequence, seq + 2, ATOMIC_ORDER_RELEASE );
}

/**
 * @brief Begins an SMP sequence lock read operation.
 *
 * This function will not disable interrupts.
 *
 * @param[out] lock The SMP sequence lock control.
 *
 * @return The current sequence number.
 */
static inline unsigned int _SMP_sequence_lock_Read_begin(
  const SMP_sequence_lock_Control *lock
)
{
  return _Atomic_Load_uint( &lock->sequence, ATOMIC_ORDER_ACQUIRE );
}

/**
 * @brief Ends an SMP sequence lock read operation and indicates if a retry is
 * necessary.
 *
 * @param[in, out] lock The SMP sequence lock control.
 * @param seq The sequence number returned by _SMP_sequence_lock_Read_begin().
 *
 * @retval true The read operation must be retried with a call to
 *   _SMP_sequence_lock_Read_begin().
 * @retval false The read operation need not be retried.
 */
static inline bool _SMP_sequence_lock_Read_retry(
  SMP_sequence_lock_Control *lock,
  unsigned int               seq
)
{
  unsigned int seq2;

  seq2 = _Atomic_Fetch_add_uint( &lock->sequence, 0, ATOMIC_ORDER_RELEASE );
  return seq != seq2 || seq % 2 != 0;
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_SMP */

#endif /* _RTEMS_SCORE_SMPLOCKSEQ_H */
