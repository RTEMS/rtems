/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  COPYRIGHT (c) 2013, 2018 Chris Johns <chrisj@rtems.org>
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
 * @ingroup RTEMSAPI
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
typedef bool (*rtems_rap_iterator) (void* handle);

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
bool rtems_rap_iterate (rtems_rap_iterator iterator);

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
