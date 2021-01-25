/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIConfig
 *
 * @brief This header file provides parts of the application configuration
 *   information API.
 */

/*
 * Copyright (C) 2009, 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

/* Generated from spec:/rtems/config/if/header */

#ifndef _RTEMS_CONFIG_H
#define _RTEMS_CONFIG_H

#include <stddef.h>
#include <stdint.h>
#include <rtems/rtems/config.h>
#include <rtems/score/cpu.h>
#include <rtems/score/isr.h>
#include <rtems/score/memory.h>
#include <rtems/score/object.h>
#include <rtems/score/smp.h>
#include <rtems/score/stack.h>
#include <rtems/score/threadidledata.h>
#include <rtems/score/userextdata.h>
#include <rtems/score/watchdogticks.h>
#include <rtems/score/wkspacedata.h>

#if defined(RTEMS_MULTIPROCESSING)
  #include <rtems/rtems/types.h>
  #include <rtems/score/mpci.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/config/if/group */

/**
 * @defgroup RTEMSAPIConfig Application Configuration Information
 *
 * @ingroup RTEMSAPI
 *
 * @brief The application configuration information group provides an API to
 *   get the configuration of an application.
 *
 * Some interfaces of this API are also used to define application
 * configuration option values, for example rtems_resource_unlimited().
 */

/* Generated from spec:/rtems/config/if/get-copyright-notice */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the RTEMS copyright notice.
 *
 * @return Returns the pointer to the RTEMS copyright notice.
 */
const char *rtems_get_copyright_notice( void );

/* Generated from spec:/rtems/config/if/get-do-zero-of-workspace */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Indicates if the RTEMS Workspace is configured to be zeroed during
 *   system initialization for this application.
 *
 * @return Returns true, if the RTEMS Workspace is configured to be zeroed
 *   during system initialization for this application, otherwise false.
 *
 * @par Notes
 * See #CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY.
 */
#define rtems_configuration_get_do_zero_of_workspace() _Memory_Zero_before_use

/* Generated from spec:/rtems/config/if/get-idle-task */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the IDLE task entry of this application.
 *
 * @return Returns the IDLE task entry of this application.
 *
 * @par Notes
 * See #CONFIGURE_IDLE_TASK_BODY.
 */
#define rtems_configuration_get_idle_task() _Thread_Idle_body

/* Generated from spec:/rtems/config/if/get-idle-task-stack-size */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the IDLE task stack size in bytes of this application.
 *
 * @return Returns the IDLE task stack size in bytes of this application.
 *
 * @par Notes
 * See #CONFIGURE_IDLE_TASK_STACK_SIZE.
 */
#define rtems_configuration_get_idle_task_stack_size() _Thread_Idle_stack_size

/* Generated from spec:/rtems/config/if/get-interrupt-stack-size */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the interrupt stack size in bytes of this application.
 *
 * @return Returns the interrupt stack size in bytes of this application.
 *
 * @par Notes
 * See #CONFIGURE_INTERRUPT_STACK_SIZE.
 */
#define rtems_configuration_get_interrupt_stack_size() \
  ((size_t) _ISR_Stack_size)

/* Generated from spec:/rtems/config/if/get-maximum-extensions */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the maximum number of Classic API User Extensions configured for
 *   this application.
 *
 * @return Returns the maximum number of Classic API User Extensions configured
 *   for this application.
 *
 * @par Notes
 * See #CONFIGURE_MAXIMUM_USER_EXTENSIONS.
 */
uint32_t rtems_configuration_get_maximum_extensions( void );

/* Generated from spec:/rtems/config/if/get-maximum-processors */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the maximum number of processors configured for this
 *   application.
 *
 * @return Returns the maximum number of processors configured for this
 *   application.
 *
 * @par Notes
 * @parblock
 * The actual number of processors available to the application is returned by
 * rtems_scheduler_get_processor_maximum() which less than or equal to the
 * configured maximum number of processors (#CONFIGURE_MAXIMUM_PROCESSORS).
 *
 * In uniprocessor configurations, this macro is a compile time constant which
 * evaluates to one.
 * @endparblock
 */
#define rtems_configuration_get_maximum_processors() \
  _SMP_Processor_configured_maximum

/* Generated from spec:/rtems/config/if/get-microseconds-per-tick */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the number of microseconds per clock tick configured for this
 *   application.
 *
 * @return Returns the number of microseconds per clock tick configured for
 *   this application.
 *
 * @par Notes
 * See #CONFIGURE_MICROSECONDS_PER_TICK.
 */
#define rtems_configuration_get_microseconds_per_tick() \
  _Watchdog_Microseconds_per_tick

/* Generated from spec:/rtems/config/if/get-milliseconds-per-tick */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the number of milliseconds per clock tick configured for this
 *   application.
 *
 * @return Returns the number of milliseconds per clock tick configured for
 *   this application.
 *
 * @par Notes
 * See #CONFIGURE_MICROSECONDS_PER_TICK.
 */
#define rtems_configuration_get_milliseconds_per_tick() \
  ( _Watchdog_Microseconds_per_tick / 1000 )

/* Generated from spec:/rtems/config/if/get-nanoseconds-per-tick */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the number of microseconds per clock tick configured for this
 *   application.
 *
 * @return Returns the number of microseconds per clock tick configured for
 *   this application.
 *
 * @par Notes
 * See #CONFIGURE_MICROSECONDS_PER_TICK.
 */
#define rtems_configuration_get_nanoseconds_per_tick() \
  _Watchdog_Nanoseconds_per_tick

/* Generated from spec:/rtems/config/if/get-number-of-initial-extensions */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the number of initial extensions configured for this
 *   application.
 *
 * @return Returns the number of initial extensions configured for this
 *   application.
 *
 * @par Notes
 * See #CONFIGURE_INITIAL_EXTENSIONS.
 */
#define rtems_configuration_get_number_of_initial_extensions() \
  ((uint32_t) _User_extensions_Initial_count)

/* Generated from spec:/rtems/config/if/get-stack-allocate-hook */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the thread stack allocator allocate hook configured for this
 *   application.
 *
 * @return Returns the thread stack allocator allocate hook configured for this
 *   application.
 *
 * @par Notes
 * See #CONFIGURE_TASK_STACK_ALLOCATOR.
 */
#define rtems_configuration_get_stack_allocate_hook() _Stack_Allocator_allocate

/* Generated from spec:/rtems/config/if/get-stack-allocate-init-hook */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the thread stack allocator initialization hook configured for
 *   this application.
 *
 * @return Returns the thread stack allocator initialization hook configured
 *   for this application.
 *
 * @par Notes
 * See #CONFIGURE_TASK_STACK_ALLOCATOR_INIT.
 */
#define rtems_configuration_get_stack_allocate_init_hook() \
  _Stack_Allocator_initialize

/* Generated from spec:/rtems/config/if/get-stack-allocator-avoids-work-space */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Indicates if the thread stack allocator is configured to avoid the
 *   RTEMS Workspace for this application.
 *
 * @return Returns true, if the thread stack allocator is configured to avoid
 *   the RTEMS Workspace for this application, otherwise false.
 *
 * @par Notes
 * See #CONFIGURE_TASK_STACK_ALLOCATOR_AVOIDS_WORK_SPACE.
 */
#define rtems_configuration_get_stack_allocator_avoids_work_space() \
  _Stack_Allocator_avoids_workspace

/* Generated from spec:/rtems/config/if/get-stack-free-hook */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the thread stack allocator free hook configured for this
 *   application.
 *
 * @return Returns the thread stack allocator free hook configured for this
 *   application.
 *
 * @par Notes
 * See #CONFIGURE_TASK_STACK_DEALLOCATOR.
 */
#define rtems_configuration_get_stack_free_hook() _Stack_Allocator_free

/* Generated from spec:/rtems/config/if/get-stack-space-size */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the thread stack space size in bytes of configured for this
 *   application.
 *
 * @return Returns the thread stack space size in bytes of configured for this
 *   application.
 */
uintptr_t rtems_configuration_get_stack_space_size( void );

/* Generated from spec:/rtems/config/if/get-ticks-per-timeslice */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the clock ticks per timeslice configured for this application.
 *
 * @return Returns the clock ticks per timeslice configured for this
 *   application.
 *
 * @par Notes
 * See #CONFIGURE_TICKS_PER_TIMESLICE.
 */
#define rtems_configuration_get_ticks_per_timeslice() \
  _Watchdog_Ticks_per_timeslice

/* Generated from spec:/rtems/config/if/get-unified-work-area */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Indicates if the RTEMS Workspace and C Program Heap are configured to
 *   be unified for this application.
 *
 * @return Returns true, if the RTEMS Workspace and C Program Heap are
 *   configured to be unified for this application, otherwise false.
 *
 * @par Notes
 * See #CONFIGURE_UNIFIED_WORK_AREAS.
 */
#define rtems_configuration_get_unified_work_area() _Workspace_Is_unified

/* Generated from spec:/rtems/config/if/get-user-extension-table */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the initial extensions table configured for this application.
 *
 * @return Returns the pointer to the initial extensions table configured for
 *   this application.
 */
#define rtems_configuration_get_user_extension_table() \
  _User_extensions_Initial_extensions

/* Generated from spec:/rtems/config/if/get-user-multiprocessing-table */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the MPCI configuration table configured for this application.
 *
 * @return Returns the pointer to the MPCI configuration table configured for
 *   this application.
 */
#if defined(RTEMS_MULTIPROCESSING)
  #define rtems_configuration_get_user_multiprocessing_table() \
    ( &_MPCI_Configuration )
#else
  #define rtems_configuration_get_user_multiprocessing_table() NULL
#endif

/* Generated from spec:/rtems/config/if/get-version-string */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the RTEMS version string.
 *
 * @return Returns the pointer to the RTEMS version string.
 */
const char *rtems_get_version_string( void );

/* Generated from spec:/rtems/config/if/get-work-space-size */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the RTEMS Workspace size in bytes configured for this
 *   application.
 *
 * @return Returns the RTEMS Workspace size in bytes configured for this
 *   application.
 */
#define rtems_configuration_get_work_space_size() \
  ( _Workspace_Size + \
    ( rtems_configuration_get_stack_allocator_avoids_work_space() ? \
      0 : rtems_configuration_get_stack_space_size() ) )

/* Generated from spec:/rtems/config/if/has-hardware-fp */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief This constant evaluates to #TRUE, if this processor variant has
 *   hardware floating point support, otherwise to #FALSE.
 */
#define RTEMS_HAS_HARDWARE_FP CPU_HARDWARE_FP

/* Generated from spec:/rtems/config/if/resource-is-unlimited */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Indicates if the resource is unlimited.
 *
 * @param _resource is the resource number.
 *
 * @return Returns true, if the resource is unlimited, otherwise false.
 *
 * @par Notes
 * This function is implemented as a macro and can be used to define compile
 * time constants.
 */
#define rtems_resource_is_unlimited( _resource ) \
  _Objects_Is_unlimited( _resource )

/* Generated from spec:/rtems/config/if/resource-maximum-per-allocation */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Gets the maximum number per allocation of a resource number.
 *
 * @param _resource is the resource number.
 *
 * @return Returns the maximum number per allocation of a resource number.
 *
 * @par Notes
 * This function is implemented as a macro and can be used to define compile
 * time constants.
 */
#define rtems_resource_maximum_per_allocation( _resource ) \
  _Objects_Maximum_per_allocation( _resource )

/* Generated from spec:/rtems/config/if/stack-allocate-hook */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief A thread stack allocator allocate handler shall have this type.
 */
typedef Stack_Allocator_allocate rtems_stack_allocate_hook;

/* Generated from spec:/rtems/config/if/stack-allocate-init-hook */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief A thread stack allocator initialization handler shall have this type.
 */
typedef Stack_Allocator_initialize rtems_stack_allocate_init_hook;

/* Generated from spec:/rtems/config/if/stack-free-hook */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief A thread stack allocator free handler shall have this type.
 */
typedef Stack_Allocator_free rtems_stack_free_hook;

/* Generated from spec:/rtems/config/if/unlimited-objects */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief This flag is used in augment a resource number so that it indicates
 *   an unlimited resource.
 */
#define RTEMS_UNLIMITED_OBJECTS OBJECTS_UNLIMITED_OBJECTS

/* Generated from spec:/rtems/config/if/resource-unlimited */

/**
 * @ingroup RTEMSAPIConfig
 *
 * @brief Augments the resource number so that it indicates an unlimited
 *   resource.
 *
 * @param _resource is the resource number to augment.
 *
 * @return Returns the resource number augmented to indicate an unlimited
 *   resource.
 *
 * @par Notes
 * This function is implemented as a macro and can be used to define compile
 * time constants.
 */
#define rtems_resource_unlimited( _resource ) \
  ( ( _resource ) | RTEMS_UNLIMITED_OBJECTS )

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_CONFIG_H */
