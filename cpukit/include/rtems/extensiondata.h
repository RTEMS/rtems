/**
 * @file
 *
 * @ingroup ClassicUserExtensionsImpl
 *
 * @brief Classic User Extensions Data Structures
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_EXTENSIONDATA_H
#define _RTEMS_EXTENSIONDATA_H

#include <rtems/extension.h>
#include <rtems/score/objectdata.h>
#include <rtems/score/userextdata.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ClassicUserExtensionsImpl
 *
 * @{
 */

typedef struct {
  Objects_Control          Object;
  User_extensions_Control  Extension;
}   Extension_Control;

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
