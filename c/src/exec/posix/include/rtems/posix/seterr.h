/*
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
 
#ifndef __POSIX_SET_ERRNO_h
#define __POSIX_SET_ERRNO_h
 
#define set_errno_and_return_minus_one( _error ) \
  { errno = (_error); return -1; }

#endif
/* end of include file */
