/*
 *  COPYRIGHT (c) 2012, 2018 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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

#include <rtems/rtl/rtl-fwd.h>
#include <rtems/rtl/rtl-obj-fwd.h>
#include <rtems/rtl/rtl-sym.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * The RAP format check handler.
 *
 * @param obj The object being checked.
 * @param fd The file descriptor.
 */
bool rtems_rtl_rap_file_check (rtems_rtl_obj* obj, int fd);

/**
 * The RAP format load handler.
 *
 * @param obj The object to load.
 * @param fd The file descriptor.
 */
bool rtems_rtl_rap_file_load (rtems_rtl_obj* obj, int fd);

/**
 * The RAP format unload handler.
 *
 * @param obj The object to unload.
 */
bool rtems_rtl_rap_file_unload (rtems_rtl_obj* obj);

/**
 * The RAP format signature handler.
 *
 * @return rtems_rtl_loader_format* The format's signature.
 */
rtems_rtl_loader_format* rtems_rtl_rap_file_sig (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
