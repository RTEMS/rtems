/*
 *  COPYRIGHT (c) 1997.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __DUMP_BUFFER_h
#define __DUMP_BUFFER_h

#ifdef __cplusplus
extern "C" {
#endif

void Dump_Buffer(
  unsigned char *buffer,
  int            length
);

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
