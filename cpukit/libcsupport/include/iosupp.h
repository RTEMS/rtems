/*  iosupp.h
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

#ifndef __IOSUPP_h
#define __IOSUPP_h

#ifdef __cplusplus
extern "C" {
#endif

/* character constants */

#define BS              0x08            /* backspace */
#define LF              0x0a            /* line feed */
#define CR              0x0d            /* carriage return */
#define XON             0x11            /* control-Q */
#define XOFF            0x13            /* control-S */

/* structures */

#ifdef IOSUPP_INIT
#define IOSUPP_EXTERN
#else
#undef IOSUPP_EXTERN
#define IOSUPP_EXTERN  extern
#endif

/* functions */

#ifdef __cplusplus
}
#endif

#endif
