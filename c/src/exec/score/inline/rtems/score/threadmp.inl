/*  inline/threadmp.inl
 *
 *  This include file contains the bodies of all inlined routines
 *  for the multiprocessing part of thread package.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
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

STATIC INLINE boolean _Thread_MP_Is_receive (
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

STATIC INLINE void _Thread_MP_Free_proxy (
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
