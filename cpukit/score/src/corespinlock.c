/*
 *  SuperCore Spinlock Handler
 *
 *  DESCRIPTION:
 *
 *  This package is part of the implementation of the SuperCore Spinlock Handler.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/corespinlock.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>

/*
 *  _CORE_spinlock_Initialize
 *
 *  This function initialize a spinlock and sets the initial value based
 *  on the given count.
 *
 *  Input parameters:
 *    the_spinlock            - the spinlock control block to initialize
 *    the_spinlock_attributes - the attributes specified at create time
 *
 *  Output parameters:  NONE
 */

void _CORE_spinlock_Initialize(
  CORE_spinlock_Control       *the_spinlock,
  CORE_spinlock_Attributes    *the_spinlock_attributes
)
{

  the_spinlock->Attributes                = *the_spinlock_attributes;

  the_spinlock->lock   = 0;
  the_spinlock->users  = 0;
  the_spinlock->holder = 0;
}
