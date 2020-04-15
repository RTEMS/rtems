/**
 *  @file
 *
 *  @brief Get File Status
 *  @ingroup libcsupport
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _STAT_NAME         lstat
#define _STAT_R_NAME       _lstat_r
#define _STAT_FOLLOW_LINKS RTEMS_FS_FOLLOW_HARD_LINK

#include "stat.c"

/**
 *  BSD 4.3 and SVR4 - Get File Status
 */
