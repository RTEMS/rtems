/**
 * @file
 *
 * @brief Table of User Defined Configuration Parameters
 *
 * This include file contains the table of user defined configuration
 * parameters.
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_CONFIG_H
#define _RTEMS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Unlimited object support. Changes the configuration table entry for POSIX
 *  or RTEMS APIs to bounded only by the memory of the work-space.
 *
 *  Use the macro to define the resource unlimited before placing in
 *  the configuration table.
 */

#include <rtems/score/object.h>
#define RTEMS_UNLIMITED_OBJECTS OBJECTS_UNLIMITED_OBJECTS

#define rtems_resource_unlimited(resource) \
  ( resource | RTEMS_UNLIMITED_OBJECTS )

#define rtems_resource_is_unlimited(resource) \
  _Objects_Is_unlimited(resource)

#define rtems_resource_maximum_per_allocation(resource) \
  _Objects_Maximum_per_allocation(resource)

/*
 *  This is kind of kludgy but it allows targets to totally ignore the
 *  optional APIs like POSIX safely.
 */

#ifdef RTEMS_POSIX_API
#include <rtems/posix/config.h>
#else
typedef void *posix_api_configuration_table;
#endif

#include <rtems/rtems/config.h>

#include <rtems/extension.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif

#if defined(RTEMS_MULTIPROCESSING)
/*
 *  The following records define the Multiprocessor Configuration
 *  Table.  This table defines the multiprocessor system
 *  characteristics which must be known by RTEMS in a multiprocessor
 *  system.
 */
typedef struct {
  /** This is the local node number. */
  uint32_t            node;
  /** This is the maximum number of nodes in system. */
  uint32_t            maximum_nodes;
  /** This is the maximum number of global objects. */
  uint32_t            maximum_global_objects;
  /** This is the maximum number of proxies. */
  uint32_t            maximum_proxies;

  /** 
   * The MPCI Receive server is assumed to have a stack of at least
   * minimum stack size.  This field specifies the amount of extra
   * stack this task will be given in bytes.
   */
  uint32_t            extra_mpci_receive_server_stack;

  /** This is a pointer to User/BSP provided MPCI Table. */
  rtems_mpci_table   *User_mpci_table;
} rtems_multiprocessing_table;
#endif

/**
 * @brief Task stack allocator initialization hook.
 *
 * @param[in] stack_space_size is the size of the stack space in bytes.
 */
typedef void (*rtems_stack_allocate_init_hook)( size_t stack_space_size );

/**
 * @brief Task stack allocator hook.
 *
 * @param[in] stack_size is the Size of the task stack in bytes.
 *
 * @retval NULL Not enough memory.
 * @retval other Pointer to task stack.
 */
typedef void *(*rtems_stack_allocate_hook)( size_t stack_size );

/**
 * @brief Task stack deallocator hook.
 *
 * @param[in] addr is a pointer to previously allocated task stack.
 */
typedef void (*rtems_stack_free_hook)( void *addr );

/*
 *  The following records define the Configuration Table.  The
 *  information contained in this table is required in all
 *  RTEMS systems, whether single or multiprocessor.  This
 *  table primarily defines the following:
 *
 *     + location and size of the RTEMS Workspace
 *     + microseconds per clock tick
 *     + clock ticks per task timeslice
 *     + required number of each object type for each API configured
 */
typedef struct {
  /**
   * This field specifies the size in bytes of the RTEMS Workspace.
   */
  uintptr_t                      work_space_size;

  /**
   * This field specifies the size in bytes of the RTEMS thread stack space.
   */
  uintptr_t                      stack_space_size;

  /** 
   * This field specifies the maximum number of dynamically installed
   * used extensions.
   */
  uint32_t                       maximum_extensions;

  /**
   * This field contains the maximum number of POSIX API
   * keys which are configured for this application.
   */
  uint32_t                       maximum_keys;

  /**
   * This field contains the maximum number of POSIX API
   * key value pairs which are configured for this application.
   *
   * @note There can be potentially be a key/value pair for
   *       every thread to use every key. But normally this
   *       many are not needed in a system.
   */
  uint32_t                            maximum_key_value_pairs;

  /** 
   * This field specifies the number of microseconds which elapse
   * between clock ticks.  This is the basis for RTEMS timing.
   */
  uint32_t                       microseconds_per_tick;

  /** 
   * This field specifies the number of nanoseconds which elapse
   * between clock ticks.  This value is derived from the
   * microseconds_per_tick field and provided to avoid calculation at
   * run-time.
   */
  uint32_t                       nanoseconds_per_tick;

  /** 
   * This field specifies the number of ticks in each task's timeslice.
   */
  uint32_t                       ticks_per_timeslice;

  /** 
   * This element points to the BSP's optional idle task which may override
   * the default one provided with RTEMS.
   */
  Thread                       (*idle_task)( uintptr_t );

  /** 
   * This field specifies the size of the IDLE task's stack.  If less than or
   * equal to the minimum stack size, then the IDLE task will have the minimum
   * stack size.
   */
  uint32_t                       idle_task_stack_size;

  /** 
   * This field specifies the size of the interrupt stack.  If less than or
   * equal to the minimum stack size, then the interrupt stack will be of
   * minimum stack size.
   */
  uint32_t                       interrupt_stack_size;

  /**
   * @brief Optional task stack allocator initialization hook.
   */
  rtems_stack_allocate_init_hook stack_allocate_init_hook;

  /**
   * @brief Optional task stack allocator hook.
   */
  rtems_stack_allocate_hook      stack_allocate_hook;

  /**
   * @brief Optional task stack free hook.
   */
  rtems_stack_free_hook          stack_free_hook;

  /** 
   * If this element is TRUE, then RTEMS will zero the Executive Workspace.
   * When this element is FALSE, it is assumed that the BSP or invoking
   * environment has ensured that memory was cleared before RTEMS was
   * invoked.
   */
  bool                           do_zero_of_workspace;

  /**
   * @brief Specifies if a unified work area is used or not.
   *
   * If this element is @a true, then the RTEMS Workspace and the C Program
   * Heap use the same heap, otherwise they use separate heaps.
   */
  bool                           unified_work_area;

  /**
   * @brief Specifies if the stack allocator avoids the work space.
   *
   * If this element is @a true, then the stack allocator must not allocate the
   * thread stacks from the RTEMS Workspace, otherwise it should allocate the
   * thread stacks from the RTEMS Workspace.
   */
  bool                           stack_allocator_avoids_work_space;

  #ifdef RTEMS_SMP
    bool                         smp_enabled;
  #endif

  uint32_t                       number_of_initial_extensions;
  const rtems_extensions_table  *User_extension_table;
  #if defined(RTEMS_MULTIPROCESSING)
    rtems_multiprocessing_table   *User_multiprocessing_table;
  #endif
  #ifdef RTEMS_SMP
    uint32_t                     maximum_processors;
  #endif
} rtems_configuration_table;

/**
 * This is the configuration table generated by confdefs.h.
 */
extern const rtems_configuration_table Configuration;

#if defined(RTEMS_MULTIPROCESSING)
  /**
   *  This points to the multiprocessing configuration table.
   */
  SAPI_EXTERN rtems_multiprocessing_table  *_Configuration_MP_table;
#endif

#if defined(RTEMS_MULTIPROCESSING)
  /**
   * @brief RTEMS multiprocessing configuration table.
   *
   * This is the RTEMS Multiprocessing Configuration Table expected to
   * be generated by confdefs.h.
   */
  extern rtems_multiprocessing_table  Multiprocessing_configuration;
#endif


/*
 *  Some handy macros to avoid dependencies on either the BSP
 *  or the exact format of the configuration table.
 */

#define rtems_configuration_get_unified_work_area() \
        (Configuration.unified_work_area)

#define rtems_configuration_get_stack_allocator_avoids_work_space() \
        (Configuration.stack_allocator_avoids_work_space)

#define rtems_configuration_get_stack_space_size() \
        (Configuration.stack_space_size)

#define rtems_configuration_get_work_space_size() \
        (Configuration.work_space_size + \
          (rtems_configuration_get_stack_allocator_avoids_work_space() ? \
            0 : rtems_configuration_get_stack_space_size()))

#define rtems_configuration_get_maximum_extensions() \
        (Configuration.maximum_extensions)

#define rtems_configuration_get_microseconds_per_tick() \
        (Configuration.microseconds_per_tick)
#define rtems_configuration_get_milliseconds_per_tick() \
        (Configuration.microseconds_per_tick / 1000)
#define rtems_configuration_get_nanoseconds_per_tick() \
        (Configuration.nanoseconds_per_tick)

#define rtems_configuration_get_ticks_per_timeslice() \
        (Configuration.ticks_per_timeslice)

#define rtems_configuration_get_idle_task() \
        (Configuration.idle_task)

#define rtems_configuration_get_idle_task_stack_size() \
        (Configuration.idle_task_stack_size)

#define rtems_configuration_get_interrupt_stack_size() \
        (Configuration.interrupt_stack_size)

#define rtems_configuration_get_stack_allocate_init_hook() \
        (Configuration.stack_allocate_init_hook)

#define rtems_configuration_get_stack_allocate_hook() \
        (Configuration.stack_allocate_hook)

#define rtems_configuration_get_stack_free_hook() \
        (Configuration.stack_free_hook)

 /**
  * This macro assists in accessing the field which indicates whether
  * RTEMS is responsible for zeroing the Executive Workspace.
  */
#define rtems_configuration_get_do_zero_of_workspace() \
   (Configuration.do_zero_of_workspace)

#define rtems_configuration_get_number_of_initial_extensions() \
        (Configuration.number_of_initial_extensions)

#define rtems_configuration_get_user_extension_table() \
        (Configuration.User_extension_table)

#if defined(RTEMS_MULTIPROCESSING)
  #define rtems_configuration_get_user_multiprocessing_table() \
        (Configuration.User_multiprocessing_table)
#else
  #define rtems_configuration_get_user_multiprocessing_table() \
        NULL
#endif

/**
 * @brief Returns true if the SMP mode of operation is enabled, and false
 * otherwise.
 *
 * On single-processor configurations this is a compile time constant which
 * evaluates to false.
 *
 * @retval true SMP mode of operation is enabled.
 * @retval false Otherwise.
 */
#ifdef RTEMS_SMP
  #define rtems_configuration_is_smp_enabled() \
        (Configuration.smp_enabled)
#else
  #define rtems_configuration_is_smp_enabled() \
        false
#endif

/**
 * @brief Returns the configured maximum count of processors.
 *
 * The actual number of processors available for the application will be less
 * than or equal to the configured maximum count of processors.
 *
 * On single-processor configurations this is a compile time constant which
 * evaluates to one.
 *
 * @return The configured maximum count of processors.
 */
#ifdef RTEMS_SMP
  #define rtems_configuration_get_maximum_processors() \
        (Configuration.maximum_processors)
#else
  #define rtems_configuration_get_maximum_processors() \
        1
#endif

#define rtems_configuration_get_rtems_api_configuration() \
        (&Configuration_RTEMS_API)

#define rtems_configuration_get_posix_api_configuration() \
        (&Configuration_POSIX_API)

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
