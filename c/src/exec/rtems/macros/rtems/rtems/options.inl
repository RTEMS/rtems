/*  options.inl
 *
 *  This file contains the macro implementation of the inlined
 *  routines from the Options Handler.
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

#ifndef __OPTIONS_inl
#define __OPTIONS_inl

/*PAGE
 *
 *  _Options_Is_no_wait
 *
 */

#define _Options_Is_no_wait( _option_set ) \
   ( (_option_set) & RTEMS_NO_WAIT )

/*PAGE
 *
 *  _Options_Is_any
 *
 */

#define _Options_Is_any( _option_set ) \
   ( (_option_set) & RTEMS_EVENT_ANY )

#endif
/* end of include file */
