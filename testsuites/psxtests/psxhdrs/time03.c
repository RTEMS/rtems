/*
 *  This test file is used to verify that the header files associated with
 *  the callout are correct.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <time.h>
 
void test( void )
{
  struct tm timestruct;
  time_t    time_encoded;

  timestruct.tm_sec = 0;
  timestruct.tm_min = 0;
  timestruct.tm_hour = 0;
  timestruct.tm_mday = 1;
  timestruct.tm_mon = 0;
  timestruct.tm_year = 0;
  timestruct.tm_wday = 0;
  timestruct.tm_yday = 0;
  timestruct.tm_isdst = 0;

  time_encoded = mktime( &timestruct );
}
