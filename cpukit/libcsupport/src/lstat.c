/*
 *  lstat() - BSD 4.3 and SVR4 - Get File Status
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#define _STAT_NAME         lstat
#define _STAT_R_NAME       _lstat_r
#define _STAT_FOLLOW_LINKS FALSE

#include "stat.c"
