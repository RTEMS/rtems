/*
 *  COPYRIGHT (c) 2012-2013 Chris Johns <chrisj@rtems.org>
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
 * @brief RTEMS Run-Time Linker Object Support.
 */

#if !defined (_RTEMS_RTL_FIND_FILE_H_)
#define _RTEMS_RTL_FIND_FILE_H_

#include <rtems.h>
#include <rtems/chain.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Find a file on disk given a name and a path.
 *
 * @param name The file name to find. Can be relative or absolute.
 * @param paths The paths to search.
 * @param file_name Place the full path in this location if found.
 * @param size The size of the file if found as returned by the 'stat' call.
 * @retval true The file was found.
 * @retval false The file was not found.
 */
bool rtems_rtl_find_file (const char*  name,
                          const char*  paths,
                          const char** file_name,
                          size_t*      size);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
