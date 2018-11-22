/**
 * @file
 *
 * @ingroup ClassicDPMEMImpl
 *
 * @brief Classic Dual Ported Memory Manager Data Structures
 */

/* COPYRIGHT (c) 1989-2008.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_RTEMS_DPMEMDATA_H
#define _RTEMS_RTEMS_DPMEMDATA_H

#include <rtems/rtems/dpmem.h>
#include <rtems/score/objectdata.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ClassicDPMEMImpl
 *
 * @{
 */

/**
 *  The following structure defines the port control block.  Each port
 *  has a control block associated with it.  This control block contains
 *  all information required to support the port related operations.
 */
typedef struct {
  /** This field is the object management portion of a Port instance. */
  Objects_Control  Object;
  /** This field is the base internal address of the port. */
  void            *internal_base;
  /** This field is the base external address of the port. */
  void            *external_base;
  /** This field is the length of dual-ported area of the port. */
  uint32_t         length;
}   Dual_ported_memory_Control;

/**
 * @brief The Classic Dual Ported Memory objects information.
 */
extern Objects_Information _Dual_ported_memory_Information;

/**
 * @brief Macro to define the objects information for the Classic Dual Ported
 * Memory objects.
 *
 * This macro should only be used by <rtems/confdefs.h>.
 *
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 * may be set).
 */
#define DUAL_PORTED_MEMORY_INFORMATION_DEFINE( max ) \
  OBJECTS_INFORMATION_DEFINE( \
    _Dual_ported_memory, \
    OBJECTS_CLASSIC_API, \
    OBJECTS_RTEMS_PORTS, \
    Dual_ported_memory_Control, \
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
