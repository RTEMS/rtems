/**
 * @file
 *
 * @brief Configuration Table Template that will be Instantiated
 * by an Application
 *
 * This include file contains the configuration table template that will
 * be instantiated by an application based on the setting of a number
 * of macros.  The macros are documented in the Configuring a System
 * chapter of the Classic API User's Guide
 */

/*
 *  COPYRIGHT (c) 1989-2015.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __CONFIGURATION_TEMPLATE_h
#define __CONFIGURATION_TEMPLATE_h

/*
 * Include the executive's configuration
 */
#include <rtems.h>
#include <rtems/extensiondata.h>
#include <rtems/ioimpl.h>
#include <rtems/sysinit.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/percpu.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/rtems/barrierdata.h>
#include <rtems/rtems/dpmemdata.h>
#include <rtems/rtems/messagedata.h>
#include <rtems/rtems/partdata.h>
#include <rtems/rtems/ratemondata.h>
#include <rtems/rtems/regiondata.h>
#include <rtems/rtems/semdata.h>
#include <rtems/rtems/tasksdata.h>
#include <rtems/rtems/timerdata.h>
#include <rtems/posix/key.h>
#include <rtems/posix/mqueue.h>
#include <rtems/posix/psignal.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/shm.h>
#include <rtems/posix/timer.h>

#include <limits.h>

#ifdef CONFIGURE_DISABLE_BSP_SETTINGS
  #undef BSP_DEFAULT_UNIFIED_WORK_AREAS
  #undef BSP_IDLE_TASK_BODY
  #undef BSP_IDLE_TASK_STACK_SIZE
  #undef BSP_INITIAL_EXTENSION
  #undef BSP_INTERRUPT_STACK_SIZE
  #undef BSP_MAXIMUM_DEVICES
  #undef BSP_ZERO_WORKSPACE_AUTOMATICALLY
  #undef CONFIGURE_BSP_PREREQUISITE_DRIVERS
  #undef CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK
#else
  #include <bsp.h>
#endif

#ifdef RTEMS_NEWLIB
  #include <sys/reent.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Internal defines must be prefixed with _CONFIGURE to distinguish them from
 * user-provided options which use a CONFIGURE prefix.
 */

/**
 * @defgroup Configuration RTEMS Configuration
 *
 * This module contains all RTEMS Configuration parameters.
 *
 * The model is to estimate the memory required for each configured item
 * and sum those estimates.  The estimate can be too high or too low for
 * a variety of reasons:
 *
 * Reasons estimate is too high:
 *   + FP contexts (not all tasks are FP)
 *
 * Reasons estimate is too low:
 *   + stacks greater than minimum size
 *   + messages
 *   + application must account for device driver resources
 *   + application must account for add-on library resource requirements
 *
 * NOTE:  Eventually this may be able to take into account some of
 *        the above.  This procedure has evolved from just enough to
 *        support the RTEMS Test Suites into something that can be
 *        used remarkably reliably by most applications.
 */

/**
 * This is the Classic API initialization tasks table.
 */
extern rtems_initialization_tasks_table Initialization_tasks[];

#if defined(RTEMS_MULTIPROCESSING)
  /**
   * This it the distributed multiprocessing configuration table.
   */
  extern rtems_multiprocessing_table      Multiprocessing_configuration;
#endif

/**
 * This macro determines whether the RTEMS reentrancy support for
 * the Newlib C Library is enabled.
 */
#ifdef RTEMS_SCHEDSIM
  #undef RTEMS_NEWLIB
#endif

#if (defined(RTEMS_NEWLIB) && !defined(CONFIGURE_DISABLE_NEWLIB_REENTRANCY))
  #define _CONFIGURE_NEWLIB_EXTENSION 1
#else
  #define _CONFIGURE_NEWLIB_EXTENSION 0
#endif

#ifndef RTEMS_SCHEDSIM
#include <rtems/libio_.h>

#ifdef CONFIGURE_INIT
  #ifndef CONFIGURE_APPLICATION_DISABLE_FILESYSTEM
    RTEMS_SYSINIT_ITEM(
      rtems_filesystem_initialize,
      RTEMS_SYSINIT_ROOT_FILESYSTEM,
      RTEMS_SYSINIT_ORDER_MIDDLE
    );
  #endif
#endif
#endif

/**
 * This macro defines the number of POSIX file descriptors allocated
 * and managed by libio.  These are the "integer" file descriptors that
 * are used by calls like open(2) and read(2).
 */
#ifndef CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS
  #define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 3
#endif

/*
 * POSIX key count used by the IO library.
 */
#define _CONFIGURE_LIBIO_POSIX_KEYS 1

#ifdef CONFIGURE_INIT
  rtems_libio_t rtems_libio_iops[CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS];

  /**
   * When instantiating the configuration tables, this variable is
   * initialized to specify the maximum number of file descriptors.
   */
  const uint32_t rtems_libio_number_iops = RTEMS_ARRAY_SIZE(rtems_libio_iops);
#endif

#ifdef CONFIGURE_SMP_MAXIMUM_PROCESSORS
  #warning "CONFIGURE_SMP_MAXIMUM_PROCESSORS has been renamed to CONFIGURE_MAXIMUM_PROCESSORS since RTEMS 5.1"
  #define CONFIGURE_MAXIMUM_PROCESSORS CONFIGURE_SMP_MAXIMUM_PROCESSORS
#endif

/* Ensure that _CONFIGURE_MAXIMUM_PROCESSORS > 1 only in SMP configurations */
#if defined(CONFIGURE_MAXIMUM_PROCESSORS) && defined(RTEMS_SMP)
  #define _CONFIGURE_MAXIMUM_PROCESSORS CONFIGURE_MAXIMUM_PROCESSORS
#else
  #define _CONFIGURE_MAXIMUM_PROCESSORS 1
#endif

/*
 * An internal define to indicate that this is an SMP application
 * configuration.
 */
#ifdef RTEMS_SMP
  #if !defined(CONFIGURE_DISABLE_SMP_CONFIGURATION)
    #define _CONFIGURE_SMP_APPLICATION
  #elif _CONFIGURE_MAXIMUM_PROCESSORS > 1
    #error "CONFIGURE_DISABLE_SMP_CONFIGURATION and CONFIGURE_MAXIMUM_PROCESSORS > 1 makes no sense"
  #endif
#endif

#ifdef CONFIGURE_SMP_APPLICATION
  #warning "CONFIGURE_SMP_APPLICATION is obsolete since RTEMS 5.1"
#endif

/*
 * This sets up the resources for the FIFOs/pipes.
 */

/**
 * This is specified to configure the maximum number of POSIX FIFOs.
 */
#if !defined(CONFIGURE_MAXIMUM_FIFOS)
  #define CONFIGURE_MAXIMUM_FIFOS 0
#endif

/**
 * This is specified to configure the maximum number of POSIX named pipes.
 */
#if !defined(CONFIGURE_MAXIMUM_PIPES)
  #define CONFIGURE_MAXIMUM_PIPES 0
#endif

/*
 * This specifies the number of barriers required for the configured
 * number of FIFOs and named pipes.
 */
#if CONFIGURE_MAXIMUM_FIFOS > 0 || CONFIGURE_MAXIMUM_PIPES > 0
  #define _CONFIGURE_BARRIERS_FOR_FIFOS \
    (2 * (CONFIGURE_MAXIMUM_FIFOS + CONFIGURE_MAXIMUM_PIPES))
#else
  #define _CONFIGURE_BARRIERS_FOR_FIFOS   0
#endif

/**
 *  @defgroup ConfigFilesystems Filesystems and Mount Table Configuration
 *
 *  @ingroup Configuration
 *
 *  Defines to control the file system:
 *
 *   - CONFIGURE_APPLICATION_DISABLE_FILESYSTEM:
 *     Disable the RTEMS filesystems. You get an empty DEVFS.
 *
 *   - CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM:
 *     Use the DEVFS as the root file system. Limited functions are
 *     provided when this is used.
 *
 *   - CONFIGURE_FILESYSTEM_ALL:
 *     Add file filesystems to the default filesystem table.
 *
 *   List of available file systems. You can define as many as you like:
 *     - CONFIGURE_FILESYSTEM_IMFS   - In Memory File System (IMFS)
 *     - CONFIGURE_FILESYSTEM_DEVFS  - Device File System (DSVFS)
 *     - CONFIGURE_FILESYSTEM_TFTPFS - TFTP File System, networking enabled
 *     - CONFIGURE_FILESYSTEM_FTPFS  - FTP File System, networking enabled
 *     - CONFIGURE_FILESYSTEM_NFS    - Network File System, networking enabled
 *     - CONFIGURE_FILESYSTEM_DOSFS  - DOS File System, uses libblock
 *     - CONFIGURE_FILESYSTEM_RFS    - RTEMS File System (RFS), uses libblock
 *     - CONFIGURE_FILESYSTEM_JFFS2  - Journalling Flash File System, Version 2
 *
 *   Combinations:
 *
 *    - If nothing is defined the base file system is the IMFS.
 *
 *    - If CONFIGURE_APPLICATION_DISABLE_FILESYSTEM is defined all filesystems
 *      are disabled by force.
 *
 *    - If CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM is defined all filesystems
 *      are disabled by force and DEVFS is defined.
 */
/**@{*/

#ifdef CONFIGURE_INIT

  /*
   * Include all file systems. Do this before checking if the filesystem has
   * been disabled.
   */
  #ifdef CONFIGURE_FILESYSTEM_ALL
    #define CONFIGURE_FILESYSTEM_IMFS
    #define CONFIGURE_FILESYSTEM_DEVFS
    #define CONFIGURE_FILESYSTEM_TFTPFS
    #define CONFIGURE_FILESYSTEM_FTPFS
    #define CONFIGURE_FILESYSTEM_NFS
    #define CONFIGURE_FILESYSTEM_DOSFS
    #define CONFIGURE_FILESYSTEM_RFS
    #define CONFIGURE_FILESYSTEM_JFFS2
  #endif

  /*
   * If disabling the file system, give a compile error if the user has
   * configured other filesystem parameters.
   */
  #if defined(CONFIGURE_APPLICATION_DISABLE_FILESYSTEM)
     #if defined(CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM)
       #error "Filesystem disabled and a base filesystem configured."
     #endif

     #if defined(CONFIGURE_FILESYSTEM_IMFS) || \
       defined(CONFIGURE_FILESYSTEM_DEVFS) || \
       defined(CONFIGURE_FILESYSTEM_TFTPFS) || \
       defined(CONFIGURE_FILESYSTEM_FTPFS) || \
       defined(CONFIGURE_FILESYSTEM_NFS) || \
       defined(CONFIGURE_FILESYSTEM_DOSFS) || \
       defined(CONFIGURE_FILESYSTEM_RFS) || \
       defined(CONFIGURE_FILESYSTEM_JFFS2)
       #error "Filesystem disabled and a filesystem configured."
     #endif
  #endif

  /*
   * If the base filesystem is DEVFS define it else define IMFS.
   * We will have either DEVFS or IMFS defined after this.
   */
  #if !defined(CONFIGURE_APPLICATION_DISABLE_FILESYSTEM)
    #if defined(CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM)
      #define CONFIGURE_FILESYSTEM_DEVFS
    #endif
  #endif

#endif

#ifndef RTEMS_SCHEDSIM
/**
 * IMFS
 */
#include <rtems/imfs.h>

/**
 * This specifies the number of bytes per block for files within the IMFS.
 * There are a maximum number of blocks per file so this dictates the maximum
 * size of a file.  This has to be balanced with the unused portion of each
 * block that might be wasted.
 */
#ifndef CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK
  #define CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK \
                    IMFS_MEMFILE_DEFAULT_BYTES_PER_BLOCK
#endif

/**
 * This defines the IMFS file system table entry.
 */
#if !defined(CONFIGURE_FILESYSTEM_ENTRY_IMFS) && \
  defined(CONFIGURE_FILESYSTEM_IMFS)
  #define CONFIGURE_FILESYSTEM_ENTRY_IMFS \
    { RTEMS_FILESYSTEM_TYPE_IMFS, IMFS_initialize }
#endif
#endif

#ifdef CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM
  #define CONFIGURE_IMFS_DISABLE_CHMOD
  #define CONFIGURE_IMFS_DISABLE_CHOWN
  #define CONFIGURE_IMFS_DISABLE_UTIME
  #define CONFIGURE_IMFS_DISABLE_LINK
  #define CONFIGURE_IMFS_DISABLE_SYMLINK
  #define CONFIGURE_IMFS_DISABLE_READLINK
  #define CONFIGURE_IMFS_DISABLE_RENAME
  #define CONFIGURE_IMFS_DISABLE_UNMOUNT
#endif

/**
 * DEVFS
 */
#if !defined(CONFIGURE_FILESYSTEM_ENTRY_DEVFS) && \
    defined(CONFIGURE_FILESYSTEM_DEVFS)
#include <rtems/devfs.h>
  #define CONFIGURE_FILESYSTEM_ENTRY_DEVFS \
    { RTEMS_FILESYSTEM_TYPE_DEVFS, devFS_initialize }
#endif

/**
 * FTPFS
 */
#if !defined(CONFIGURE_FILESYSTEM_ENTRY_FTPFS) && \
    defined(CONFIGURE_FILESYSTEM_FTPFS)
  #include <rtems/ftpfs.h>
  #define CONFIGURE_FILESYSTEM_ENTRY_FTPFS \
    { RTEMS_FILESYSTEM_TYPE_FTPFS, rtems_ftpfs_initialize }
#endif

/**
 * TFTPFS
 */
#if !defined(CONFIGURE_FILESYSTEM_ENTRY_TFTPFS) && \
    defined(CONFIGURE_FILESYSTEM_TFTPFS)
  #include <rtems/tftp.h>
  #define CONFIGURE_FILESYSTEM_ENTRY_TFTPFS \
    { RTEMS_FILESYSTEM_TYPE_TFTPFS, rtems_tftpfs_initialize }
#endif

/**
 * NFS
 */
#if !defined(CONFIGURE_FILESYSTEM_ENTRY_NFS) && \
    defined(CONFIGURE_FILESYSTEM_NFS)
  #include <librtemsNfs.h>
  #define CONFIGURE_FILESYSTEM_ENTRY_NFS \
    { RTEMS_FILESYSTEM_TYPE_NFS, rtems_nfs_initialize }
#endif

/**
 * DOSFS
 */
#if !defined(CONFIGURE_FILESYSTEM_ENTRY_DOSFS) && \
    defined(CONFIGURE_FILESYSTEM_DOSFS)
  #include <rtems/dosfs.h>
  #define CONFIGURE_FILESYSTEM_ENTRY_DOSFS \
    { RTEMS_FILESYSTEM_TYPE_DOSFS, rtems_dosfs_initialize }
#endif

/**
 * RFS
 */
#if !defined(CONFIGURE_FILESYSTEM_ENTRY_RFS) && \
    defined(CONFIGURE_FILESYSTEM_RFS)
  #include <rtems/rtems-rfs.h>
  #define CONFIGURE_FILESYSTEM_ENTRY_RFS \
    { RTEMS_FILESYSTEM_TYPE_RFS, rtems_rfs_rtems_initialise }
#endif

/**
 * JFFS2
 */
#if !defined(CONFIGURE_FILESYSTEM_ENTRY_JFFS2) && \
    defined(CONFIGURE_FILESYSTEM_JFFS2)
  #include <rtems/jffs2.h>
  #define CONFIGURE_FILESYSTEM_ENTRY_JFFS2 \
    { RTEMS_FILESYSTEM_TYPE_JFFS2, rtems_jffs2_initialize }
#endif

#ifdef CONFIGURE_INIT

  /**
   * DEVFS variables.
   *
   * The number of individual devices that may be registered
   * in the system or the CONFIGURE_MAXIMUM_DEVICES variable
   * is defaulted to 4 when a filesystem is enabled, unless
   * the bsp overwrides this.  In which case the value is set
   * to BSP_MAXIMUM_DEVICES.
   */
  #ifdef CONFIGURE_FILESYSTEM_DEVFS
    #ifndef CONFIGURE_MAXIMUM_DEVICES
      #if defined(BSP_MAXIMUM_DEVICES)
        #define CONFIGURE_MAXIMUM_DEVICES BSP_MAXIMUM_DEVICES
      #else
        #define CONFIGURE_MAXIMUM_DEVICES 4
      #endif
    #endif
    #include <rtems/devfs.h>
  #endif

  /**
   * Table termination record.
   */
  #define CONFIGURE_FILESYSTEM_NULL { NULL, NULL }

#ifndef RTEMS_SCHEDSIM
  #if !defined(CONFIGURE_APPLICATION_DISABLE_FILESYSTEM) && \
    !defined(CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM)
    int imfs_rq_memfile_bytes_per_block =
      CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK;
  #endif

  /**
   * The default file system table. Must be terminated with the NULL entry if
   * you provide your own.
   */
  #ifndef CONFIGURE_APPLICATION_DISABLE_FILESYSTEM
    const rtems_filesystem_table_t rtems_filesystem_table[] = {
      #if !defined(CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM)
        { "/", IMFS_initialize_support },
      #endif
      #if defined(CONFIGURE_FILESYSTEM_IMFS) && \
          defined(CONFIGURE_FILESYSTEM_ENTRY_IMFS)
        CONFIGURE_FILESYSTEM_ENTRY_IMFS,
      #endif
      #if defined(CONFIGURE_FILESYSTEM_DEVFS) && \
          defined(CONFIGURE_FILESYSTEM_ENTRY_DEVFS)
        CONFIGURE_FILESYSTEM_ENTRY_DEVFS,
      #endif
      #if defined(CONFIGURE_FILESYSTEM_TFTPFS) && \
          defined(CONFIGURE_FILESYSTEM_ENTRY_TFTPFS)
        CONFIGURE_FILESYSTEM_ENTRY_TFTPFS,
      #endif
      #if defined(CONFIGURE_FILESYSTEM_FTPFS) && \
          defined(CONFIGURE_FILESYSTEM_ENTRY_FTPFS)
        CONFIGURE_FILESYSTEM_ENTRY_FTPFS,
      #endif
      #if defined(CONFIGURE_FILESYSTEM_NFS) && \
          defined(CONFIGURE_FILESYSTEM_ENTRY_NFS)
        CONFIGURE_FILESYSTEM_ENTRY_NFS,
      #endif
      #if defined(CONFIGURE_FILESYSTEM_DOSFS) && \
          defined(CONFIGURE_FILESYSTEM_ENTRY_DOSFS)
        CONFIGURE_FILESYSTEM_ENTRY_DOSFS,
      #endif
      #if defined(CONFIGURE_FILESYSTEM_RFS) && \
          defined(CONFIGURE_FILESYSTEM_ENTRY_RFS)
        CONFIGURE_FILESYSTEM_ENTRY_RFS,
      #endif
      #if defined(CONFIGURE_FILESYSTEM_JFFS2) && \
          defined(CONFIGURE_FILESYSTEM_ENTRY_JFFS2)
        CONFIGURE_FILESYSTEM_ENTRY_JFFS2,
      #endif
      CONFIGURE_FILESYSTEM_NULL
    };

    #if defined(CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM)
      static devFS_node devFS_root_filesystem_nodes [CONFIGURE_MAXIMUM_DEVICES];
      static const devFS_data devFS_root_filesystem_data = {
        devFS_root_filesystem_nodes,
        CONFIGURE_MAXIMUM_DEVICES
      };
    #else
      static IMFS_fs_info_t _Configure_IMFS_fs_info;

      static const rtems_filesystem_operations_table _Configure_IMFS_ops = {
        rtems_filesystem_default_lock,
        rtems_filesystem_default_unlock,
        IMFS_eval_path,
        #ifdef CONFIGURE_IMFS_DISABLE_LINK
          rtems_filesystem_default_link,
        #else
          IMFS_link,
        #endif
        rtems_filesystem_default_are_nodes_equal,
        #ifdef CONFIGURE_IMFS_DISABLE_MKNOD
          rtems_filesystem_default_mknod,
        #else
          IMFS_mknod,
        #endif
        #ifdef CONFIGURE_IMFS_DISABLE_RMNOD
          rtems_filesystem_default_rmnod,
        #else
          IMFS_rmnod,
        #endif
        #ifdef CONFIGURE_IMFS_DISABLE_CHMOD
          rtems_filesystem_default_fchmod,
        #else
          IMFS_fchmod,
        #endif
        #ifdef CONFIGURE_IMFS_DISABLE_CHOWN
          rtems_filesystem_default_chown,
        #else
          IMFS_chown,
        #endif
        IMFS_node_clone,
        IMFS_node_free,
        #ifdef CONFIGURE_IMFS_DISABLE_MOUNT
          rtems_filesystem_default_mount,
        #else
          IMFS_mount,
        #endif
        #ifdef CONFIGURE_IMFS_DISABLE_UNMOUNT
          rtems_filesystem_default_unmount,
        #else
          IMFS_unmount,
        #endif
        rtems_filesystem_default_fsunmount,
        #ifdef CONFIGURE_IMFS_DISABLE_UTIME
          rtems_filesystem_default_utime,
        #else
          IMFS_utime,
        #endif
        #ifdef CONFIGURE_IMFS_DISABLE_SYMLINK
          rtems_filesystem_default_symlink,
        #else
          IMFS_symlink,
        #endif
        #ifdef CONFIGURE_IMFS_DISABLE_READLINK
          rtems_filesystem_default_readlink,
        #else
          IMFS_readlink,
        #endif
        #ifdef CONFIGURE_IMFS_DISABLE_RENAME
          rtems_filesystem_default_rename,
        #else
          IMFS_rename,
        #endif
        rtems_filesystem_default_statvfs
      };

      static const IMFS_mknod_controls _Configure_IMFS_mknod_controls = {
        #ifdef CONFIGURE_IMFS_DISABLE_READDIR
          &IMFS_mknod_control_dir_minimal,
        #else
          &IMFS_mknod_control_dir_default,
        #endif
        &IMFS_mknod_control_device,
        #ifdef CONFIGURE_IMFS_DISABLE_MKNOD_FILE
          &IMFS_mknod_control_enosys,
        #else
          &IMFS_mknod_control_memfile,
        #endif
        #if CONFIGURE_MAXIMUM_FIFOS > 0 || CONFIGURE_MAXIMUM_PIPES > 0
          &IMFS_mknod_control_fifo
        #else
          &IMFS_mknod_control_enosys
        #endif
      };

      static const IMFS_mount_data _Configure_IMFS_mount_data = {
        &_Configure_IMFS_fs_info,
        &_Configure_IMFS_ops,
        &_Configure_IMFS_mknod_controls
      };
    #endif

    const rtems_filesystem_mount_configuration
      rtems_filesystem_root_configuration = {
      NULL,
      NULL,
      #if defined(CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM)
        RTEMS_FILESYSTEM_TYPE_DEVFS,
      #else
        "/",
      #endif
      RTEMS_FILESYSTEM_READ_WRITE,
      #if defined(CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM)
        &devFS_root_filesystem_data
      #else
        &_Configure_IMFS_mount_data
      #endif
    };
  #endif

#endif
#endif
/**@}*/ /* end of file system group */

/*
 *  STACK_CHECKER_ON was still available in 4.9 so give a warning for now.
 */
#if defined(STACK_CHECKER_ON)
  #define CONFIGURE_STACK_CHECKER_ENABLED
  #warning "STACK_CHECKER_ON deprecated -- use CONFIGURE_STACK_CHECKER_ENABLED"
#endif

/**
 * This configures the stack checker user extension.
 */
#ifdef CONFIGURE_STACK_CHECKER_ENABLED
  #define _CONFIGURE_STACK_CHECKER_EXTENSION 1
#else
  #define _CONFIGURE_STACK_CHECKER_EXTENSION 0
#endif

/**
 * @brief Maximum priority configuration.
 *
 * This configures the maximum priority value that
 * a task may have.
 *
 * The following applies to the data space requirements
 * of the Priority Scheduler.
 *
 * By reducing the number of priorities in a system,
 * the amount of RAM required by RTEMS can be significantly
 * reduced.  RTEMS allocates a Chain_Control structure per
 * priority and this structure contains 3 pointers.  So
 * the default is (256 * 12) = 3K on 32-bit architectures.
 *
 * This must be one less than a power of 2 between
 * 4 and 256.  Valid values along with the application
 * priority levels and memory saved when pointers are
 * 32-bits in size are:
 *
 *   + 3,  2 application priorities, 3024 bytes saved
 *   + 7, 5 application priorities, 2976 bytes saved
 *   + 15, 13 application priorities, 2880 bytes saved
 *   + 31, 29 application priorities, 2688 bytes saved
 *   + 63, 61 application priorities, 2304 bytes saved
 *   + 127, 125 application priorities, 1536 bytes saved
 *   + 255, 253 application priorities, 0 bytes saved
 *
 * It is specified in terms of Classic API priority values.
 */
#ifndef CONFIGURE_MAXIMUM_PRIORITY
  #define CONFIGURE_MAXIMUM_PRIORITY PRIORITY_DEFAULT_MAXIMUM
#endif

/**
 *  @defgroup ConfigScheduler Scheduler configuration
 *
 *  @ingroup Configuration
 *
 * The scheduler configuration allows an application to select the
 * scheduling policy to use.  The supported configurations are:
 *
 *  - CONFIGURE_SCHEDULER_PRIORITY - Deterministic Priority Scheduler
 *  - CONFIGURE_SCHEDULER_PRIORITY_SMP - Deterministic Priority SMP Scheduler
 *  - CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP - Deterministic
 *    Priority SMP Affinity Scheduler
 *  - CONFIGURE_SCHEDULER_STRONG_APA - Strong APA Scheduler
 *  - CONFIGURE_SCHEDULER_SIMPLE - Light-weight Priority Scheduler
 *  - CONFIGURE_SCHEDULER_SIMPLE_SMP - Simple SMP Priority Scheduler
 *  - CONFIGURE_SCHEDULER_EDF - EDF Scheduler
 *  - CONFIGURE_SCHEDULER_EDF_SMP - EDF SMP Scheduler
 *  - CONFIGURE_SCHEDULER_CBS - CBS Scheduler
 *  - CONFIGURE_SCHEDULER_USER  - user provided scheduler
 *
 * If no configuration is specified by the application in a uniprocessor
 * configuration, then CONFIGURE_SCHEDULER_PRIORITY is the default.
 *
 * If no configuration is specified by the application in SMP
 * configuration, then CONFIGURE_SCHEDULER_PRIORITY_SMP is the default.
 *
 * An application can define its own scheduling policy by defining
 * CONFIGURE_SCHEDULER_USER and the following:
 *
 *    - CONFIGURE_SCHEDULER
 *    - CONFIGURE_SCHEDULER_TABLE_ENTRIES
 *    - CONFIGURE_SCHEDULER_USER_PER_THREAD
 */

#ifdef CONFIGURE_SCHEDULER_CONTEXT
  #warning "CONFIGURE_SCHEDULER_CONTEXT has been renamed to CONFIGURE_SCHEDULER since RTEMS 5.1"
  #define CONFIGURE_SCHEDULER CONFIGURE_SCHEDULER_CONTEXT
#endif

#ifdef CONFIGURE_SCHEDULER_CONTROLS
  #warning "CONFIGURE_SCHEDULER_CONTROLS has been renamed to CONFIGURE_SCHEDULER_TABLE_ENTRIES since RTEMS 5.1"
  #define CONFIGURE_SCHEDULER_TABLE_ENTRIES CONFIGURE_SCHEDULER_CONTROLS
#endif

#ifdef CONFIGURE_SMP_SCHEDULER_ASSIGNMENTS
  #warning "CONFIGURE_SMP_SCHEDULER_ASSIGNMENTS has been renamed to CONFIGURE_SCHEDULER_ASSIGNMENTS since RTEMS 5.1"
  #define CONFIGURE_SCHEDULER_ASSIGNMENTS CONFIGURE_SMP_SCHEDULER_ASSIGNMENTS
#endif

#if !defined(CONFIGURE_SCHEDULER_USER) && \
    !defined(CONFIGURE_SCHEDULER_PRIORITY) && \
    !defined(CONFIGURE_SCHEDULER_PRIORITY_SMP) && \
    !defined(CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP) && \
    !defined(CONFIGURE_SCHEDULER_STRONG_APA) && \
    !defined(CONFIGURE_SCHEDULER_SIMPLE) && \
    !defined(CONFIGURE_SCHEDULER_SIMPLE_SMP) && \
    !defined(CONFIGURE_SCHEDULER_EDF) && \
    !defined(CONFIGURE_SCHEDULER_EDF_SMP) && \
    !defined(CONFIGURE_SCHEDULER_CBS)
  #if defined(RTEMS_SMP) && _CONFIGURE_MAXIMUM_PROCESSORS > 1
    /**
     * If no scheduler is specified in an SMP configuration, the
     * EDF scheduler is default.
     */
    #define CONFIGURE_SCHEDULER_EDF_SMP
  #else
    /**
     * If no scheduler is specified in a uniprocessor configuration, the
     * priority scheduler is default.
     */
    #define CONFIGURE_SCHEDULER_PRIORITY
  #endif
#endif

#include <rtems/scheduler.h>

/*
 * If the Priority Scheduler is selected, then configure for it.
 */
#if defined(CONFIGURE_SCHEDULER_PRIORITY)
  #if !defined(CONFIGURE_SCHEDULER_NAME)
    /** Configure the name of the scheduler instance */
    #define CONFIGURE_SCHEDULER_NAME rtems_build_name('U', 'P', 'D', ' ')
  #endif

  #if !defined(CONFIGURE_SCHEDULER_TABLE_ENTRIES)
    /** Configure the context needed by the scheduler instance */
    #define CONFIGURE_SCHEDULER \
      RTEMS_SCHEDULER_PRIORITY( \
        dflt, \
        CONFIGURE_MAXIMUM_PRIORITY + 1 \
      )

    /** Configure the controls for this scheduler instance */
    #define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
      RTEMS_SCHEDULER_TABLE_PRIORITY(dflt, CONFIGURE_SCHEDULER_NAME)
  #endif
#endif

/*
 * If the Deterministic Priority SMP Scheduler is selected, then configure for
 * it.
 */
#if defined(CONFIGURE_SCHEDULER_PRIORITY_SMP)
  #if !defined(CONFIGURE_SCHEDULER_NAME)
    /** Configure the name of the scheduler instance */
    #define CONFIGURE_SCHEDULER_NAME rtems_build_name('M', 'P', 'D', ' ')
  #endif

  #if !defined(CONFIGURE_SCHEDULER_TABLE_ENTRIES)
    /** Configure the context needed by the scheduler instance */
    #define CONFIGURE_SCHEDULER \
      RTEMS_SCHEDULER_PRIORITY_SMP( \
        dflt, \
        CONFIGURE_MAXIMUM_PRIORITY + 1 \
      )

    /** Configure the controls for this scheduler instance */
    #define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
      RTEMS_SCHEDULER_TABLE_PRIORITY_SMP(dflt, CONFIGURE_SCHEDULER_NAME)
  #endif
#endif

/*
 * If the Deterministic Priority Affinity SMP Scheduler is selected, then configure for
 * it.
 */
#if defined(CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP)
  #if !defined(CONFIGURE_SCHEDULER_NAME)
    /** Configure the name of the scheduler instance */
    #define CONFIGURE_SCHEDULER_NAME rtems_build_name('M', 'P', 'A', ' ')
  #endif

  #if !defined(CONFIGURE_SCHEDULER_TABLE_ENTRIES)
    /** Configure the context needed by the scheduler instance */
    #define CONFIGURE_SCHEDULER \
      RTEMS_SCHEDULER_PRIORITY_AFFINITY_SMP( \
        dflt, \
        CONFIGURE_MAXIMUM_PRIORITY + 1 \
      )

    /** Configure the controls for this scheduler instance */
    #define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
      RTEMS_SCHEDULER_TABLE_PRIORITY_AFFINITY_SMP( \
        dflt, \
        CONFIGURE_SCHEDULER_NAME \
      )
  #endif
#endif

/*
 * If the Strong APA Scheduler is selected, then configure for
 * it.
 */
#if defined(CONFIGURE_SCHEDULER_STRONG_APA)
  #if !defined(CONFIGURE_SCHEDULER_NAME)
    /** Configure the name of the scheduler instance */
    #define CONFIGURE_SCHEDULER_NAME rtems_build_name('M', 'A', 'P', 'A')
  #endif

  #if !defined(CONFIGURE_SCHEDULER_TABLE_ENTRIES)
    /** Configure the context needed by the scheduler instance */
    #define CONFIGURE_SCHEDULER \
      RTEMS_SCHEDULER_STRONG_APA( \
        dflt, \
        CONFIGURE_MAXIMUM_PRIORITY + 1 \
      )

    /** Configure the controls for this scheduler instance */
    #define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
      RTEMS_SCHEDULER_TABLE_STRONG_APA(dflt, CONFIGURE_SCHEDULER_NAME)
  #endif
#endif

/*
 * If the Simple Priority Scheduler is selected, then configure for it.
 */
#if defined(CONFIGURE_SCHEDULER_SIMPLE)
  #if !defined(CONFIGURE_SCHEDULER_NAME)
    /** Configure the name of the scheduler instance */
    #define CONFIGURE_SCHEDULER_NAME rtems_build_name('U', 'P', 'S', ' ')
  #endif

  #if !defined(CONFIGURE_SCHEDULER_TABLE_ENTRIES)
    /** Configure the context needed by the scheduler instance */
    #define CONFIGURE_SCHEDULER RTEMS_SCHEDULER_SIMPLE(dflt)

    /** Configure the controls for this scheduler instance */
    #define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
      RTEMS_SCHEDULER_TABLE_SIMPLE(dflt, CONFIGURE_SCHEDULER_NAME)
  #endif
#endif

/*
 * If the Simple SMP Priority Scheduler is selected, then configure for it.
 */
#if defined(CONFIGURE_SCHEDULER_SIMPLE_SMP)
  #if !defined(CONFIGURE_SCHEDULER_NAME)
    /** Configure the name of the scheduler instance */
    #define CONFIGURE_SCHEDULER_NAME rtems_build_name('M', 'P', 'S', ' ')
  #endif

  #if !defined(CONFIGURE_SCHEDULER_TABLE_ENTRIES)
    /** Configure the context needed by the scheduler instance */
    #define CONFIGURE_SCHEDULER \
      RTEMS_SCHEDULER_SIMPLE_SMP(dflt)

    /** Configure the controls for this scheduler instance */
    #define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
      RTEMS_SCHEDULER_TABLE_SIMPLE_SMP(dflt, CONFIGURE_SCHEDULER_NAME)
  #endif
#endif

/*
 * If the EDF Scheduler is selected, then configure for it.
 */
#if defined(CONFIGURE_SCHEDULER_EDF)
  #if !defined(CONFIGURE_SCHEDULER_NAME)
    /** Configure the name of the scheduler instance */
    #define CONFIGURE_SCHEDULER_NAME rtems_build_name('U', 'E', 'D', 'F')
  #endif

  #if !defined(CONFIGURE_SCHEDULER_TABLE_ENTRIES)
    /** Configure the context needed by the scheduler instance */
    #define CONFIGURE_SCHEDULER RTEMS_SCHEDULER_EDF(dflt)

    /** Configure the controls for this scheduler instance */
    #define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
      RTEMS_SCHEDULER_TABLE_EDF(dflt, CONFIGURE_SCHEDULER_NAME)
  #endif
#endif

/*
 * If the EDF SMP Scheduler is selected, then configure for it.
 */
#if defined(CONFIGURE_SCHEDULER_EDF_SMP)
  #if !defined(CONFIGURE_SCHEDULER_NAME)
    /** Configure the name of the scheduler instance */
    #define CONFIGURE_SCHEDULER_NAME rtems_build_name('M', 'E', 'D', 'F')
  #endif

  #if !defined(CONFIGURE_SCHEDULER_TABLE_ENTRIES)
    /** Configure the context needed by the scheduler instance */
    #define CONFIGURE_SCHEDULER \
      RTEMS_SCHEDULER_EDF_SMP(dflt, _CONFIGURE_MAXIMUM_PROCESSORS)

    /** Configure the controls for this scheduler instance */
    #define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
      RTEMS_SCHEDULER_TABLE_EDF_SMP(dflt, CONFIGURE_SCHEDULER_NAME)
  #endif
#endif

/*
 * If the CBS Scheduler is selected, then configure for it.
 */
#if defined(CONFIGURE_SCHEDULER_CBS)
  #if !defined(CONFIGURE_SCHEDULER_NAME)
    /** Configure the name of the scheduler instance */
    #define CONFIGURE_SCHEDULER_NAME rtems_build_name('U', 'C', 'B', 'S')
  #endif

  #if !defined(CONFIGURE_SCHEDULER_TABLE_ENTRIES)
    /** Configure the context needed by the scheduler instance */
    #define CONFIGURE_SCHEDULER RTEMS_SCHEDULER_CBS(dflt)

    /** Configure the controls for this scheduler instance */
    #define CONFIGURE_SCHEDULER_TABLE_ENTRIES \
      RTEMS_SCHEDULER_TABLE_CBS(dflt, CONFIGURE_SCHEDULER_NAME)
  #endif

  #ifndef CONFIGURE_CBS_MAXIMUM_SERVERS
    #define CONFIGURE_CBS_MAXIMUM_SERVERS CONFIGURE_MAXIMUM_TASKS
  #endif

  #ifdef CONFIGURE_INIT
    const uint32_t _Scheduler_CBS_Maximum_servers =
      CONFIGURE_CBS_MAXIMUM_SERVERS;

    Scheduler_CBS_Server
      _Scheduler_CBS_Server_list[ CONFIGURE_CBS_MAXIMUM_SERVERS ];
  #endif
#endif

/*
 * Set up the scheduler entry points table.  The scheduling code uses
 * this code to know which scheduler is configured by the user.
 */
#ifdef CONFIGURE_INIT
  #if defined(CONFIGURE_SCHEDULER)
    CONFIGURE_SCHEDULER;
  #endif

  const Scheduler_Control _Scheduler_Table[] = {
    CONFIGURE_SCHEDULER_TABLE_ENTRIES
  };

  #define _CONFIGURE_SCHEDULER_COUNT RTEMS_ARRAY_SIZE( _Scheduler_Table )

  #if defined(RTEMS_SMP)
    const size_t _Scheduler_Count = _CONFIGURE_SCHEDULER_COUNT;

    const Scheduler_Assignment _Scheduler_Initial_assignments[] = {
      #if defined(CONFIGURE_SCHEDULER_ASSIGNMENTS)
        CONFIGURE_SCHEDULER_ASSIGNMENTS
      #else
        #define _CONFIGURE_SCHEDULER_ASSIGN \
          RTEMS_SCHEDULER_ASSIGN( \
            0, \
            RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL \
          )
        _CONFIGURE_SCHEDULER_ASSIGN
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 2
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 3
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 4
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 5
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 6
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 7
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 8
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 9
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 10
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 11
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 12
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 13
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 14
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 15
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 16
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 17
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 18
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 19
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 20
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 21
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 22
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 23
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 24
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 25
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 26
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 27
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 28
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 29
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 30
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 31
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #if _CONFIGURE_MAXIMUM_PROCESSORS >= 32
          , _CONFIGURE_SCHEDULER_ASSIGN
        #endif
        #undef _CONFIGURE_SCHEDULER_ASSIGN
      #endif
    };

    RTEMS_STATIC_ASSERT(
      _CONFIGURE_MAXIMUM_PROCESSORS
        == RTEMS_ARRAY_SIZE( _Scheduler_Initial_assignments ),
      _Scheduler_Initial_assignments
    );
  #endif
#endif
/**@}*/ /* end of Scheduler Configuration */

/**
 * @defgroup ConfigurationIdle IDLE Thread Configuration
 *
 * @addtogroup Configuration
 *
 * This module contains configuration parameters related to the
 * set of IDLE threads. On a uniprocessor system, there is one
 * IDLE thread. On an SMP system, there is one for each core.
 */

/*
 *  If you said the IDLE task was going to do application initialization
 *  and didn't override the IDLE body, then something is amiss.
 */
#if (defined(CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION) && \
     !defined(CONFIGURE_IDLE_TASK_BODY))
  #error "CONFIGURE_ERROR: You did not override the IDLE task body."
#endif

/**
 * @brief Idle task body configuration.
 *
 * There is a default IDLE thread body provided by RTEMS which
 * has the possibility of being CPU specific.  There may be a
 * BSP specific override of the RTEMS default body and in turn,
 * the application may override and provide its own.
 */
#ifndef CONFIGURE_IDLE_TASK_BODY
  #if defined(BSP_IDLE_TASK_BODY)
    #define CONFIGURE_IDLE_TASK_BODY BSP_IDLE_TASK_BODY
  #else
    #define CONFIGURE_IDLE_TASK_BODY _CPU_Thread_Idle_body
  #endif
#endif
/**@}*/ /* end of IDLE thread configuration */

/**
 * @defgroup ConfigurationStackSize Configuration Thread Stack Size
 *
 * @addtogroup Configuration
 *
 * This module contains parameters related to thread and interrupt stacks.
 */

/**
 * By default, use the minimum stack size requested by this port.
 */
#ifndef CONFIGURE_MINIMUM_TASK_STACK_SIZE
  #define CONFIGURE_MINIMUM_TASK_STACK_SIZE CPU_STACK_MINIMUM_SIZE
#endif

/**
 * This specifies the default POSIX thread stack size. By default, it is
 * twice that recommended for the port.
 */
#ifndef CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE
#define CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE \
  (2 * CONFIGURE_MINIMUM_TASK_STACK_SIZE)
#endif

/**
 * @brief Idle task stack size configuration.
 *
 * By default, the IDLE task will have a stack of minimum size.
 * The BSP or application may override this value.
 */
#ifndef CONFIGURE_IDLE_TASK_STACK_SIZE
  #ifdef BSP_IDLE_TASK_STACK_SIZE
    #define CONFIGURE_IDLE_TASK_STACK_SIZE BSP_IDLE_TASK_STACK_SIZE
  #else
    #define CONFIGURE_IDLE_TASK_STACK_SIZE CONFIGURE_MINIMUM_TASK_STACK_SIZE
  #endif
#endif
#if CONFIGURE_IDLE_TASK_STACK_SIZE < CONFIGURE_MINIMUM_TASK_STACK_SIZE
  #error "CONFIGURE_IDLE_TASK_STACK_SIZE less than CONFIGURE_MINIMUM_TASK_STACK_SIZE"
#endif

/*
 * Interrupt stack configuration.
 *
 * By default, the interrupt stack will be of minimum size.
 * The BSP or application may override this value.
 */

#ifndef CONFIGURE_INTERRUPT_STACK_SIZE
  #ifdef BSP_INTERRUPT_STACK_SIZE
    #define CONFIGURE_INTERRUPT_STACK_SIZE BSP_INTERRUPT_STACK_SIZE
  #else
    #define CONFIGURE_INTERRUPT_STACK_SIZE CPU_STACK_MINIMUM_SIZE
  #endif
#endif

#if CONFIGURE_INTERRUPT_STACK_SIZE % CPU_INTERRUPT_STACK_ALIGNMENT != 0
  #error "CONFIGURE_INTERRUPT_STACK_SIZE fails to meet the CPU port interrupt stack alignment"
#endif

#ifdef CONFIGURE_INIT
  RTEMS_DEFINE_GLOBAL_SYMBOL(
    _ISR_Stack_size,
    CONFIGURE_INTERRUPT_STACK_SIZE
  );

  char _ISR_Stack_area_begin[
    _CONFIGURE_MAXIMUM_PROCESSORS * CONFIGURE_INTERRUPT_STACK_SIZE
  ] RTEMS_ALIGNED( CPU_INTERRUPT_STACK_ALIGNMENT )
  RTEMS_SECTION( ".rtemsstack.interrupt.begin" );

  const char _ISR_Stack_area_end[ 0 ]
    RTEMS_SECTION( ".rtemsstack.interrupt.end" ) = { };
#endif

/**
 * Configure the very much optional task stack allocator initialization
 */
#ifndef CONFIGURE_TASK_STACK_ALLOCATOR_INIT
  #define CONFIGURE_TASK_STACK_ALLOCATOR_INIT NULL
#endif

/*
 *  Configure the very much optional task stack allocator and deallocator.
 */
#if !defined(CONFIGURE_TASK_STACK_ALLOCATOR) \
  && !defined(CONFIGURE_TASK_STACK_DEALLOCATOR)
  /**
   * This specifies the task stack allocator method.
   */
  #define CONFIGURE_TASK_STACK_ALLOCATOR _Workspace_Allocate
  /**
   * This specifies the task stack deallocator method.
   */
  #define CONFIGURE_TASK_STACK_DEALLOCATOR _Workspace_Free
#elif (defined(CONFIGURE_TASK_STACK_ALLOCATOR) \
  && !defined(CONFIGURE_TASK_STACK_DEALLOCATOR)) \
    || (!defined(CONFIGURE_TASK_STACK_ALLOCATOR) \
      && defined(CONFIGURE_TASK_STACK_DEALLOCATOR))
  #error "CONFIGURE_TASK_STACK_ALLOCATOR and CONFIGURE_TASK_STACK_DEALLOCATOR must be both defined or both undefined"
#endif
/**@}*/ /* end of thread/interrupt stack configuration */

/**
 * @addtogroup Configuration
 */
/**@{*/

/**
 * Should the RTEMS Workspace and C Program Heap be cleared automatically
 * at system start up?
 */
#ifndef CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY
  #ifdef BSP_ZERO_WORKSPACE_AUTOMATICALLY
    #define CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY \
            BSP_ZERO_WORKSPACE_AUTOMATICALLY
  #else
    #define CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY FALSE
  #endif
#endif
/**@}*/ /* end of add to group Configuration */

/**
 * @defgroup ConfigurationMalloc RTEMS Malloc configuration
 *
 * This module contains parameters related to configuration of the RTEMS
 * Malloc implementation.
 */
/**@{*/
#include <rtems/malloc.h>

#ifdef CONFIGURE_INIT
  /**
   * By default, RTEMS uses separate heaps for the RTEMS Workspace and
   * the C Program Heap.  The application can choose optionally to combine
   * these to provide one larger memory pool. This is particularly
   * useful in combination with the unlimited objects configuration.
   */
  #ifdef CONFIGURE_UNIFIED_WORK_AREAS
    Heap_Control  *RTEMS_Malloc_Heap = &_Workspace_Area;
  #else
    Heap_Control   RTEMS_Malloc_Area;
    Heap_Control  *RTEMS_Malloc_Heap = &RTEMS_Malloc_Area;
  #endif
#endif

#ifdef CONFIGURE_INIT
  /**
   * This configures the sbrk() support for the malloc family.
   * By default it is assumed that the BSP provides all available
   * RAM to the malloc family implementation so sbrk()'ing to get
   * more memory would always fail anyway.
   */
  const rtems_heap_extend_handler rtems_malloc_extend_handler =
    #ifdef CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK
      rtems_heap_extend_via_sbrk;
    #else
      rtems_heap_null_extend;
    #endif
#endif

#ifdef CONFIGURE_INIT
  /**
   * This configures the malloc family plugin which dirties memory
   * allocated.  This is helpful for finding unitialized data structure
   * problems.
   */
  rtems_malloc_dirtier_t rtems_malloc_dirty_helper =
    #if defined(CONFIGURE_MALLOC_DIRTY)
      rtems_malloc_dirty_memory;
    #else
      NULL;
    #endif
#endif
/**@}*/  /* end of Malloc Configuration */

/**
 * @defgroup ConfigurationHelpers Configuration Helpers
 *
 * @ingroup Configuration
 *
 * This module contains items which are used internally to ease
 * the configuration calculations.
 */
/**@{*/

/**
 * Zero of one returns 0 if the parameter is 0 else 1 is returned.
 */
#define _Configure_Zero_or_One(_number) ((_number) != 0 ? 1 : 0)

/**
 * General helper to align up a value.
 */
#define _Configure_Align_up(_val, _align) \
  (((_val) + (_align) - 1) - ((_val) + (_align) - 1) % (_align))

#define _CONFIGURE_HEAP_MIN_BLOCK_SIZE \
  _Configure_Align_up(sizeof(Heap_Block), CPU_HEAP_ALIGNMENT)

/**
 * This is a helper macro used in calculations in this file.  It is used
 * to noted when an element is allocated from the RTEMS Workspace and adds
 * a factor to account for heap overhead plus an alignment factor that
 * may be applied.
 */
#define _Configure_From_workspace(_size) \
  (ssize_t) (_Configure_Zero_or_One(_size) * \
    _Configure_Align_up(_size + HEAP_BLOCK_HEADER_SIZE, \
      _CONFIGURE_HEAP_MIN_BLOCK_SIZE))

/**
 * This is a helper macro used in stack space calculations in this file.  It
 * may be provided by the application in case a special task stack allocator
 * is used.  The default is allocation from the RTEMS Workspace.
 */
#ifdef CONFIGURE_TASK_STACK_FROM_ALLOCATOR
  #define _Configure_From_stackspace(_stack_size) \
    CONFIGURE_TASK_STACK_FROM_ALLOCATOR(_stack_size)
#else
  #define _Configure_From_stackspace(_stack_size) \
    _Configure_From_workspace(_stack_size)
#endif

/**
 * Do not use the unlimited bit as part of the multiplication
 * for memory usage.
 */
#define _Configure_Max_Objects(_max) \
  (_Configure_Zero_or_One(_max) * rtems_resource_maximum_per_allocation(_max))
/**@}*/

/**
 * @defgroup ConfigurationInitTasksTable Initialization Tasks Configuration
 *
 * @addtogroup Configuration
 *
 * This group contains the elements needed to define the Classic API
 * Initialization Tasks Table.
 *
 * Default User Initialization Task Table.  This table guarantees that
 * one user initialization table is defined.
 *
 *  WHEN CONFIGURE_HAS_OWN_INIT_TASK_TABLE is defined, the user is
 *  responsible for defining their own table information and setting the
 *  appropriate variables.
 */
#if defined(CONFIGURE_RTEMS_INIT_TASKS_TABLE) && \
    !defined(CONFIGURE_HAS_OWN_INIT_TASK_TABLE)

/**
 * When using the default Classic API Initialization Tasks Table, this is
 * used to specify the name of the single Classic API task.
 */
#ifndef CONFIGURE_INIT_TASK_NAME
  #define CONFIGURE_INIT_TASK_NAME          rtems_build_name('U', 'I', '1', ' ')
#endif

/**
 * When using the default Classic API Initialization Tasks Table, this is
 * used to specify the stack size of the single Classic API task.
 */
#ifndef CONFIGURE_INIT_TASK_STACK_SIZE
  #define CONFIGURE_INIT_TASK_STACK_SIZE    CONFIGURE_MINIMUM_TASK_STACK_SIZE
#endif

/**
 * When using the default Classic API Initialization Tasks Table, this is
 * used to specify the priority of the single Classic API task.
 */
#ifndef CONFIGURE_INIT_TASK_PRIORITY
  #define CONFIGURE_INIT_TASK_PRIORITY      1
#endif

/**
 * When using the default Classic API Initialization Tasks Table, this is
 * used to specify the attributes size of the single Classic API task.
 */
#ifndef CONFIGURE_INIT_TASK_ATTRIBUTES
  #define CONFIGURE_INIT_TASK_ATTRIBUTES    RTEMS_DEFAULT_ATTRIBUTES
#endif

/**
 * When using the default Classic API Initialization Tasks Table, this is
 * used to specify the entry point of the single Classic API task.
 */
#ifndef CONFIGURE_INIT_TASK_ENTRY_POINT
  #ifdef __cplusplus
  extern "C" {
  #endif
    rtems_task Init (rtems_task_argument );
  #ifdef __cplusplus
  }
  #endif
  #define CONFIGURE_INIT_TASK_ENTRY_POINT   Init
  extern const char* bsp_boot_cmdline;
  #define CONFIGURE_INIT_TASK_ARGUMENTS     ((rtems_task_argument) &bsp_boot_cmdline)
#endif

/**
 * When using the default Classic API Initialization Tasks Table, this is
 * used to specify the initial execution mode of the single Classic API task.
 */
#ifndef CONFIGURE_INIT_TASK_INITIAL_MODES
  #ifdef _CONFIGURE_SMP_APPLICATION
    #define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_DEFAULT_MODES
  #else
    #define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_NO_PREEMPT
  #endif
#endif

/**
 * When using the default Classic API Initialization Tasks Table, this is
 * used to specify the initial argument to the single Classic API task.
 */
#ifndef CONFIGURE_INIT_TASK_ARGUMENTS
  #define CONFIGURE_INIT_TASK_ARGUMENTS     0
#endif

#ifdef CONFIGURE_INIT
  rtems_initialization_tasks_table Initialization_tasks[] = {
    { CONFIGURE_INIT_TASK_NAME,
      CONFIGURE_INIT_TASK_STACK_SIZE,
      CONFIGURE_INIT_TASK_PRIORITY,
      CONFIGURE_INIT_TASK_ATTRIBUTES,
      CONFIGURE_INIT_TASK_ENTRY_POINT,
      CONFIGURE_INIT_TASK_INITIAL_MODES,
      CONFIGURE_INIT_TASK_ARGUMENTS
    }
  };
#endif

/**
 * This is the name of the Initialization Tasks Table generated.
 */
#define CONFIGURE_INIT_TASK_TABLE Initialization_tasks

/*
 * This is the size of the Initialization Tasks Table generated.
 */
#define CONFIGURE_INIT_TASK_TABLE_SIZE \
  RTEMS_ARRAY_SIZE(CONFIGURE_INIT_TASK_TABLE)

#else     /* CONFIGURE_RTEMS_INIT_TASKS_TABLE */
#ifdef CONFIGURE_HAS_OWN_INIT_TASK_TABLE

/*
 * The user application is responsible for defining everything
 * when CONFIGURE_HAS_OWN_INIT_TABLE is defined.
 */
#else     /* not using standard or providing own Init Task Table */

/*
 * This is the name of the Initialization Task when none is configured.
 */
#define CONFIGURE_INIT_TASK_TABLE      NULL

/*
 * This is the size of the Initialization Task when none is configured.
 */
#define CONFIGURE_INIT_TASK_TABLE_SIZE 0

/*
 * This is the stack size of the Initialization Task when none is configured.
 */
#define CONFIGURE_INIT_TASK_STACK_SIZE 0

#endif    /* CONFIGURE_HAS_OWN_INIT_TASK_TABLE */

#endif
/**@}*/  /* end of Classic API Initialization Tasks Table */

/**
 * @defgroup ConfigurationDriverTable Device Driver Table Configuration
 *
 * @addtogroup Configuration
 *
 * This group contains parameters related to generating a Device Driver
 * Table.
 *
 * Default Device Driver Table.  Each driver needed by the test is explicitly
 * chosen by the application.  There is always a null driver entry.
 */
/**@{*/

/**
 * This is an empty device driver slot.
 */
#define NULL_DRIVER_TABLE_ENTRY \
 { NULL, NULL, NULL, NULL, NULL, NULL }

#if (defined(CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER) && \
    defined(CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER)) || \
  (defined(CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER) && \
    defined(CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER)) || \
  (defined(CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER) && \
    defined(CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER))
#error "CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER, CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER, and CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER are mutually exclusive"
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
  #include <rtems/console.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
  #include <rtems/console.h>

  #ifdef CONFIGURE_INIT
    RTEMS_SYSINIT_ITEM(
      _Console_simple_Initialize,
      RTEMS_SYSINIT_DEVICE_DRIVERS,
      RTEMS_SYSINIT_ORDER_SECOND
    );
  #endif
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER
  #include <rtems/console.h>

  #ifdef CONFIGURE_INIT
    RTEMS_SYSINIT_ITEM(
      _Console_simple_task_Initialize,
      RTEMS_SYSINIT_DEVICE_DRIVERS,
      RTEMS_SYSINIT_ORDER_SECOND
    );
  #endif
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
  #include <rtems/clockdrv.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER
  #include <rtems/btimer.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER
  #include <rtems/rtc.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_WATCHDOG_DRIVER
  #include <rtems/watchdogdrv.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_FRAME_BUFFER_DRIVER
  #include <rtems/framebuffer.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
  #include <rtems/devnull.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER
  #include <rtems/devzero.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_IDE_DRIVER
  /* the ide driver needs the ATA driver */
  #ifndef CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
    #define CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
  #endif
  #include <libchip/ide_ctrl.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
  #include <libchip/ata.h>
#endif

#ifndef CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE

/**
 * This specifies the maximum number of device drivers that
 * can be installed in the system at one time.  It must account
 * for both the statically and dynamically installed drivers.
 */
#ifndef CONFIGURE_MAXIMUM_DRIVERS
  #define CONFIGURE_MAXIMUM_DRIVERS
#endif

#ifdef CONFIGURE_INIT
  rtems_driver_address_table
    _IO_Driver_address_table[ CONFIGURE_MAXIMUM_DRIVERS ] = {
    #ifdef CONFIGURE_BSP_PREREQUISITE_DRIVERS
      CONFIGURE_BSP_PREREQUISITE_DRIVERS,
    #endif
    #ifdef CONFIGURE_APPLICATION_PREREQUISITE_DRIVERS
      CONFIGURE_APPLICATION_PREREQUISITE_DRIVERS,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
      CONSOLE_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
      CLOCK_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER
      RTC_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_WATCHDOG_DRIVER
      WATCHDOG_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
      DEVNULL_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER
      DEVZERO_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_IDE_DRIVER
      IDE_CONTROLLER_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
      ATA_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_FRAME_BUFFER_DRIVER
      FRAME_BUFFER_DRIVER_TABLE_ENTRY,
    #endif
    #ifdef CONFIGURE_APPLICATION_EXTRA_DRIVERS
      CONFIGURE_APPLICATION_EXTRA_DRIVERS,
    #endif
    #ifdef CONFIGURE_APPLICATION_NEEDS_NULL_DRIVER
      NULL_DRIVER_TABLE_ENTRY
    #elif !defined(CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_NEEDS_IDE_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_NEEDS_FRAME_BUFFER_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_EXTRA_DRIVERS)
      NULL_DRIVER_TABLE_ENTRY
    #endif
  };

  const size_t _IO_Number_of_drivers =
    RTEMS_ARRAY_SIZE( _IO_Driver_address_table );
#endif

#endif  /* CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE */

#ifdef CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
  /*
   * configure the priority of the ATA driver task
   */
  #ifndef CONFIGURE_ATA_DRIVER_TASK_PRIORITY
    #define CONFIGURE_ATA_DRIVER_TASK_PRIORITY ATA_DRIVER_TASK_DEFAULT_PRIORITY
  #endif
  #ifdef CONFIGURE_INIT
    rtems_task_priority rtems_ata_driver_task_priority
      = CONFIGURE_ATA_DRIVER_TASK_PRIORITY;
  #endif /* CONFIGURE_INIT */
#endif
/**@}*/ /* end of Device Driver Table Configuration */

/**
 * @defgroup ConfigurationLibBlock Configuration of LIBBLOCK
 *
 * @addtogroup Configuration
 *
 * This module contains parameters related to the LIBBLOCK buffering
 * and caching subsystem. It requires tasks to swap out data to be
 * written to non-volatile storage.
 */
/**@{*/
#ifdef CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
  #include <rtems/bdbuf.h>
  /*
   * configure the bdbuf cache parameters
   */
  #ifndef CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS
    #define CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS \
                              RTEMS_BDBUF_MAX_READ_AHEAD_BLOCKS_DEFAULT
  #endif
  #ifndef CONFIGURE_BDBUF_MAX_WRITE_BLOCKS
    #define CONFIGURE_BDBUF_MAX_WRITE_BLOCKS \
                              RTEMS_BDBUF_MAX_WRITE_BLOCKS_DEFAULT
  #endif
  #ifndef CONFIGURE_SWAPOUT_TASK_PRIORITY
    #define CONFIGURE_SWAPOUT_TASK_PRIORITY \
                              RTEMS_BDBUF_SWAPOUT_TASK_PRIORITY_DEFAULT
  #endif
  #ifndef CONFIGURE_SWAPOUT_SWAP_PERIOD
    #define CONFIGURE_SWAPOUT_SWAP_PERIOD \
                              RTEMS_BDBUF_SWAPOUT_TASK_SWAP_PERIOD_DEFAULT
  #endif
  #ifndef CONFIGURE_SWAPOUT_BLOCK_HOLD
    #define CONFIGURE_SWAPOUT_BLOCK_HOLD \
                              RTEMS_BDBUF_SWAPOUT_TASK_BLOCK_HOLD_DEFAULT
  #endif
  #ifndef CONFIGURE_SWAPOUT_WORKER_TASKS
    #define CONFIGURE_SWAPOUT_WORKER_TASKS \
                              RTEMS_BDBUF_SWAPOUT_WORKER_TASKS_DEFAULT
  #endif
  #ifndef CONFIGURE_SWAPOUT_WORKER_TASK_PRIORITY
    #define CONFIGURE_SWAPOUT_WORKER_TASK_PRIORITY \
                              RTEMS_BDBUF_SWAPOUT_WORKER_TASK_PRIORITY_DEFAULT
  #endif
  #ifndef CONFIGURE_BDBUF_TASK_STACK_SIZE
    #define CONFIGURE_BDBUF_TASK_STACK_SIZE \
                              RTEMS_BDBUF_TASK_STACK_SIZE_DEFAULT
  #endif
  #ifndef CONFIGURE_BDBUF_CACHE_MEMORY_SIZE
    #define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE \
                              RTEMS_BDBUF_CACHE_MEMORY_SIZE_DEFAULT
  #endif
  #ifndef CONFIGURE_BDBUF_BUFFER_MIN_SIZE
    #define CONFIGURE_BDBUF_BUFFER_MIN_SIZE \
                              RTEMS_BDBUF_BUFFER_MIN_SIZE_DEFAULT
  #endif
  #ifndef CONFIGURE_BDBUF_BUFFER_MAX_SIZE
    #define CONFIGURE_BDBUF_BUFFER_MAX_SIZE \
                              RTEMS_BDBUF_BUFFER_MAX_SIZE_DEFAULT
  #endif
  #ifndef CONFIGURE_BDBUF_READ_AHEAD_TASK_PRIORITY
    #define CONFIGURE_BDBUF_READ_AHEAD_TASK_PRIORITY \
                              RTEMS_BDBUF_READ_AHEAD_TASK_PRIORITY_DEFAULT
  #endif
  #ifdef CONFIGURE_INIT
    const rtems_bdbuf_config rtems_bdbuf_configuration = {
      CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS,
      CONFIGURE_BDBUF_MAX_WRITE_BLOCKS,
      CONFIGURE_SWAPOUT_TASK_PRIORITY,
      CONFIGURE_SWAPOUT_SWAP_PERIOD,
      CONFIGURE_SWAPOUT_BLOCK_HOLD,
      CONFIGURE_SWAPOUT_WORKER_TASKS,
      CONFIGURE_SWAPOUT_WORKER_TASK_PRIORITY,
      CONFIGURE_BDBUF_TASK_STACK_SIZE,
      CONFIGURE_BDBUF_CACHE_MEMORY_SIZE,
      CONFIGURE_BDBUF_BUFFER_MIN_SIZE,
      CONFIGURE_BDBUF_BUFFER_MAX_SIZE,
      CONFIGURE_BDBUF_READ_AHEAD_TASK_PRIORITY
    };
  #endif

  #define _CONFIGURE_LIBBLOCK_TASKS \
    (1 + CONFIGURE_SWAPOUT_WORKER_TASKS + \
    (CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS != 0))

  #define _CONFIGURE_LIBBLOCK_TASK_EXTRA_STACKS \
    (_CONFIGURE_LIBBLOCK_TASKS * \
    (CONFIGURE_BDBUF_TASK_STACK_SIZE <= CONFIGURE_MINIMUM_TASK_STACK_SIZE ? \
    0 : CONFIGURE_BDBUF_TASK_STACK_SIZE - CONFIGURE_MINIMUM_TASK_STACK_SIZE))

  #if defined(CONFIGURE_HAS_OWN_BDBUF_TABLE) || \
      defined(CONFIGURE_BDBUF_BUFFER_SIZE) || \
      defined(CONFIGURE_BDBUF_BUFFER_COUNT)
    #error BDBUF Cache does not use a buffer configuration table. Please remove.
  #endif
#else
  /** This specifies the number of libblock tasks. */
  #define _CONFIGURE_LIBBLOCK_TASKS 0
  /** This specifies the extra stack space configured for libblock tasks. */
  #define _CONFIGURE_LIBBLOCK_TASK_EXTRA_STACKS 0
  /** This specifies the number of Classic API semaphores needed by libblock. */
#endif /* CONFIGURE_APPLICATION_NEEDS_LIBBLOCK */
/**@}*/

/**
 * @defgroup ConfigurationMultiprocessing Multiprocessing Configuration
 *
 * @addtogroup Configuration
 *
 * This module contains the parameters related to the Multiprocessing
 * configuration of RTEMS.
 *
 * In a single processor or SMP configuration, only two parameters are
 * needed and they are defaulted. The user should not have to specify
 * any parameters.
 */
/**@{*/

/**
 * This defines the extra stack space required for the MPCI server thread.
 */
#ifndef CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK
  #define CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK 0
#endif

#if defined(RTEMS_MULTIPROCESSING)
  /*
   *  Default Multiprocessing Configuration Table.  The defaults are
   *  appropriate for most of the RTEMS Multiprocessor Test Suite.  Each
   *  value may be overridden within each test to customize the environment.
   */

  #ifdef CONFIGURE_MP_APPLICATION
    #define _CONFIGURE_TIMER_FOR_SHARED_MEMORY_DRIVER 1

    #ifndef CONFIGURE_HAS_OWN_MULTIPROCESSING_TABLE

      #ifndef CONFIGURE_MP_NODE_NUMBER
        #define CONFIGURE_MP_NODE_NUMBER                NODE_NUMBER
      #endif

      #ifndef CONFIGURE_MP_MAXIMUM_NODES
        #define CONFIGURE_MP_MAXIMUM_NODES              2
      #endif

      #ifndef CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS
        #define CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS     32
      #endif
      #define _CONFIGURE_MEMORY_FOR_GLOBAL_OBJECTS(_global_objects) \
        _Configure_From_workspace( \
          (_global_objects) * sizeof(Objects_MP_Control) \
        )

      #ifndef CONFIGURE_MP_MAXIMUM_PROXIES
        #define CONFIGURE_MP_MAXIMUM_PROXIES            32
      #endif
      #define _CONFIGURE_MEMORY_FOR_PROXIES(_proxies) \
        _Configure_From_workspace((_proxies) \
          * (sizeof(Thread_Proxy_control) \
            + sizeof(Thread_queue_Configured_heads)))

      #ifndef CONFIGURE_MP_MPCI_TABLE_POINTER
        #include <mpci.h>
        #define CONFIGURE_MP_MPCI_TABLE_POINTER         &MPCI_table
      #endif

      #ifdef CONFIGURE_INIT
        rtems_multiprocessing_table Multiprocessing_configuration = {
          CONFIGURE_MP_NODE_NUMBER,               /* local node number */
          CONFIGURE_MP_MAXIMUM_NODES,             /* maximum # nodes */
          CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS,    /* maximum # global objects */
          CONFIGURE_MP_MAXIMUM_PROXIES,           /* maximum # proxies */
          CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK, /* MPCI stack > minimum */
          CONFIGURE_MP_MPCI_TABLE_POINTER         /* ptr to MPCI config table */
        };
      #endif

      #define CONFIGURE_MULTIPROCESSING_TABLE    &Multiprocessing_configuration

      #define _CONFIGURE_MPCI_RECEIVE_SERVER_COUNT 1

    #endif /* CONFIGURE_HAS_OWN_MULTIPROCESSING_TABLE */
  #else
    #define CONFIGURE_MULTIPROCESSING_TABLE NULL
    #define _CONFIGURE_TIMER_FOR_SHARED_MEMORY_DRIVER 0
    #define _CONFIGURE_MPCI_RECEIVE_SERVER_COUNT 0
  #endif /* CONFIGURE_MP_APPLICATION */
#else
  #define _CONFIGURE_TIMER_FOR_SHARED_MEMORY_DRIVER 0
  #define _CONFIGURE_MPCI_RECEIVE_SERVER_COUNT 0
#endif /* RTEMS_MULTIPROCESSING */
/**@}*/ /* end of Multiprocessing Configuration */

/**
 * This macro specifies that the user wants to use unlimited objects for any
 * classic or posix objects that have not already been given resource limits.
 */
#if defined(CONFIGURE_UNLIMITED_OBJECTS)
  #if !defined(CONFIGURE_UNIFIED_WORK_AREAS) && \
     !defined(CONFIGURE_EXECUTIVE_RAM_SIZE) && \
     !defined(CONFIGURE_MEMORY_OVERHEAD)
     #error "CONFIGURE_UNLIMITED_OBJECTS requires a unified work area, an executive RAM size, or a defined workspace memory overhead"
  #endif

  #if !defined(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  /**
   * This macro specifies a default allocation size for when auto-extending
   * unlimited objects if none was given by the user.
   */
    #define CONFIGURE_UNLIMITED_ALLOCATION_SIZE 8
  #endif
  #if !defined(CONFIGURE_MAXIMUM_TASKS)
    #define CONFIGURE_MAXIMUM_TASKS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_TIMERS)
    #define CONFIGURE_MAXIMUM_TIMERS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_SEMAPHORES)
    #define CONFIGURE_MAXIMUM_SEMAPHORES \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_MESSAGE_QUEUES)
    #define CONFIGURE_MAXIMUM_MESSAGE_QUEUES \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_PARTITIONS)
    #define CONFIGURE_MAXIMUM_PARTITIONS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_REGIONS)
    #define CONFIGURE_MAXIMUM_REGIONS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_PORTS)
    #define CONFIGURE_MAXIMUM_PORTS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_PERIODS)
    #define CONFIGURE_MAXIMUM_PERIODS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_BARRIERS)
    #define CONFIGURE_MAXIMUM_BARRIERS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_POSIX_KEYS)
    #define CONFIGURE_MAXIMUM_POSIX_KEYS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS)
    #define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES)
    #define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_POSIX_SEMAPHORES)
    #define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_POSIX_SHMS)
    #define CONFIGURE_MAXIMUM_POSIX_SHMS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif
  #if !defined(CONFIGURE_MAXIMUM_POSIX_THREADS)
    #define CONFIGURE_MAXIMUM_POSIX_THREADS \
      rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
  #endif

  #ifdef RTEMS_POSIX_API
    #if !defined(CONFIGURE_MAXIMUM_POSIX_TIMERS)
      #define CONFIGURE_MAXIMUM_POSIX_TIMERS \
        rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
    #endif
  #endif /* RTEMS_POSIX_API */
#endif /* CONFIGURE_UNLIMITED_OBJECTS */


/**
 * @defgroup ConfigurationClassicAPI Classic API Configuration
 *
 * @ingroup Configuration
 *
 * This module contains the parameters related to configuration
 * of the Classic API services.
 */
/**@{*/

/** This configures the maximum number of Classic API tasks. */
#ifndef CONFIGURE_MAXIMUM_TASKS
  #define CONFIGURE_MAXIMUM_TASKS               0
#endif

/*
 * This is calculated to account for the maximum number of Classic API
 * tasks used by the application and configured RTEMS capabilities.
 */
#define _CONFIGURE_TASKS \
  (CONFIGURE_MAXIMUM_TASKS + _CONFIGURE_LIBBLOCK_TASKS)

#ifndef CONFIGURE_MAXIMUM_TIMERS
  /** This specifies the maximum number of Classic API timers. */
  #define CONFIGURE_MAXIMUM_TIMERS             0
#endif

#define _CONFIGURE_TIMERS \
  (CONFIGURE_MAXIMUM_TIMERS + _CONFIGURE_TIMER_FOR_SHARED_MEMORY_DRIVER)

#ifndef CONFIGURE_MAXIMUM_SEMAPHORES
  /** This specifies the maximum number of Classic API semaphores. */
  #define CONFIGURE_MAXIMUM_SEMAPHORES                 0
#endif

/*
 * This macro is calculated to specify the memory required for
 * Classic API Semaphores using MRSP. This is only available in
 * SMP configurations.
 */
#if !defined(RTEMS_SMP) || \
  !defined(CONFIGURE_MAXIMUM_MRSP_SEMAPHORES)
  #define _CONFIGURE_MEMORY_FOR_MRSP_SEMAPHORES 0
#else
  #define _CONFIGURE_MEMORY_FOR_MRSP_SEMAPHORES \
    CONFIGURE_MAXIMUM_MRSP_SEMAPHORES * \
      _Configure_From_workspace( \
        RTEMS_ARRAY_SIZE(_Scheduler_Table) * sizeof(Priority_Control) \
      )
#endif

#ifndef CONFIGURE_MAXIMUM_MESSAGE_QUEUES
  /**
   * This configuration parameter specifies the maximum number of
   * Classic API Message Queues.
   */
  #define CONFIGURE_MAXIMUM_MESSAGE_QUEUES             0
#endif

#ifndef CONFIGURE_MAXIMUM_PARTITIONS
  /**
   * This configuration parameter specifies the maximum number of
   * Classic API Partitions.
   */
  #define CONFIGURE_MAXIMUM_PARTITIONS                 0
#endif

#ifndef CONFIGURE_MAXIMUM_REGIONS
  /**
   * This configuration parameter specifies the maximum number of
   * Classic API Regions.
   */
  #define CONFIGURE_MAXIMUM_REGIONS              0
#endif

#ifndef CONFIGURE_MAXIMUM_PORTS
  /**
   * This configuration parameter specifies the maximum number of
   * Classic API Dual-Ported Memory Ports.
   */
  #define CONFIGURE_MAXIMUM_PORTS            0
#endif

#ifndef CONFIGURE_MAXIMUM_PERIODS
  /**
   * This configuration parameter specifies the maximum number of
   * Classic API Rate Monotonic Periods.
   */
  #define CONFIGURE_MAXIMUM_PERIODS              0
#endif

/**
 * This configuration parameter specifies the maximum number of
 * Classic API Barriers.
 */
#ifndef CONFIGURE_MAXIMUM_BARRIERS
  #define CONFIGURE_MAXIMUM_BARRIERS               0
#endif

/*
 * This macro is calculated to specify the number of Classic API
 * Barriers required by the application and configured capabilities.
 */
#define _CONFIGURE_BARRIERS \
  (CONFIGURE_MAXIMUM_BARRIERS + _CONFIGURE_BARRIERS_FOR_FIFOS)

#ifndef CONFIGURE_MAXIMUM_USER_EXTENSIONS
  /**
   * This configuration parameter specifies the maximum number of
   * Classic API User Extensions.
   */
  #define CONFIGURE_MAXIMUM_USER_EXTENSIONS                 0
#endif

/**@}*/ /* end of Classic API Configuration */

/**
 * @defgroup ConfigurationGeneral General System Configuration
 *
 * @ingroup Configuration
 *
 * This module contains configuration parameters that are independent
 * of any API but impact general system configuration.
 */
/**@{*/

/** The configures the number of microseconds per clock tick. */
#ifndef CONFIGURE_MICROSECONDS_PER_TICK
  #define CONFIGURE_MICROSECONDS_PER_TICK \
          RTEMS_MILLISECONDS_TO_MICROSECONDS(10)
#endif

#if 1000000 % CONFIGURE_MICROSECONDS_PER_TICK != 0
  #warning "The clock ticks per second is not an integer"
#endif

#if CONFIGURE_MICROSECONDS_PER_TICK <= 0
  #error "The CONFIGURE_MICROSECONDS_PER_TICK must be positive"
#endif

#define _CONFIGURE_TICKS_PER_SECOND (1000000 / CONFIGURE_MICROSECONDS_PER_TICK)

/** The configures the number of clock ticks per timeslice. */
#ifndef CONFIGURE_TICKS_PER_TIMESLICE
  #define CONFIGURE_TICKS_PER_TIMESLICE        50
#endif

/**@}*/ /* end of General Configuration */

/*
 *  Initial Extension Set
 */

#ifdef CONFIGURE_INIT
#ifdef CONFIGURE_STACK_CHECKER_ENABLED
#include <rtems/stackchk.h>
#endif
#include <rtems/libcsupport.h>

#if defined(BSP_INITIAL_EXTENSION) || \
    defined(CONFIGURE_INITIAL_EXTENSIONS) || \
    defined(CONFIGURE_STACK_CHECKER_ENABLED) || \
    (defined(RTEMS_NEWLIB) && !defined(CONFIGURE_DISABLE_NEWLIB_REENTRANCY))
  static const rtems_extensions_table Configuration_Initial_Extensions[] = {
    #if CONFIGURE_RECORD_PER_PROCESSOR_ITEMS > 0 && \
      defined(CONFIGURE_RECORD_EXTENSIONS_ENABLED)
      RECORD_EXTENSION,
    #endif
    #if !defined(CONFIGURE_DISABLE_NEWLIB_REENTRANCY)
      RTEMS_NEWLIB_EXTENSION,
    #endif
    #if defined(CONFIGURE_STACK_CHECKER_ENABLED)
      RTEMS_STACK_CHECKER_EXTENSION,
    #endif
    #if defined(CONFIGURE_INITIAL_EXTENSIONS)
      CONFIGURE_INITIAL_EXTENSIONS,
    #endif
    #if defined(BSP_INITIAL_EXTENSION)
      BSP_INITIAL_EXTENSION
    #endif
  };

  #define _CONFIGURE_INITIAL_EXTENSION_TABLE Configuration_Initial_Extensions
  #define _CONFIGURE_NUMBER_OF_INITIAL_EXTENSIONS \
    RTEMS_ARRAY_SIZE(Configuration_Initial_Extensions)

  RTEMS_SYSINIT_ITEM(
    _User_extensions_Handler_initialization,
    RTEMS_SYSINIT_INITIAL_EXTENSIONS,
    RTEMS_SYSINIT_ORDER_MIDDLE
  );
#else
  #define _CONFIGURE_INITIAL_EXTENSION_TABLE NULL
  #define _CONFIGURE_NUMBER_OF_INITIAL_EXTENSIONS 0
#endif

#if defined(RTEMS_NEWLIB) && !defined(CONFIGURE_DISABLE_NEWLIB_REENTRANCY)
struct _reent *__getreent(void)
{
  return _Thread_Get_executing()->libc_reent;
}
#endif

#endif

/**
 * @defgroup ConfigurationPOSIXAPI POSIX API Configuration Parameters
 *
 * This module contains the parameters related to configuration
 * of the POSIX API services.
 */
/**@{*/

/**
 * This configuration parameter specifies the maximum number of
 * POSIX API keys.
 *
 * POSIX Keys are available whether or not the POSIX API is enabled.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_KEYS
  #define CONFIGURE_MAXIMUM_POSIX_KEYS 0
#endif

/*
 * This macro is calculated to specify the memory required for
 * POSIX API key/value pairs.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS
  #define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS \
    (CONFIGURE_MAXIMUM_POSIX_KEYS * \
     (CONFIGURE_MAXIMUM_POSIX_THREADS + CONFIGURE_MAXIMUM_TASKS))
#endif

/*
 * This macro is calculated to specify the total number of
 * POSIX API keys required by the application and configured
 * system capabilities.
 */
#define _CONFIGURE_POSIX_KEYS \
  (CONFIGURE_MAXIMUM_POSIX_KEYS + _CONFIGURE_LIBIO_POSIX_KEYS)

/**
 * This configuration parameter specifies the maximum number of
 * POSIX API threads.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_THREADS
  #define CONFIGURE_MAXIMUM_POSIX_THREADS 0
#endif

/*
 * Account for the object control structures plus the name
 * of the object to be duplicated.
 */
#define _Configure_POSIX_Named_Object_RAM(_number, _size) \
  (_Configure_Max_Objects(_number) \
    * _Configure_From_workspace(_POSIX_PATH_MAX + 1))

/**
 * This configuration parameter specifies the maximum number of
 * POSIX API message queues.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES
  #define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES 0
#endif

/*
 * This macro is calculated to specify the memory required for
 * POSIX API message queues.
 */
#define _CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUES(_message_queues) \
  _Configure_POSIX_Named_Object_RAM( \
     _message_queues, sizeof(POSIX_Message_queue_Control) )

/**
 * This configuration parameter specifies the maximum number of
 * POSIX API semaphores.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_SEMAPHORES
  #define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES 0
#endif

/*
 * This macro is calculated to specify the memory required for
 * POSIX API semaphores.
 */
#define _CONFIGURE_MEMORY_FOR_POSIX_SEMAPHORES(_semaphores) \
  _Configure_POSIX_Named_Object_RAM( \
     _semaphores, sizeof(POSIX_Semaphore_Control) )

/**
 * Configure the maximum number of POSIX shared memory objects.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_SHMS
  #define CONFIGURE_MAXIMUM_POSIX_SHMS 0
#endif

/*
 * This macro is calculated to specify the memory required for
 * POSIX API shared memory.
 */
#define _CONFIGURE_MEMORY_FOR_POSIX_SHMS(_shms) \
  _Configure_POSIX_Named_Object_RAM(_shms, sizeof(POSIX_Shm_Control) )

/**
 * This configuration parameter specifies the maximum number of
 * POSIX API timers.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_TIMERS
  #define CONFIGURE_MAXIMUM_POSIX_TIMERS 0
#endif

#if !defined(RTEMS_POSIX_API) && CONFIGURE_MAXIMUM_POSIX_TIMERS != 0
  #error "CONFIGURE_MAXIMUM_POSIX_TIMERS must be zero if POSIX API is disabled"
#endif

/**
 * This configuration parameter specifies the maximum number of
 * POSIX API queued signals.
 */
#ifndef CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS
  #define CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS 0
#endif

#if !defined(RTEMS_POSIX_API) && CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS != 0
  #error "CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS must be zero if POSIX API is disabled"
#endif

#if CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS > 0
  #define _CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS \
    _Configure_From_workspace( (CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS) * \
      sizeof( POSIX_signals_Siginfo_node ) )
#else
  #define _CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS 0
#endif

#ifdef CONFIGURE_POSIX_INIT_THREAD_TABLE
  #ifndef CONFIGURE_POSIX_HAS_OWN_INIT_THREAD_TABLE
    #ifndef CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT
      #define CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT   POSIX_Init
    #endif

    #ifndef CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE
      #define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE \
        CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE
    #endif

    #ifdef CONFIGURE_INIT
      posix_initialization_threads_table POSIX_Initialization_threads[] = {
        { CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT,
          CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE }
      };
    #endif

    #define CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME \
      POSIX_Initialization_threads

    #define CONFIGURE_POSIX_INIT_THREAD_TABLE_SIZE \
      RTEMS_ARRAY_SIZE(CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME)
  #endif /* !CONFIGURE_POSIX_HAS_OWN_INIT_TASK_TABLE */
#else /* !CONFIGURE_POSIX_INIT_THREAD_TABLE */
  #define CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME NULL
  #define CONFIGURE_POSIX_INIT_THREAD_TABLE_SIZE 0
#endif /* CONFIGURE_POSIX_INIT_THREAD_TABLE */

/**
 * This configuration parameter specifies the stack size of the
 * POSIX API Initialization thread (if used).
 */
#ifndef CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE
  #define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE    0
#endif
/**@}*/  /* end of POSIX API Configuration */

/**
 * @defgroup ConfigurationGNAT GNAT/RTEMS Configuration
 *
 * @addtogroup Configuration
 *
 *  This modules includes configuration parameters for applications which
 *  use GNAT/RTEMS. GNAT implements each Ada task as a POSIX thread.
 */
/**@{*/
#ifdef CONFIGURE_GNAT_RTEMS
  /**
   * This is the maximum number of Ada tasks which can be concurrently
   * in existence.  Twenty (20) are required to run all tests in the
   * ACATS (formerly ACVC).
   */
  #ifndef CONFIGURE_MAXIMUM_ADA_TASKS
    #define CONFIGURE_MAXIMUM_ADA_TASKS  20
  #endif

  /**
   * This is the number of non-Ada tasks which invoked Ada code.
   */
  #ifndef CONFIGURE_MAXIMUM_FAKE_ADA_TASKS
    #define CONFIGURE_MAXIMUM_FAKE_ADA_TASKS 0
  #endif
#else
  /** This defines he number of POSIX mutexes GNAT needs. */
  /** This defines he number of Ada tasks needed by the application. */
  #define CONFIGURE_MAXIMUM_ADA_TASKS      0
  /**
   * This defines he number of non-Ada tasks/threads that will invoke
   * Ada subprograms or functions.
   */
  #define CONFIGURE_MAXIMUM_FAKE_ADA_TASKS 0
#endif
/**@}*/  /* end of GNAT Configuration */

/**
 * This is so we can account for tasks with stacks greater than minimum
 * size.  This is in bytes.
 */
#ifndef CONFIGURE_EXTRA_TASK_STACKS
  #define CONFIGURE_EXTRA_TASK_STACKS 0
#endif

/**
 * This macro provides a summation of the various POSIX thread requirements.
 */
#define _CONFIGURE_POSIX_THREADS \
   (CONFIGURE_MAXIMUM_POSIX_THREADS + \
     CONFIGURE_MAXIMUM_ADA_TASKS)

/*
 * We must be able to split the free block used for the second last allocation
 * into two parts so that we have a free block for the last allocation.  See
 * _Heap_Block_split().
 */
#define _CONFIGURE_HEAP_HANDLER_OVERHEAD \
  _Configure_Align_up( HEAP_BLOCK_HEADER_SIZE, CPU_HEAP_ALIGNMENT )

/**
 * This calculates the amount of memory reserved for the IDLE tasks.
 * In an SMP system, each CPU core has its own idle task.
 */
#define _CONFIGURE_IDLE_TASKS_COUNT _CONFIGURE_MAXIMUM_PROCESSORS

/*
 *  Calculate the RAM size based on the maximum number of objects configured.
 */
#ifndef CONFIGURE_EXECUTIVE_RAM_SIZE

/*
 * Account for allocating the following per object
 *   + array of object control structures
 *   + local pointer table -- pointer per object plus a zero'th
 *     entry in the local pointer table.
 */
#define _CONFIGURE_MEMORY_FOR_TASKS(_tasks, _number_FP_tasks) \
  (_Configure_Max_Objects(_number_FP_tasks) \
    * _Configure_From_workspace(CONTEXT_FP_SIZE))

/*
 * This defines the amount of memory configured for the multiprocessing
 * support required by this application.
 */
#ifdef CONFIGURE_MP_APPLICATION
  #define _CONFIGURE_MEMORY_FOR_MP \
    (_CONFIGURE_MEMORY_FOR_PROXIES(CONFIGURE_MP_MAXIMUM_PROXIES) + \
     _CONFIGURE_MEMORY_FOR_GLOBAL_OBJECTS(CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS))
#else
  #define _CONFIGURE_MEMORY_FOR_MP  0
#endif

/**
 * The following macro is used to calculate the memory allocated by RTEMS
 * for the message buffers associated with a particular message queue.
 * There is a fixed amount of overhead per message.
 */
#define CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(_messages, _size) \
    _Configure_From_workspace( \
      (_messages) * (_Configure_Align_up(_size, sizeof(uintptr_t)) \
        + sizeof(CORE_message_queue_Buffer_control)))

/*
 * This macro is set to the amount of memory required for pending message
 * buffers in bytes.  It should be constructed by adding together a
 * set of values determined by CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE.
 */
#ifndef CONFIGURE_MESSAGE_BUFFER_MEMORY
  #define CONFIGURE_MESSAGE_BUFFER_MEMORY 0
#endif

/**
 * This macro is available just in case the confdefs.h file underallocates
 * memory for a particular application.  This lets the user add some extra
 * memory in case something broken and underestimates.
 *
 * It is also possible for cases where confdefs.h overallocates memory,
 * you could substract memory from the allocated.  The estimate is just
 * that, an estimate, and assumes worst case alignment and padding on
 * each allocated element.  So in some cases it could be too conservative.
 *
 * NOTE: Historically this was used for message buffers.
 */
#ifndef CONFIGURE_MEMORY_OVERHEAD
  #define CONFIGURE_MEMORY_OVERHEAD 0
#endif

/**
 * This defines the formula used to compute the amount of memory
 * reserved for internal task control structures.
 */
#if CPU_IDLE_TASK_IS_FP == TRUE
  #define _CONFIGURE_MEMORY_FOR_INTERNAL_TASKS \
    _CONFIGURE_MEMORY_FOR_TASKS( \
      _CONFIGURE_IDLE_TASKS_COUNT + _CONFIGURE_MPCI_RECEIVE_SERVER_COUNT, \
      _CONFIGURE_IDLE_TASKS_COUNT + _CONFIGURE_MPCI_RECEIVE_SERVER_COUNT \
    )
#else
  #define _CONFIGURE_MEMORY_FOR_INTERNAL_TASKS \
    _CONFIGURE_MEMORY_FOR_TASKS( \
      _CONFIGURE_IDLE_TASKS_COUNT + _CONFIGURE_MPCI_RECEIVE_SERVER_COUNT, \
      _CONFIGURE_MPCI_RECEIVE_SERVER_COUNT \
    )
#endif

/**
 * This macro accounts for general RTEMS system overhead.
 */
#define _CONFIGURE_MEMORY_FOR_SYSTEM_OVERHEAD \
  _CONFIGURE_MEMORY_FOR_INTERNAL_TASKS

/**
 * This macro reserves the memory required by the statically configured
 * user extensions.
 */
#define _CONFIGURE_MEMORY_FOR_STATIC_EXTENSIONS \
  (_CONFIGURE_NUMBER_OF_INITIAL_EXTENSIONS == 0 ? 0 : \
    _Configure_From_workspace( \
      _CONFIGURE_NUMBER_OF_INITIAL_EXTENSIONS \
        * sizeof(User_extensions_Switch_control) \
    ))

/**
 * This calculates the memory required for the executive workspace.
 *
 * This is an internal parameter.
 */
#define CONFIGURE_EXECUTIVE_RAM_SIZE \
( \
   _CONFIGURE_MEMORY_FOR_SYSTEM_OVERHEAD + \
   _CONFIGURE_MEMORY_FOR_TASKS( \
     _CONFIGURE_TASKS, _CONFIGURE_TASKS) + \
   _CONFIGURE_MEMORY_FOR_TASKS( \
     _CONFIGURE_POSIX_THREADS, _CONFIGURE_POSIX_THREADS) + \
   _CONFIGURE_MEMORY_FOR_MRSP_SEMAPHORES + \
   _CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUES( \
     CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES) + \
   _CONFIGURE_MEMORY_FOR_POSIX_SEMAPHORES( \
     CONFIGURE_MAXIMUM_POSIX_SEMAPHORES) + \
   _CONFIGURE_MEMORY_FOR_POSIX_SHMS( \
     CONFIGURE_MAXIMUM_POSIX_SHMS) + \
   _CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS + \
   _CONFIGURE_MEMORY_FOR_STATIC_EXTENSIONS + \
   _CONFIGURE_MEMORY_FOR_MP + \
   CONFIGURE_MESSAGE_BUFFER_MEMORY + \
   (CONFIGURE_MEMORY_OVERHEAD * 1024) + \
   _CONFIGURE_HEAP_HANDLER_OVERHEAD \
)

/*
 *  Now account for any extra memory that initialization tasks or threads
 *  may have requested.
 */

/*
 * This accounts for any extra memory required by the Classic API
 * Initialization Task.
 */
#if (CONFIGURE_INIT_TASK_STACK_SIZE > CONFIGURE_MINIMUM_TASK_STACK_SIZE)
  #define _CONFIGURE_INITIALIZATION_THREADS_STACKS_CLASSIC_PART \
      (CONFIGURE_INIT_TASK_STACK_SIZE - CONFIGURE_MINIMUM_TASK_STACK_SIZE)
#else
  #define _CONFIGURE_INITIALIZATION_THREADS_STACKS_CLASSIC_PART 0
#endif

/*
 * This accounts for any extra memory required by the POSIX API
 * Initialization Thread.
 */
#if (CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE > \
      CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE)
  #define _CONFIGURE_INITIALIZATION_THREADS_STACKS_POSIX_PART \
    (CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE - \
      CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE)
#else
  #define _CONFIGURE_INITIALIZATION_THREADS_STACKS_POSIX_PART 0
#endif

/*
 * This macro provides a summation of the various initialization task
 * and thread stack requirements.
 */
#define _CONFIGURE_INITIALIZATION_THREADS_EXTRA_STACKS \
    (_CONFIGURE_INITIALIZATION_THREADS_STACKS_CLASSIC_PART + \
    _CONFIGURE_INITIALIZATION_THREADS_STACKS_POSIX_PART)

/*
 * This macro is calculated to specify the memory required for
 * the Idle tasks(s) stack.
 */
#define _CONFIGURE_IDLE_TASKS_STACK \
  (_CONFIGURE_IDLE_TASKS_COUNT * \
    _Configure_From_stackspace( CONFIGURE_IDLE_TASK_STACK_SIZE ) )

/*
 * This macro is calculated to specify the stack memory required for the MPCI
 * task.
 */
#define _CONFIGURE_MPCI_RECEIVE_SERVER_STACK \
  (_CONFIGURE_MPCI_RECEIVE_SERVER_COUNT * \
    _Configure_From_stackspace(CONFIGURE_MINIMUM_TASK_STACK_SIZE))

/*
 * This macro is calculated to specify the memory required for
 * the stacks of all tasks.
 */
#define _CONFIGURE_TASKS_STACK \
  (_Configure_Max_Objects( _CONFIGURE_TASKS ) * \
    _Configure_From_stackspace( CONFIGURE_MINIMUM_TASK_STACK_SIZE ) )

/*
 * This macro is calculated to specify the memory required for
 * the stacks of all POSIX threads.
 */
#define _CONFIGURE_POSIX_THREADS_STACK \
  (_Configure_Max_Objects( CONFIGURE_MAXIMUM_POSIX_THREADS ) * \
    _Configure_From_stackspace( CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE ) )

/*
 * This macro is calculated to specify the memory required for
 * the stacks of all Ada tasks.
 */
#define _CONFIGURE_ADA_TASKS_STACK \
  (_Configure_Max_Objects( CONFIGURE_MAXIMUM_ADA_TASKS ) * \
    _Configure_From_stackspace( CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE ) )

#else /* CONFIGURE_EXECUTIVE_RAM_SIZE */

#define _CONFIGURE_IDLE_TASKS_STACK 0
#define _CONFIGURE_MPCI_RECEIVE_SERVER_STACK 0
#define _CONFIGURE_INITIALIZATION_THREADS_EXTRA_STACKS 0
#define _CONFIGURE_TASKS_STACK 0
#define _CONFIGURE_POSIX_THREADS_STACK 0
#define _CONFIGURE_ADA_TASKS_STACK 0

#if CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK != 0
  #error "CONFIGURE_EXECUTIVE_RAM_SIZE defined with request for CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK"
#endif

#if CONFIGURE_EXTRA_TASK_STACKS != 0
  #error "CONFIGURE_EXECUTIVE_RAM_SIZE defined with request for CONFIGURE_EXTRA_TASK_STACKS"
#endif

#endif /* CONFIGURE_EXECUTIVE_RAM_SIZE */

/*
 * This macro is calculated to specify the memory required for
 * all tasks and threads of all varieties.
 */
#define _CONFIGURE_STACK_SPACE_SIZE \
  ( \
    _CONFIGURE_IDLE_TASKS_STACK + \
    _CONFIGURE_MPCI_RECEIVE_SERVER_STACK + \
    _CONFIGURE_INITIALIZATION_THREADS_EXTRA_STACKS + \
    _CONFIGURE_TASKS_STACK + \
    _CONFIGURE_POSIX_THREADS_STACK + \
    _CONFIGURE_ADA_TASKS_STACK + \
    CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK + \
    _CONFIGURE_LIBBLOCK_TASK_EXTRA_STACKS + \
    CONFIGURE_EXTRA_TASK_STACKS + \
    _CONFIGURE_HEAP_HANDLER_OVERHEAD \
  )

#ifndef CONFIGURE_MAXIMUM_THREAD_NAME_SIZE
  #define CONFIGURE_MAXIMUM_THREAD_NAME_SIZE THREAD_DEFAULT_MAXIMUM_NAME_SIZE
#endif

#ifdef CONFIGURE_INIT
  typedef union {
    Scheduler_Node Base;
    #ifdef CONFIGURE_SCHEDULER_CBS
      Scheduler_CBS_Node CBS;
    #endif
    #ifdef CONFIGURE_SCHEDULER_EDF
      Scheduler_EDF_Node EDF;
    #endif
    #ifdef CONFIGURE_SCHEDULER_EDF_SMP
      Scheduler_EDF_SMP_Node EDF_SMP;
    #endif
    #ifdef CONFIGURE_SCHEDULER_PRIORITY
      Scheduler_priority_Node Priority;
    #endif
    #ifdef CONFIGURE_SCHEDULER_SIMPLE_SMP
      Scheduler_SMP_Node Simple_SMP;
    #endif
    #ifdef CONFIGURE_SCHEDULER_PRIORITY_SMP
      Scheduler_priority_SMP_Node Priority_SMP;
    #endif
    #ifdef CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP
      Scheduler_priority_affinity_SMP_Node Priority_affinity_SMP;
    #endif
    #ifdef CONFIGURE_SCHEDULER_STRONG_APA
      Scheduler_strong_APA_Node Strong_APA;
    #endif
    #ifdef CONFIGURE_SCHEDULER_USER_PER_THREAD
      CONFIGURE_SCHEDULER_USER_PER_THREAD User;
    #endif
  } Configuration_Scheduler_node;

  #ifdef RTEMS_SMP
    const size_t _Scheduler_Node_size = sizeof( Configuration_Scheduler_node );
  #endif

  const size_t _Thread_Maximum_name_size = CONFIGURE_MAXIMUM_THREAD_NAME_SIZE;

  struct Thread_Configured_control {
    Thread_Control Control;
    #if CONFIGURE_MAXIMUM_USER_EXTENSIONS > 0
      void *extensions[ CONFIGURE_MAXIMUM_USER_EXTENSIONS + 1 ];
    #endif
    Configuration_Scheduler_node Scheduler_nodes[ _CONFIGURE_SCHEDULER_COUNT ];
    RTEMS_API_Control API_RTEMS;
    #ifdef RTEMS_POSIX_API
      POSIX_API_Control API_POSIX;
    #endif
    #if CONFIGURE_MAXIMUM_THREAD_NAME_SIZE > 1
      char name[ CONFIGURE_MAXIMUM_THREAD_NAME_SIZE ];
    #endif
    #if !defined(RTEMS_SCHEDSIM) \
      && defined(RTEMS_NEWLIB) \
      && !defined(CONFIGURE_DISABLE_NEWLIB_REENTRANCY)
      struct _reent Newlib;
    #else
      struct { /* Empty */ } Newlib;
    #endif
  };

  const Thread_Control_add_on _Thread_Control_add_ons[] = {
    {
      offsetof( Thread_Configured_control, Control.Scheduler.nodes ),
      offsetof( Thread_Configured_control, Scheduler_nodes )
    }, {
      offsetof(
        Thread_Configured_control,
        Control.API_Extensions[ THREAD_API_RTEMS ]
      ),
      offsetof( Thread_Configured_control, API_RTEMS )
    }, {
      offsetof(
        Thread_Configured_control,
        Control.libc_reent
      ),
      offsetof( Thread_Configured_control, Newlib )
    }
    #if CONFIGURE_MAXIMUM_THREAD_NAME_SIZE > 1
      , {
        offsetof(
          Thread_Configured_control,
          Control.Join_queue.Queue.name
        ),
        offsetof( Thread_Configured_control, name )
      }
    #endif
    #ifdef RTEMS_POSIX_API
      , {
        offsetof(
          Thread_Configured_control,
          Control.API_Extensions[ THREAD_API_POSIX ]
        ),
        offsetof( Thread_Configured_control, API_POSIX )
      }
    #endif
  };

  const size_t _Thread_Control_add_on_count =
    RTEMS_ARRAY_SIZE( _Thread_Control_add_ons );

  #if defined(RTEMS_SMP)
    struct Thread_queue_Configured_heads {
      Thread_queue_Heads Heads;
        Thread_queue_Priority_queue Priority[ _CONFIGURE_SCHEDULER_COUNT ];
    };

    const size_t _Thread_queue_Heads_size =
      sizeof( Thread_queue_Configured_heads );
  #endif

  const uint32_t _Watchdog_Nanoseconds_per_tick =
    (uint32_t) 1000 * CONFIGURE_MICROSECONDS_PER_TICK;

  const uint32_t _Watchdog_Ticks_per_second = _CONFIGURE_TICKS_PER_SECOND;

  const size_t _Thread_Initial_thread_count = _CONFIGURE_IDLE_TASKS_COUNT +
    _CONFIGURE_MPCI_RECEIVE_SERVER_COUNT +
    rtems_resource_maximum_per_allocation( _CONFIGURE_TASKS ) +
    rtems_resource_maximum_per_allocation( _CONFIGURE_POSIX_THREADS );

  THREAD_INFORMATION_DEFINE(
    _Thread,
    OBJECTS_INTERNAL_API,
    OBJECTS_INTERNAL_THREADS,
    _CONFIGURE_IDLE_TASKS_COUNT + _CONFIGURE_MPCI_RECEIVE_SERVER_COUNT
  );

  #if _CONFIGURE_BARRIERS > 0
    BARRIER_INFORMATION_DEFINE( _CONFIGURE_BARRIERS );
  #endif

  #if CONFIGURE_MAXIMUM_MESSAGE_QUEUES > 0
    MESSAGE_QUEUE_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_MESSAGE_QUEUES );
  #endif

  #if CONFIGURE_MAXIMUM_PARTITIONS > 0
    PARTITION_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_PARTITIONS );
  #endif

  #if CONFIGURE_MAXIMUM_PERIODS > 0
    RATE_MONOTONIC_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_PERIODS );
  #endif

  #if CONFIGURE_MAXIMUM_PORTS > 0
    DUAL_PORTED_MEMORY_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_PORTS );
  #endif

  #if CONFIGURE_MAXIMUM_REGIONS > 0
    REGION_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_REGIONS );
  #endif

  #if CONFIGURE_MAXIMUM_SEMAPHORES > 0
    SEMAPHORE_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_SEMAPHORES );
  #endif

  #if _CONFIGURE_TIMERS > 0
    TIMER_INFORMATION_DEFINE( _CONFIGURE_TIMERS );
  #endif

  #if _CONFIGURE_TASKS > 0
    THREAD_INFORMATION_DEFINE(
      _RTEMS_tasks,
      OBJECTS_CLASSIC_API,
      OBJECTS_RTEMS_TASKS,
      _CONFIGURE_TASKS
    );
  #endif

  #if CONFIGURE_MAXIMUM_USER_EXTENSIONS > 0
    EXTENSION_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_USER_EXTENSIONS );
  #endif

  /**
   * This is the Classic API Configuration Table.
   */
  rtems_api_configuration_table Configuration_RTEMS_API = {
    CONFIGURE_INIT_TASK_TABLE_SIZE,
    CONFIGURE_INIT_TASK_TABLE
  };

  #if CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS > 0
    POSIX_Keys_Key_value_pair _POSIX_Keys_Key_value_pairs[
      rtems_resource_maximum_per_allocation(
        CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS
      )
    ];

    const uint32_t _POSIX_Keys_Key_value_pair_maximum =
      CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS;
  #endif

  #if _CONFIGURE_POSIX_KEYS > 0
    POSIX_KEYS_INFORMATION_DEFINE( _CONFIGURE_POSIX_KEYS );
  #endif

  #if CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES > 0
    POSIX_MESSAGE_QUEUE_INFORMATION_DEFINE(
      CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES
    );
  #endif

  #if CONFIGURE_MAXIMUM_POSIX_SEMAPHORES > 0
    POSIX_SEMAPHORE_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_POSIX_SEMAPHORES );
  #endif

  #if CONFIGURE_MAXIMUM_POSIX_SHMS > 0
    POSIX_SHM_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_POSIX_SHMS );
  #endif

  #if _CONFIGURE_POSIX_THREADS > 0
    THREAD_INFORMATION_DEFINE(
      _POSIX_Threads,
      OBJECTS_POSIX_API,
      OBJECTS_POSIX_THREADS,
      CONFIGURE_MAXIMUM_POSIX_THREADS
    );
  #endif

  #ifdef RTEMS_POSIX_API
    #if CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS > 0
      const uint32_t _POSIX_signals_Maximum_queued_signals =
        CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS;

      POSIX_signals_Siginfo_node _POSIX_signals_Siginfo_nodes[
        CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS
      ];
    #endif

    #if CONFIGURE_MAXIMUM_POSIX_TIMERS > 0
      POSIX_TIMER_INFORMATION_DEFINE( CONFIGURE_MAXIMUM_POSIX_TIMERS );
    #endif
  #endif

  const size_t _POSIX_Threads_Minimum_stack_size =
    CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE;

  /**
   * This variable specifies the minimum stack size for tasks in an RTEMS
   * application.
   *
   * NOTE: This is left as a simple uint32_t so it can be externed as
   *       needed without requring being high enough logical to
   *       include the full configuration table.
   */
  uint32_t rtems_minimum_stack_size =
    CONFIGURE_MINIMUM_TASK_STACK_SIZE;

  /**
   * This is the primary Configuration Table for this application.
   */
  const rtems_configuration_table Configuration = {
    CONFIGURE_EXECUTIVE_RAM_SIZE,             /* required RTEMS workspace */
    _CONFIGURE_STACK_SPACE_SIZE,               /* required stack space */
    CONFIGURE_MAXIMUM_USER_EXTENSIONS,        /* maximum dynamic extensions */
    CONFIGURE_MICROSECONDS_PER_TICK,          /* microseconds per clock tick */
    CONFIGURE_TICKS_PER_TIMESLICE,            /* ticks per timeslice quantum */
    CONFIGURE_IDLE_TASK_BODY,                 /* user's IDLE task */
    CONFIGURE_IDLE_TASK_STACK_SIZE,           /* IDLE task stack size */
    CONFIGURE_TASK_STACK_ALLOCATOR_INIT,      /* stack allocator init */
    CONFIGURE_TASK_STACK_ALLOCATOR,           /* stack allocator */
    CONFIGURE_TASK_STACK_DEALLOCATOR,         /* stack deallocator */
    CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY,   /* true to clear memory */
    #ifdef CONFIGURE_UNIFIED_WORK_AREAS       /* true for unified work areas */
      true,
    #else
      false,
    #endif
    #ifdef CONFIGURE_TASK_STACK_ALLOCATOR_AVOIDS_WORK_SPACE /* true to avoid
                                                 work space for thread stack
                                                 allocation */
      true,
    #else
      false,
    #endif
    #ifdef RTEMS_SMP
      #ifdef _CONFIGURE_SMP_APPLICATION
        true,
      #else
        false,
      #endif
    #endif
    _CONFIGURE_NUMBER_OF_INITIAL_EXTENSIONS,   /* number of static extensions */
    _CONFIGURE_INITIAL_EXTENSION_TABLE,        /* pointer to static extensions */
    #if defined(RTEMS_MULTIPROCESSING)
      CONFIGURE_MULTIPROCESSING_TABLE,        /* pointer to MP config table */
    #endif
    #ifdef RTEMS_SMP
      _CONFIGURE_MAXIMUM_PROCESSORS,
    #endif
  };

  #if CONFIGURE_RECORD_PER_PROCESSOR_ITEMS > 0
    #include <rtems/record.h>

    #if (CONFIGURE_RECORD_PER_PROCESSOR_ITEMS & (CONFIGURE_RECORD_PER_PROCESSOR_ITEMS - 1)) != 0
      #error "CONFIGURE_RECORD_PER_PROCESSOR_ITEMS must be a power of two"
    #endif

    #if CONFIGURE_RECORD_PER_PROCESSOR_ITEMS < 16
      #error "CONFIGURE_RECORD_PER_PROCESSOR_ITEMS must be at least 16"
    #endif

    typedef struct {
      Record_Control    Control;
      rtems_record_item Items[ CONFIGURE_RECORD_PER_PROCESSOR_ITEMS ];
    } Record_Configured_control;

    static Record_Configured_control _Record_Controls[ _CONFIGURE_MAXIMUM_PROCESSORS ];

    const Record_Configuration _Record_Configuration = {
      CONFIGURE_RECORD_PER_PROCESSOR_ITEMS,
      &_Record_Controls[ 0 ].Control
    };

    RTEMS_SYSINIT_ITEM(
      _Record_Initialize,
      RTEMS_SYSINIT_RECORD,
      RTEMS_SYSINIT_ORDER_MIDDLE
    );
  #endif
#endif

#if defined(RTEMS_SMP)
 /*
  * Instantiate the Per CPU information based upon the user configuration.
  */
 #if defined(CONFIGURE_INIT)
   Per_CPU_Control_envelope _Per_CPU_Information[_CONFIGURE_MAXIMUM_PROCESSORS];
 #endif

#endif

/*
 *  If the user has configured a set of Classic API Initialization Tasks,
 *  then we need to install the code that runs that loop.
 */
#ifdef CONFIGURE_INIT
  #if defined(CONFIGURE_RTEMS_INIT_TASKS_TABLE) || \
      defined(CONFIGURE_HAS_OWN_INIT_TASK_TABLE)
    RTEMS_SYSINIT_ITEM(
      _RTEMS_tasks_Initialize_user_tasks_body,
      RTEMS_SYSINIT_CLASSIC_USER_TASKS,
      RTEMS_SYSINIT_ORDER_MIDDLE
    );
  #endif
#endif

/*
 *  If the user has configured a set of POSIX Initialization Threads,
 *  then we need to install the code that runs that loop.
 */
#ifdef CONFIGURE_INIT
  #if defined(CONFIGURE_POSIX_INIT_THREAD_TABLE) || \
      defined(CONFIGURE_POSIX_HAS_OWN_INIT_THREAD_TABLE)
    posix_initialization_threads_table * const
      _Configuration_POSIX_Initialization_threads =
        CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME;

    const size_t _Configuration_POSIX_Initialization_thread_count =
      CONFIGURE_POSIX_INIT_THREAD_TABLE_SIZE;

    RTEMS_SYSINIT_ITEM(
      _POSIX_Threads_Initialize_user_threads_body,
      RTEMS_SYSINIT_POSIX_USER_THREADS,
      RTEMS_SYSINIT_ORDER_MIDDLE
    );
  #endif
#endif

/*
 *  Select PCI Configuration Library
 */
#ifdef RTEMS_PCI_CONFIG_LIB
  #ifdef CONFIGURE_INIT
    #define PCI_LIB_NONE 0
    #define PCI_LIB_AUTO 1
    #define PCI_LIB_STATIC 2
    #define PCI_LIB_READ 3
    #define PCI_LIB_PERIPHERAL 4
    #if CONFIGURE_PCI_LIB == PCI_LIB_AUTO
      #define PCI_CFG_AUTO_LIB
      #include <pci/cfg.h>
      struct pci_bus pci_hb;
      #define PCI_LIB_INIT pci_config_auto
      #define PCI_LIB_CONFIG pci_config_auto_register
    #elif CONFIGURE_PCI_LIB == PCI_LIB_STATIC
      #define PCI_CFG_STATIC_LIB
      #include <pci/cfg.h>
      #define PCI_LIB_INIT pci_config_static
      #define PCI_LIB_CONFIG NULL
      /* Let user define PCI configuration (struct pci_bus pci_hb) */
    #elif CONFIGURE_PCI_LIB == PCI_LIB_READ
      #define PCI_CFG_READ_LIB
      #include <pci/cfg.h>
      #define PCI_LIB_INIT pci_config_read
      #define PCI_LIB_CONFIG NULL
      struct pci_bus pci_hb;
    #elif CONFIGURE_PCI_LIB == PCI_LIB_PERIPHERAL
      #define PCI_LIB_INIT pci_config_peripheral
      #define PCI_LIB_CONFIG NULL
      /* Let user define PCI configuration (struct pci_bus pci_hb) */
    #elif CONFIGURE_PCI_LIB == PCI_LIB_NONE
      #define PCI_LIB_INIT NULL
      #define PCI_LIB_CONFIG NULL
      /* No PCI Configuration at all, user can use/debug access routines */
    #else
      #error NO PCI LIBRARY DEFINED
    #endif

    const int pci_config_lib_type = CONFIGURE_PCI_LIB;
    int (*pci_config_lib_init)(void) = PCI_LIB_INIT;
    void (*pci_config_lib_register)(void *config) = PCI_LIB_CONFIG;
  #endif
#endif

#ifdef __cplusplus
}
#endif

/******************************************************************
 ******************************************************************
 ******************************************************************
 *         CONFIGURATION WARNINGS AND ERROR CHECKING              *
 ******************************************************************
 ******************************************************************
 ******************************************************************
 */

/*
 *  Make sure a task/thread of some sort is configured.
 *
 *  When analyzing RTEMS to find the smallest possible of memory
 *  that must be allocated, you probably do want to configure 0
 *  tasks/threads so there is a smaller set of calls to _Workspace_Allocate
 *  to analyze.
 */
#if !defined(CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION)
  #if (CONFIGURE_MAXIMUM_TASKS == 0) && \
      (CONFIGURE_MAXIMUM_POSIX_THREADS == 0) && \
      (CONFIGURE_MAXIMUM_ADA_TASKS == 0)
    #error "CONFIGURATION ERROR: No tasks or threads configured!!"
  #endif
#endif

#ifndef RTEMS_SCHEDSIM
/*
 *  Make sure at least one of the initialization task/thread
 *  tables was defined.
 */
#if !defined(CONFIGURE_RTEMS_INIT_TASKS_TABLE) && \
    !defined(CONFIGURE_POSIX_INIT_THREAD_TABLE) && \
    !defined(CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION)
#error "CONFIGURATION ERROR: No initialization tasks or threads configured!!"
#endif
#endif

/*
 *  If the user is trying to configure a multiprocessing application and
 *  RTEMS was not configured and built multiprocessing, then error out.
 */
#if defined(CONFIGURE_MP_APPLICATION) && \
    !defined(RTEMS_MULTIPROCESSING)
#error "CONFIGURATION ERROR: RTEMS not configured for multiprocessing!!"
#endif

#if !defined(RTEMS_SCHEDSIM)
  #if !defined(CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE)
    /*
     *  You must either explicity include or exclude the clock driver.
     *  It is such a common newbie error to leave it out.  Maybe this
     *  will put an end to it.
     *
     *  NOTE: If you are using the timer driver, it is considered
     *        mutually exclusive with the clock driver because the
     *        drivers are assumed to use the same "timer" hardware
     *        on many boards.
     */
    #if !defined(CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER) && \
        !defined(CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER)
      #error "CONFIGURATION ERROR: Do you want the clock driver or not?!?"
     #endif

    /*
     * Only one of the following three configuration parameters should be
     * defined at a time.
     */
    #if ((defined(CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER) + \
          defined(CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER) + \
          defined(CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER)) > 1)
       #error "CONFIGURATION ERROR: More than one clock/timer driver configuration parameter specified?!?"
    #endif
  #endif /* !defined(CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE) */
#endif   /* !defined(RTEMS_SCHEDSIM) */

/*
 *  These names have been obsoleted so make the user application stop compiling
 */
#if defined(CONFIGURE_TEST_NEEDS_TIMER_DRIVER) || \
    defined(CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER) || \
    defined(CONFIGURE_TEST_NEEDS_CLOCK_DRIVER) || \
    defined(CONFIGURE_TEST_NEEDS_RTC_DRIVER) || \
    defined(CONFIGURE_TEST_NEEDS_STUB_DRIVER)
#error "CONFIGURATION ERROR: CONFIGURE_TEST_XXX constants are obsolete"
#endif

/*
 *  Validate the configured maximum priority
 */
#if ((CONFIGURE_MAXIMUM_PRIORITY != 3) && \
     (CONFIGURE_MAXIMUM_PRIORITY != 7) && \
     (CONFIGURE_MAXIMUM_PRIORITY != 15) && \
     (CONFIGURE_MAXIMUM_PRIORITY != 31) && \
     (CONFIGURE_MAXIMUM_PRIORITY != 63) && \
     (CONFIGURE_MAXIMUM_PRIORITY != 127) && \
     (CONFIGURE_MAXIMUM_PRIORITY != 255))
  #error "Maximum priority is not 1 less than a power of 2 between 4 and 256"
#endif

#if (CONFIGURE_MAXIMUM_PRIORITY > PRIORITY_DEFAULT_MAXIMUM)
  #error "Maximum priority configured higher than supported by target."
#endif

#ifdef CONFIGURE_HAS_OWN_CONFIGURATION_TABLE
  #warning "The CONFIGURE_HAS_OWN_CONFIGURATION_TABLE configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_HAS_OWN_FILESYSTEM_TABLE
  #warning "The CONFIGURE_HAS_OWN_FILESYSTEM_TABLE configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_HAS_OWN_MOUNT_TABLE
  #warning "The CONFIGURE_HAS_OWN_MOUNT_TABLE configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_NUMBER_OF_TERMIOS_PORTS
  #warning "The CONFIGURE_NUMBER_OF_TERMIOS_PORTS configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_BARRIERS
  #warning "The CONFIGURE_MAXIMUM_POSIX_BARRIERS configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES
  #warning "The CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUE_DESCRIPTORS
  #warning "The CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUE_DESCRIPTORS configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_MUTEXES
  #warning "The CONFIGURE_MAXIMUM_POSIX_MUTEXES configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_RWLOCKS
  #warning "The CONFIGURE_MAXIMUM_POSIX_RWLOCKS configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_MAXIMUM_POSIX_SPINLOCKS
  #warning "The CONFIGURE_MAXIMUM_POSIX_SPINLOCKS configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_MAXIMUM_PTYS
  #warning "The CONFIGURE_MAXIMUM_PTYS configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_TERMIOS_DISABLED
  #warning "The CONFIGURE_TERMIOS_DISABLED configuration option is obsolete since RTEMS 5.1"
#endif

/*
 * POSIX Key pair shouldn't be less than POSIX Key, which is highly
 * likely to be error.
 */
#if (CONFIGURE_MAXIMUM_POSIX_KEYS != 0) && \
    (CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS != 0)
  #if (CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS < CONFIGURE_MAXIMUM_POSIX_KEYS)
    #error "Fewer POSIX Key pairs than POSIX Key!"
  #endif
#endif

/*
 * IMFS block size for in memory files (memfiles) must be a power of
 * two between 16 and 512 inclusive.
 */
#if ((CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK != 16) && \
     (CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK != 32) && \
     (CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK != 64) && \
     (CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK != 128) && \
     (CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK != 256) && \
     (CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK != 512))
  #error "IMFS Memfile block size must be a power of 2 between 16 and 512"
#endif

#ifdef CONFIGURE_ENABLE_GO
  #warning "The CONFIGURE_ENABLE_GO configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_MAXIMUM_GOROUTINES
  #warning "The CONFIGURE_MAXIMUM_GOROUTINES configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_MAXIMUM_GO_CHANNELS
  #warning "The CONFIGURE_MAXIMUM_GO_CHANNELS configuration option is obsolete since RTEMS 5.1"
#endif

#endif
/* end of include file */
