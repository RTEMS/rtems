/*  macros/threadmp.h
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

#ifndef __MACROS_MP_THREAD_h
#define __MACROS_MP_THREAD_h

/*PAGE
 *
 *  _Thread_MP_Is_receive
 *
 */

#define _Thread_MP_Is_receive( _the_thread ) \
  ( (_the_thread) == _Thread_MP_Receive)

/*PAGE
 *
 *  _Thread_MP_Free_proxy
 *
 */

#define _Thread_MP_Free_proxy( _the_thread ) \
{ \
  Thread_Proxy_control *_the_proxy; \
 \
  _the_proxy = (Thread_Proxy_control *) (_the_thread); \
 \
  _Chain_Extract( &_the_proxy->Active ); \
 \
  _Chain_Append( \
    &_Thread_MP_Inactive_proxies, \
    &(_the_thread)->Object.Node \
  ); \
}

#endif
/* end of include file */
