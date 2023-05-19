/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief This header file contains interfaces used by the implementation of
 *   the In-Memory File System.
 */

/*
 * Copyright (C) 2013, 2018 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _RTEMS_IMFSIMPL_H
#define _RTEMS_IMFSIMPL_H

#include <rtems/imfs.h>
#include <rtems/score/timecounter.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup IMFS
 *
 * @{
 */

static inline time_t _IMFS_get_time( void )
{
  struct bintime now;

  /* Use most efficient way to get the time in seconds (CLOCK_REALTIME) */
  _Timecounter_Getbintime( &now );

  return now.sec;
}

static inline void IMFS_update_atime( IMFS_jnode_t *jnode )
{
  jnode->stat_atime = _IMFS_get_time();
}

static inline void IMFS_update_mtime( IMFS_jnode_t *jnode )
{
  jnode->stat_mtime = _IMFS_get_time();
}

static inline void IMFS_update_ctime( IMFS_jnode_t *jnode )
{
  jnode->stat_ctime = _IMFS_get_time();
}

static inline void IMFS_mtime_ctime_update( IMFS_jnode_t *jnode )
{
  time_t now;

  now = _IMFS_get_time();

  jnode->stat_mtime = now;
  jnode->stat_ctime = now;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_IMFSIMPL_H */
