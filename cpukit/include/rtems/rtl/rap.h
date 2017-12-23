/*
 *  COPYRIGHT (c) 2013 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems_rap
 *
 * @brief RTEMS Application Loader
 *
 * This is the RTEMS Application loader for files in the RAP format.
 */

#if !defined (_RAP_H_)
#define _RAP_H_

#include <rtems.h>
#include <rtems/chain.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup rtems_rap RTEMS Application Loader
 *
 * The module implements an application loader for files in the RAP format. The
 * RAP format is:
 *
 *   <header>
 *   <compressed container>
 *
 * The compressed container is a stream of ELF relocatable object files.
 *
 *  TBD.
 */

/**
 * The module iterator handle.
 */
typedef bool (*rtems_rap_iterator_t) (void* handle);

/**
 * Load an application.
 *
 * @param name The name of the application file.
 * @return bool True if the module loads else an error.
 */
bool rtems_rap_load (const char* name, int mode, int argc, const char* argv[]);

/**
 * Unload an application.
 *
 * @param obj The application descriptor.
 * @retval true The application file has been unloaded.
 * @retval false The application could not be unloaded.
 */
bool rtems_rap_unload (const char* name);

/**
 * Find the application handle given a file name.
 *
 * @param name The name of the application file. It can be absolute or
 *             relative. Relative names can the basename with an extension.
 * @retval NULL No application file with that name found.
 * @return void* The application descriptor.
 */
void* rtems_rap_find (const char* name);

/**
 * Run an iterator over the modules calling the iterator function.
 *
 * @param iterator The iterator function.
 * @retval true The iterator function returned did not return false.
 * @retval false The iterator function returned false..
 */
bool rtems_rap_iterate (rtems_rap_iterator_t iterator);

/**
 * Return the name of the module given a handle.
 *
 * @param handle The module handle.
 * @return const char* The name of the module if the handle is valid else it
 *                     is NULL.
 */
const char* rtems_rap_name (void* handle);

/**
 * Return the DL handle used to load the module given the RAP handle.
 *
 * @param handle The module handle.
 * @return void* The DL handle returned by the dlopen call.
 */
void* rtems_rap_dl_handle (void* handle);

/**
 * Get the last error message clearing it. This call is not thread safe is
 * multiple threads are loading object files at the same time. This call
 * follows the model provided by the dlopen family of calls.
 *
 * @param message Pointer to a buffer to copy the message into.
 * @param max_message The maximum message that can be copied.
 * @return int The last error number.
 */
int rtems_rap_get_error (char* message, size_t max_message);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
