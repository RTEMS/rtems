/*  iosupp.h
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
