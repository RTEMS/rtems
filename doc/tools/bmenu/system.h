/*
 *  COPYRIGHT (c) 1988-2002.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights reserved.
 *
 *  $Id$
 */

#ifndef __SYSTEM_h
#define __SYSTEM_h

typedef unsigned int   unsigned32;
typedef unsigned short unsigned16;
typedef unsigned char  unsigned8;

#define STATIC static
#define INLINE inline

#ifndef NULL
#define NULL 0
#endif

typedef unsigned int   boolean;

#if !defined( TRUE ) || (TRUE != 1)
#undef TRUE
#define TRUE     (1)
#endif

#if !defined( FALSE ) || (FALSE != 0)
#undef FALSE
#define FALSE 0
#endif

#endif
