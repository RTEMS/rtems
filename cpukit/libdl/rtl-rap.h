/*
 *  COPYRIGHT (c) 2012 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rtl
 *
 * @brief RTEMS Run-Time Linker ELF Headers
 */

#if !defined (_RTEMS_RTL_RAP_H_)
#define _RTEMS_RTL_RAP_H_

#include "rtl-fwd.h"
#include "rtl-obj-fwd.h"
#include "rtl-sym.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * The RAP format check handler.
 *
 * @param obj The object being checked.
 * @param fd The file descriptor.
 */
bool rtems_rtl_rap_file_check (rtems_rtl_obj_t* obj, int fd);

/**
 * The RAP format load handler.
 *
 * @param obj The object to load.
 * @param fd The file descriptor.
 */
bool rtems_rtl_rap_file_load (rtems_rtl_obj_t* obj, int fd);

/**
 * The RAP format signature handler.
 *
 * @return rtems_rtl_loader_format_t* The format's signature.
 */
rtems_rtl_loader_format_t* rtems_rtl_rap_file_sig (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
