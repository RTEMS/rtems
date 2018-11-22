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

/**
 * @brief The Classic Extensions objects information.
 */
extern Objects_Information _Extension_Information;

/**
 * @brief Macro to define the objects information for the Classic Extensions
 * objects.
 *
 * This macro should only be used by <rtems/confdefs.h>.
 *
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 * may be set).
 */
#define EXTENSION_INFORMATION_DEFINE( max ) \
  OBJECTS_INFORMATION_DEFINE( \
    _Extension, \
    OBJECTS_CLASSIC_API, \
    OBJECTS_RTEMS_EXTENSIONS, \
    Extension_Control, \
    max, \
    OBJECTS_NO_STRING_NAME, \
    NULL \
  )

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
