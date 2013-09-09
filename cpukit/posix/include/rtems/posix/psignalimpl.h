/**
 * @file
 * 
 * @brief POSIX Signals Support
 *
 * This include file defines internal information about POSIX signals.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_PSIGNALIMPL_H
#define _RTEMS_POSIX_PSIGNALIMPL_H

/**
 * @defgroup POSIX_SIGNALS POSIX Signals Support
 *
 * @ingroup POSIXAPI
 *
 * @brief Internal Information about POSIX Signals
 * 
 */
/**@{**/

#include <rtems/posix/psignal.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/sigset.h>
#include <rtems/score/apiext.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/threadq.h>

#define _States_Is_interruptible_signal( _states ) \
  ( ((_states) & \
    (STATES_WAITING_FOR_SIGNAL|STATES_INTERRUPTIBLE_BY_SIGNAL)) == \
      (STATES_WAITING_FOR_SIGNAL|STATES_INTERRUPTIBLE_BY_SIGNAL))

#define SIGACTION_TERMINATE \
  { 0, SIGNAL_ALL_MASK, {_POSIX_signals_Abnormal_termination_handler} }
#define SIGACTION_IGNORE \
  { 0, SIGNAL_ALL_MASK, {SIG_IGN} }
#define SIGACTION_STOP \
  { 0, SIGNAL_ALL_MASK, {_POSIX_signals_Stop_handler} }
#define SIGACTION_CONTINUE \
  { 0, SIGNAL_ALL_MASK, {_POSIX_signals_Continue_handler} }

#define SIG_ARRAY_MAX  (SIGRTMAX + 1)

/*
 *  Variables
 */

extern ISR_lock_Control _POSIX_signals_Lock;

extern sigset_t  _POSIX_signals_Pending;

extern const struct sigaction _POSIX_signals_Default_vectors[ SIG_ARRAY_MAX ];

extern struct sigaction _POSIX_signals_Vectors[ SIG_ARRAY_MAX ];

extern Thread_queue_Control _POSIX_signals_Wait_queue;

extern Chain_Control _POSIX_signals_Inactive_siginfo;

extern Chain_Control _POSIX_signals_Siginfo[ SIG_ARRAY_MAX ];

extern API_extensions_Post_switch_control _POSIX_signals_Post_switch;

/*
 *  Internal routines
 */

/**
 * @brief POSIX signals manager initialization.
 */
void _POSIX_signals_Manager_Initialization(void);

#define _POSIX_signals_Acquire( level ) \
  _ISR_lock_ISR_disable_and_acquire( &_POSIX_signals_Lock, level )

#define _POSIX_signals_Release( level ) \
  _ISR_lock_Release_and_ISR_enable( &_POSIX_signals_Lock, level )

static inline void _POSIX_signals_Add_post_switch_extension(void)
{
  _API_extensions_Add_post_switch( &_POSIX_signals_Post_switch );
}

/**
 * @brief Unlock POSIX signals thread.
 *
 * XXX this routine could probably be cleaned up
 */
bool _POSIX_signals_Unblock_thread(
  Thread_Control  *the_thread,
  int              signo,
  siginfo_t       *info
);

/**
 *  @brief Check POSIX signal.
 */
bool _POSIX_signals_Check_signal(
  POSIX_API_Control  *api,
  int                 signo,
  bool                is_global
);

/**
 * @brief Clear POSIX signals.
 */
bool _POSIX_signals_Clear_signals(
  POSIX_API_Control  *api,
  int                 signo,
  siginfo_t          *info,
  bool                is_global,
  bool                check_blocked,
  bool                do_signals_acquire_release
);

int killinfo(
  pid_t               pid,
  int                 sig,
  const union sigval *value
);

/**
 *  @brief Set POSIX process signals.
 */
void _POSIX_signals_Set_process_signals(
  sigset_t   mask
);

void _POSIX_signals_Clear_process_signals(
  int        signo
);

/*
 *  Default signal handlers
 */

#define _POSIX_signals_Stop_handler NULL
#define _POSIX_signals_Continue_handler NULL

void _POSIX_signals_Abnormal_termination_handler( int signo );

/** @} */

#endif
/* end of file */
