/*
 *  dosfs.h
 *
 *  Application interface to MSDOS filesystem.
 *
 *  Copyright (C) 2001 OKTET Ltd., St.-Petersburg, Russia
 *  Author: Eugeny S. Mints <Eugeny.Mints@oktet.ru>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  @(#) $Id$
 */
#ifndef __DOSFS_DOSFS_H__
#define __DOSFS_DOSFS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems.h>
#include <rtems/libio.h>

extern rtems_filesystem_operations_table  msdos_ops;

#ifdef __cplusplus
}
#endif

#endif /* __DOSFS_DOSFS_H__ */
