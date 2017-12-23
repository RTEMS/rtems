/**
 * @file
 * 
 * @brief Internal Information about POSIX Signals
 *
 * This include file defines internal information about POSIX signals.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_PSIGNAL_H
#define _RTEMS_POSIX_PSIGNAL_H

#include <sys/signal.h>

#include <rtems/score/chain.h>

/*
 *  POSIX internal siginfo structure
 */

typedef struct {
  Chain_Node  Node;
  siginfo_t   Info;
}  POSIX_signals_Siginfo_node;

#endif
/* end of file */
