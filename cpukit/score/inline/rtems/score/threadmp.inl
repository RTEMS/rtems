/*  inline/threadmp.inl
 *
 *  This include file contains the bodies of all inlined routines
 *  for the multiprocessing part of thread package.
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

#ifndef __INLINE_MP_THREAD_inl
#define __INLINE_MP_THREAD_inl

/*PAGE
 *
 *  _Thread_MP_Is_receive
 *
 *  DESCRIPTION:
 *
 *  This function returns true if the thread in question is the
 *  multiprocessing receive thread.
 */

RTEMS_INLINE_ROUTINE boolean _Thread_MP_Is_receive (
  Thread_Control *the_thread
)
{
  return the_thread == _Thread_MP_Receive;
}

/*PAGE
 *
 *  _Thread_MP_Free_proxy
 *
 *  DESCRIPTION:
 *
 *  This routine frees a proxy control block to the
 *  inactive chain of free proxy control blocks.
 */

RTEMS_INLINE_ROUTINE void _Thread_MP_Free_proxy (
  Thread_Control *the_thread
)
{
  Thread_Proxy_control *the_proxy;

  the_proxy = (Thread_Proxy_control *) the_thread;

  _Chain_Extract( &the_proxy->Active );

  _Chain_Append( &_Thread_MP_Inactive_proxies, &the_thread->Object.Node );
}

#endif
/* end of include file */
