/**
 * @file rtems/confdefs.h
 *
 *  This include file contains the configuration table template that will
 *  be instantiated by an application based on the setting of a number
 *  of macros.  The macros are documented in the Configuring a System
 *  chapter of the Classic API User's Guide
 *
 *  The model is to estimate the memory required for each configured item
 *  and sum those estimates.  The estimate can be too high or too low for
 *  a variety of reasons:
 *
 *  Reasons estimate is too high:
 *    + FP contexts (not all tasks are FP)
 *
 *  Reasons estimate is too low:
 *    + stacks greater than minimum size
 *    + messages
 *    + application must account for device driver resources
 *    + application must account for add-on library resource requirements
 *
 *  NOTE:  Eventually this may be able to take into account some of
 *         the above.  This procedure has evolved from just enough to
 *         support the RTEMS Test Suites into something that can be
 *         used remarkably reliably by most applications.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __CONFIGURATION_TEMPLATE_h
#define __CONFIGURATION_TEMPLATE_h

/*
 * Include the executive's configuration
 */
#include <rtems.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/wkspace.h>

#ifdef __cplusplus
extern "C" {
#endif

extern rtems_initialization_tasks_table Initialization_tasks[];
extern rtems_driver_address_table       Device_drivers[];
extern rtems_configuration_table        Configuration;
#if defined(RTEMS_MULTIPROCESSING)
  extern rtems_multiprocessing_table      Multiprocessing_configuration;
#endif
#ifdef RTEMS_POSIX_API
  extern posix_api_configuration_table    Configuration_POSIX_API;
#endif

/**
 *  This macro determines whether the RTEMS reentrancy support for
 *  the Newlib C Library is enabled.
 */
#ifdef RTEMS_SCHEDSIM
  #undef RTEMS_NEWLIB
#endif

#if (defined(RTEMS_NEWLIB) && !defined(CONFIGURE_DISABLE_NEWLIB_REENTRANCY))
  #define CONFIGURE_NEWLIB_EXTENSION 1
#else
  #define CONFIGURE_NEWLIB_EXTENSION 0
#endif

#ifndef RTEMS_SCHEDSIM
#include <rtems/libio.h>

#ifdef CONFIGURE_INIT
rtems_libio_init_functions_t rtems_libio_init_helper =
    #ifdef CONFIGURE_APPLICATION_DISABLE_FILESYSTEM
    NULL;
    #else
    rtems_libio_init;
    #endif

rtems_libio_supp_functions_t rtems_libio_supp_helper =
    #ifdef CONFIGURE_APPLICATION_DISABLE_FILESYSTEM
    NULL;
    #else
    open_dev_console;
    #endif

rtems_fs_init_functions_t    rtems_fs_init_helper =
    #ifdef CONFIGURE_APPLICATION_DISABLE_FILESYSTEM
    NULL;
    #else
    rtems_filesystem_initialize;
    #endif
#endif
#endif

/*
 *  If the application disables the filesystem, they will not need
 *  a mount table, so do not produce one.
 */
#ifdef CONFIGURE_APPLICATION_DISABLE_FILESYSTEM
  #define CONFIGURE_HAS_OWN_MOUNT_TABLE
#endif

/**
 *  This macro defines the number of POSIX file descriptors allocated
 *  and managed by libio.  These are the "integer" file descriptors that
 *  are used by calls like open(2) and read(2).
 */
#ifndef CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS
  #define CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS 3
#endif

/**
 *  Semaphore count used by the IO library.
 */
#define CONFIGURE_LIBIO_SEMAPHORES 1

#ifdef CONFIGURE_INIT
  /**
   *  When instantiating the configuration tables, this variable is
   *  initialized to specify the maximum number of file descriptors.
   */
  uint32_t rtems_libio_number_iops = CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS;
#endif

/**
 *  This macro determines if termios is disabled by this application.
 *  This only means that resources will not be reserved.  If you end
 *  up using termios, it will fail.
 */
#ifdef CONFIGURE_TERMIOS_DISABLED
  #define CONFIGURE_TERMIOS_SEMAPHORES 0
#else
  /**
   *  This macro specifies the number of serial or PTY ports that will
   *  use termios.
   */
  #ifndef CONFIGURE_NUMBER_OF_TERMIOS_PORTS
  #define CONFIGURE_NUMBER_OF_TERMIOS_PORTS 1
  #endif

  /**
   *  This macro reserves the number of semaphores required by termios
   *  based upon the number of communication ports that will use it.
   */
  #define CONFIGURE_TERMIOS_SEMAPHORES \
    ((CONFIGURE_NUMBER_OF_TERMIOS_PORTS * 4) + 1)
#endif

/**
 *  This macro specifies the number of PTYs that can be concurrently
 *  active.
 */
#ifndef CONFIGURE_MAXIMUM_PTYS
  #define CONFIGURE_MAXIMUM_PTYS 0
#endif

/**
 *  This variable contains the maximum number of PTYs that can be
 *  concurrently active.
 */
#ifdef CONFIGURE_INIT
  int rtems_telnetd_maximum_ptys = CONFIGURE_MAXIMUM_PTYS;
#else
  extern int rtems_telnetd_maximum_ptys;
#endif

#if defined(RTEMS_SMP)
  /*
   *  If configured for SMP, then we need to know the maximum CPU cores.
   */
  #if !defined(CONFIGURE_SMP_APPLICATION)
    #if !defined(CONFIGURE_SMP_MAXIMUM_PROCESSORS) 
      #define CONFIGURE_SMP_MAXIMUM_PROCESSORS 1
    #endif
  #else
    #if !defined(CONFIGURE_SMP_MAXIMUM_PROCESSORS) 
      #error "CONFIGURE_SMP_MAXIMUM_PROCESSORS not specified for SMP Application"
    #endif
  #endif
#endif

/*
 *  Filesystems and Mount Table Configuration.
 *
 *  Defines to control the file system:
 *
 *   CONFIGURE_APPLICATION_DISABLE_FILESYSTEM:
 *     Disable the RTEMS filesystems. You get an empty DEVFS.
 *
 *   CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM:
 *     Use the DEVFS as the root file system. Limited functions are
 *     provided when this is used.
 *
 *   CONFIGURE_FILESYSTEM_ALL:
 *     Add file filesystems to the default filesystem table.
 *
 *   List of available file systems. You can define as many as you like:
 *     CONFIGURE_FILESYSTEM_MINIIMFS - MiniIMFS, use DEVFS now
 *     CONFIGURE_FILESYSTEM_IMFS     - In Memory File System (IMFS)
 *     CONFIGURE_FILESYSTEM_DEVFS    - Device File System (DSVFS)
 *     CONFIGURE_FILESYSTEM_TFTPFS   - TFTP File System, networking enabled
 *     CONFIGURE_FILESYSTEM_FTPFS    - FTP File System, networking enabled
 *     CONFIGURE_FILESYSTEM_NFS      - Network File System, networking enabled
 *     CONFIGURE_FILESYSTEM_DOSFS    - DOS File System, uses libblock
 *     CONFIGURE_FILESYSTEM_RFS      - RTEMS File System (RFS), uses libblock
 *
 *   Combinations:
 *
 *    - If nothing is defined the base file system is the IMFS.
 *
 *    - If CONFIGURE_APPLICATION_DISABLE_FILESYSTEM is defined all filesystem
 *      are disabled by force and an empty DEVFS is created.
 *
 *    - If CONFIGURE_USE_DEV_AS_BASE_FILESYSTEM is defined all filesystem
 *      are disabled by force and DEVFS is defined.
 */

#ifdef CONFIGURE_INIT

  /*
   * Include all file systems. Do this before checking if the filesystem has
   * been disabled.
   */
  #ifdef CONFIGURE_FILESYSTEM_ALL
    #define CONFIGURE_FILESYSTEM_MINIIMFS
    #define CONFIGURE_FILESYSTEM_IMFS
    #define CONFIGURE_FILESYSTEM_DEVFS
    #define CONFIGURE_FILESYSTEM_TFTPFS
    #define CONFIGURE_FILESYSTEM_FTPFS
    #define CONFIGURE_FILESYSTEM_NFS
    #define CONFIGURE_FILESYSTEM_DOSFS
    #define CONFIGURE_FILESYSTEM_RFS
  #endif

  /*
   * If disabling the file system, give a compile error if the user has
   * configured other filesystem parameters.
   */
  #if defined(CONFIGURE_APPLICATION_DISABLE_FILESYSTEM)
     #if defined(CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM) || \
	 defined(CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM)
       #error "Filesystem disabled but a base filesystem configured."
     #endif

    #if defined(CONFIGURE_FILESYSTEM_MINIIMFS) || \
        defined(CONFIGURE_FILESYSTEM_IMFS) || \
        defined(CONFIGURE_FILESYSTEM_DEVFS) || \
        defined(CONFIGURE_FILESYSTEM_TFTPFS) || \
        defined(CONFIGURE_FILESYSTEM_FTPFS) || \
        defined(CONFIGURE_FILESYSTEM_NFS) || \
        defined(CONFIGURE_FILESYSTEM_DOSFS) || \
        defined(CONFIGURE_FILESYSTEM_RFS)
        #error "Configured filesystems but root filesystem was not IMFS!"
        #error "Filesystems could be disabled, DEVFS is root, or"
        #error "  miniIMFS is root!"
     #endif 
  #endif

  /*
   * If the base filesystem is DEVFS define it else define IMFS.
   * We will have either DEVFS or IMFS defined after this.
   */
  #if !defined(CONFIGURE_APPLICATION_DISABLE_FILESYSTEM)
    #if defined(CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM)
      #define CONFIGURE_FILESYSTEM_DEVFS
    #elif defined(CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM)
      #define CONFIGURE_FILESYSTEM_MINIIMFS
    #elif !defined(CONFIGURE_FILESYSTEM_IMFS)
      #define CONFIGURE_FILESYSTEM_IMFS
    #endif
  #endif

#endif

#ifndef RTEMS_SCHEDSIM
/**
 * IMFS
 */
#include <rtems/imfs.h>

/**
 *  This specifies the number of bytes per block for files within the IMFS.
 *  There are a maximum number of blocks per file so this dictates the maximum
 *  size of a file.  This has to be balanced with the unused portion of each
 *  block that might be wasted.
 */
#ifndef CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK
  #define CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK \
                    IMFS_MEMFILE_DEFAULT_BYTES_PER_BLOCK
#endif

/**
 *  This defines the miniIMFS file system table entry.
 */ 
#if !defined(CONFIGURE_FILESYSTEM_ENTRY_miniIMFS) && \
    defined(CONFIGURE_FILESYSTEM_MINIIMFS)
  #define CONFIGURE_FILESYSTEM_ENTRY_miniIMFS \
    { RTEMS_FILESYSTEM_TYPE_MINIIMFS, miniIMFS_initialize }
#endif
#endif

/**
 *  Internall it is called FIFOs not pipes
 */
#if defined(CONFIGURE_PIPES_ENABLED)
  #define CONFIGURE_FIFOS_ENABLED
#endif

#ifndef RTEMS_SCHEDSIM
/**
 *  This defines the IMFS file system table entry.
 */ 
#if !defined(CONFIGURE_FILESYSTEM_ENTRY_IMFS) && \
    defined(CONFIGURE_FILESYSTEM_IMFS)
  #if defined(CONFIGURE_FIFOS_ENABLED)
    #define CONFIGURE_FILESYSTEM_ENTRY_IMFS \
      { RTEMS_FILESYSTEM_TYPE_IMFS, fifoIMFS_initialize }
  #else
    #define CONFIGURE_FILESYSTEM_ENTRY_IMFS \
      { RTEMS_FILESYSTEM_TYPE_IMFS, IMFS_initialize }
  #endif
#endif
#endif

/**
 *  This sets up the resources for the PIPES/FIFOs
 */
#if defined(CONFIGURE_FIFOS_ENABLED)
  #if !defined(CONFIGURE_MAXIMUM_FIFOS) && !defined(CONFIGURE_MAXIMUM_PIPES)
     #error "No FIFOs or PIPES configured"
  #endif
  #if !defined(CONFIGURE_MAXIMUM_FIFOS)
    #define CONFIGURE_MAXIMUM_FIFOS 0
  #endif
  #if !defined(CONFIGURE_MAXIMUM_PIPES)
    #define CONFIGURE_MAXIMUM_PIPES 0
  #endif
  #define CONFIGURE_BARRIERS_FOR_FIFOS \
    (2 * (CONFIGURE_MAXIMUM_FIFOS + CONFIGURE_MAXIMUM_PIPES))
  #define CONFIGURE_SEMAPHORES_FOR_FIFOS \
    (1 + (CONFIGURE_MAXIMUM_FIFOS + CONFIGURE_MAXIMUM_PIPES))
#else
  #define CONFIGURE_BARRIERS_FOR_FIFOS   0
  #define CONFIGURE_SEMAPHORES_FOR_FIFOS 0
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

#ifdef RTEMS_NETWORKING
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

#ifdef CONFIGURE_INIT

  /**
   *  DEVFS variables.
   *
   *  The number of individual devices that may be registered
   *  in the system or the CONFIGURE_MAXIMUM_DEVICES variable
   *  is defaulted to 4 when a filesystem is enabled, unless
   *  the bsp overwrides this.  In which case the value is set
   *  to BSP_MAXIMUM_DEVICES.
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

#ifndef RTEMS_SCHEDSIM
  #if defined(CONFIGURE_FILESYSTEM_IMFS) || \
      defined(CONFIGURE_FILESYSTEM_MINIIMFS)
    int imfs_rq_memfile_bytes_per_block = CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK;
  #endif
#endif

  /**
   * Table termination record.
   */
  #define CONFIGURE_FILESYSTEM_NULL { NULL, NULL }

#ifndef RTEMS_SCHEDSIM
  /**
   * The default file system table. Must be terminated with the NULL entry if
   * you provide your own.
   */
  #ifndef CONFIGURE_HAS_OWN_FILESYSTEM_TABLE
    const rtems_filesystem_table_t rtems_filesystem_table[] = {
      #if defined(CONFIGURE_FILESYSTEM_MINIIMFS) && \
          defined(CONFIGURE_FILESYSTEM_ENTRY_miniIMFS)
        CONFIGURE_FILESYSTEM_ENTRY_miniIMFS,
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
      CONFIGURE_FILESYSTEM_NULL
    };
  #endif

  #ifndef CONFIGURE_HAS_OWN_MOUNT_TABLE
    #if defined(CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM)
      static devFS_node devFS_root_filesystem_nodes [CONFIGURE_MAXIMUM_DEVICES];
      static const devFS_data devFS_root_filesystem_data = {
        devFS_root_filesystem_nodes,
        CONFIGURE_MAXIMUM_DEVICES
      };
    #endif
    const rtems_filesystem_mount_configuration
      rtems_filesystem_root_configuration = {
      NULL,
      NULL,
      #if defined(CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM)
        RTEMS_FILESYSTEM_TYPE_DEVFS,
      #elif defined(CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM)
        RTEMS_FILESYSTEM_TYPE_MINIIMFS,
      #else
        RTEMS_FILESYSTEM_TYPE_IMFS,
      #endif
      RTEMS_FILESYSTEM_READ_WRITE,
      #if defined(CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM)
        &devFS_root_filesystem_data
      #else
        NULL
      #endif
    };
  #endif

#endif
#endif

/*
 *  STACK_CHECKER_ON was still available in 4.9 so give a warning for now.
 */
#if defined(STACK_CHECKER_ON)
  #define CONFIGURE_STACK_CHECKER_ENABLED
  #warning "STACK_CHECKER_ON deprecated -- use CONFIGURE_STACK_CHECKER_ENABLED"
#endif

/**
 *  This configures the stack checker user extension.
 */
#ifdef CONFIGURE_STACK_CHECKER_ENABLED
  #define CONFIGURE_STACK_CHECKER_EXTENSION 1
#else
  #define CONFIGURE_STACK_CHECKER_EXTENSION 0
#endif

/**
 *  @brief Maximum Priority configuration
 *
 *  This configures the maximum priority value that
 *  a task may have.
 *
 *  The following applies to the data space requirements
 *  of the Priority Scheduler.
 *
 *  By reducing the number of priorities in a system,
 *  the amount of RAM required by RTEMS can be significantly
 *  reduced.  RTEMS allocates a Chain_Control structure per
 *  priority and this structure contains 3 pointers.  So
 *  the default is (256 * 12) = 3K on 32-bit architectures.
 *
 *  This must be one less than a power of 2 between
 *  4 and 256.  Valid values along with the application
 *  priority levels and memory saved when pointers are
 *  32-bits in size are:
 *
 *    + 3,  2 application priorities, 3024 bytes saved
 *    + 7, 5 application priorities, 2976 bytes saved
 *    + 15, 13 application priorities, 2880 bytes saved
 *    + 31, 29 application priorities, 2688 bytes saved
 *    + 63, 61 application priorities, 2304 bytes saved
 *    + 127, 125 application priorities, 1536 bytes saved
 *    + 255, 253 application priorities, 0 bytes saved
 *
 *  It is specified in terms of Classic API priority values.
 */
#ifndef CONFIGURE_MAXIMUM_PRIORITY
  #define CONFIGURE_MAXIMUM_PRIORITY PRIORITY_DEFAULT_MAXIMUM
#endif

/*
 * Scheduler configuration.
 *
 * The scheduler configuration allows an application to select the 
 * scheduling policy to use.  The supported configurations are:
 *  CONFIGURE_SCHEDULER_USER       - user provided scheduler
 *  CONFIGURE_SCHEDULER_PRIORITY   - Deterministic Priority Scheduler
 *  CONFIGURE_SCHEDULER_SIMPLE     - Light-weight Priority Scheduler
 *  CONFIGURE_SCHEDULER_SIMPLE_SMP - Simple SMP Priority Scheduler
 *  CONFIGURE_SCHEDULER_EDF        - EDF Scheduler
 *  CONFIGURE_SCHEDULER_CBS        - CBS Scheduler
 * 
 * If no configuration is specified by the application, then 
 * CONFIGURE_SCHEDULER_PRIORITY is assumed to be the default.
 *
 * An application can define its own scheduling policy by defining
 * CONFIGURE_SCHEDULER_USER and the following:
 *    - CONFIGURE_SCHEDULER_ENTRY_POINTS 
 *    - CONFIGURE_MEMORY_FOR_SCHEDULER - base memory
 *    - CONFIGURE_MEMORY_PER_TASK_FOR_SCHEDULER - per task memory
 */
#include <rtems/score/scheduler.h>

#if defined(CONFIGURE_SCHEDULER_USER) && \
    !defined(CONFIGURE_SCHEDULER_USER_ENTRY_POINTS)
  #error "CONFIGURE_ERROR: CONFIGURE_SCHEDULER_USER requires CONFIGURE_SCHEDULER_USER_ENTRY_POINTS"
#endif

#if !defined(RTEMS_SMP)
  #undef CONFIGURE_SCHEDULER_SIMPLE_SMP
#endif

/* If no scheduler is specified, the priority scheduler is default. */
#if !defined(CONFIGURE_SCHEDULER_USER) && \
    !defined(CONFIGURE_SCHEDULER_PRIORITY) && \
    !defined(CONFIGURE_SCHEDULER_SIMPLE) && \
    !defined(CONFIGURE_SCHEDULER_SIMPLE_SMP) && \
    !defined(CONFIGURE_SCHEDULER_EDF) && \
    !defined(CONFIGURE_SCHEDULER_CBS)
  #if defined(RTEMS_SMP) && defined(CONFIGURE_SMP_APPLICATION)
    #define CONFIGURE_SCHEDULER_SIMPLE_SMP
  #else
    #define CONFIGURE_SCHEDULER_PRIORITY
  #endif
#endif

/* 
 * If the Priority Scheduler is selected, then configure for it.
 */
#if defined(CONFIGURE_SCHEDULER_PRIORITY)
  #include <rtems/score/schedulerpriority.h>
  #define CONFIGURE_SCHEDULER_ENTRY_POINTS SCHEDULER_PRIORITY_ENTRY_POINTS

  /**
   * This defines the memory used by the priority scheduler.
   */
  #define CONFIGURE_MEMORY_FOR_SCHEDULER ( \
    _Configure_From_workspace( \
      ((CONFIGURE_MAXIMUM_PRIORITY+1) * sizeof(Chain_Control)) ) \
  )
  #define CONFIGURE_MEMORY_PER_TASK_FOR_SCHEDULER ( \
    _Configure_From_workspace(sizeof(Scheduler_priority_Per_thread)) )
#endif

/* 
 * If the Simple Priority Scheduler is selected, then configure for it.
 */
#if defined(CONFIGURE_SCHEDULER_SIMPLE)
  #include <rtems/score/schedulersimple.h>
  #define CONFIGURE_SCHEDULER_ENTRY_POINTS SCHEDULER_SIMPLE_ENTRY_POINTS

  /**
   * define the memory used by the simple scheduler
   */
  #define CONFIGURE_MEMORY_FOR_SCHEDULER ( \
    _Configure_From_workspace( sizeof(Chain_Control) ) \
  )
  #define CONFIGURE_MEMORY_PER_TASK_FOR_SCHEDULER (0)
#endif

/*
 * If the Simple SMP Priority Scheduler is selected, then configure for it.
 */
#if defined(CONFIGURE_SCHEDULER_SIMPLE_SMP)
  #include <rtems/score/schedulersimplesmp.h>
  #define CONFIGURE_SCHEDULER_ENTRY_POINTS SCHEDULER_SIMPLE_SMP_ENTRY_POINTS

  /**
   * Define the memory used by the Simple SMP Scheduler
   *
   * NOTE: This is the same as the Simple Scheduler
   */
  #define CONFIGURE_MEMORY_FOR_SCHEDULER ( \
    _Configure_From_workspace( sizeof(Chain_Control) ) \
  )
  #define CONFIGURE_MEMORY_PER_TASK_FOR_SCHEDULER (0)
#endif

/*
 * If the EDF Scheduler is selected, then configure for it.
 */
#if defined(CONFIGURE_SCHEDULER_EDF)
  #include <rtems/score/scheduleredf.h>
  #define CONFIGURE_SCHEDULER_ENTRY_POINTS SCHEDULER_EDF_ENTRY_POINTS

  /**
   * define the memory used by the EDF scheduler
   */
  #define CONFIGURE_MEMORY_FOR_SCHEDULER ( \
    _Configure_From_workspace(0))
  #define CONFIGURE_MEMORY_PER_TASK_FOR_SCHEDULER ( \
    _Configure_From_workspace(sizeof(Scheduler_EDF_Per_thread)))
#endif

/*
 * If the CBS Scheduler is selected, then configure for it.
 */
#if defined(CONFIGURE_SCHEDULER_CBS)
  #include <rtems/score/schedulercbs.h>
  #define CONFIGURE_SCHEDULER_ENTRY_POINTS SCHEDULER_CBS_ENTRY_POINTS

  #ifndef CONFIGURE_CBS_MAXIMUM_SERVERS
    #define CONFIGURE_CBS_MAXIMUM_SERVERS CONFIGURE_MAXIMUM_TASKS
  #endif

  #ifdef CONFIGURE_INIT
    uint32_t _Scheduler_CBS_Maximum_servers = CONFIGURE_CBS_MAXIMUM_SERVERS;
  #endif

  /**
   * define the memory used by the CBS scheduler
   */
  #define CONFIGURE_MEMORY_FOR_SCHEDULER ( \
    _Configure_From_workspace((sizeof(Scheduler_CBS_Server) + \
        sizeof(Scheduler_CBS_Server*)) * CONFIGURE_CBS_MAXIMUM_SERVERS))
  #define CONFIGURE_MEMORY_PER_TASK_FOR_SCHEDULER ( \
    _Configure_From_workspace(sizeof(Scheduler_CBS_Per_thread)))
#endif

#if defined(CONFIGURE_SCHEDULER_USER)
  #define CONFIGURE_SCHEDULER_ENTRY_POINTS \
          CONFIGURE_SCHEDULER_USER_ENTRY_POINTS

  #define CONFIGURE_SCHEDULER_MEMORY_FOR_SCHEDULER \
          CONFIGURE_SCHEDULER_USER_MEMORY_FOR_SCHEDULER
#endif

/* 
 * Set up the scheduler entry points table.  The scheduling code uses
 * this code to know which scheduler is configured by the user.
 */
#ifdef CONFIGURE_INIT
  Scheduler_Control  _Scheduler = {
    NULL,                             /* Scheduler Specific Data Pointer */
    CONFIGURE_SCHEDULER_ENTRY_POINTS  /* Scheduler Operations */
  };
#endif

/*
 *  If you said the IDLE task was going to do application initialization
 *  and didn't override the IDLE body, then something is amiss.
 */
#if (defined(CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION) && \
     !defined(CONFIGURE_IDLE_TASK_BODY))
  #error "CONFIGURE_ERROR: You did not override the IDLE task body."
#endif

/**
 *  @brief Idle task body configuration
 *
 *  There is a default IDLE thread body provided by RTEMS which
 *  has the possibility of being CPU specific.  There may be a
 *  BSP specific override of the RTEMS default body and in turn,
 *  the application may override and provide its own.
 */
#ifndef CONFIGURE_IDLE_TASK_BODY
  #if defined(BSP_IDLE_TASK_BODY)
    #define CONFIGURE_IDLE_TASK_BODY BSP_IDLE_TASK_BODY
  #elif (CPU_PROVIDES_IDLE_THREAD_BODY == TRUE)
    #define CONFIGURE_IDLE_TASK_BODY _CPU_Thread_Idle_body
  #else
    /* only instantiate and compile if used */
    #ifdef CONFIGURE_INIT
      void *_Thread_Idle_body(uintptr_t ignored)
      {
        for( ; ; ) ;
        return 0;   /* to avoid warning */
      }
    #endif
    #define CONFIGURE_IDLE_TASK_BODY _Thread_Idle_body
  #endif
#endif

/**
 *  By default, use the minimum stack size requested by this port.
 */
#ifndef CONFIGURE_MINIMUM_TASK_STACK_SIZE
  #define CONFIGURE_MINIMUM_TASK_STACK_SIZE CPU_STACK_MINIMUM_SIZE
#endif

#define CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE \
  (2 * CONFIGURE_MINIMUM_TASK_STACK_SIZE)

/**
 *  @brief Idle task stack size configuration
 *
 *  By default, the IDLE task will have a stack of minimum size.
 *  The BSP or application may override this value.
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

/**
 *  @brief Interrupt stack size configuration
 *
 *  By default, the interrupt stack will be of minimum size.
 *  The BSP or application may override this value.
 */
#ifndef CONFIGURE_INTERRUPT_STACK_SIZE
  #ifdef BSP_INTERRUPT_STACK_SIZE
    #define CONFIGURE_INTERRUPT_STACK_SIZE BSP_INTERRUPT_STACK_SIZE
  #else
    #define CONFIGURE_INTERRUPT_STACK_SIZE CONFIGURE_MINIMUM_TASK_STACK_SIZE
  #endif
#endif

/**
 *  This reserves memory for the interrupt stack if it is to be allocated
 *  by RTEMS rather than the BSP.
 *
 *  @todo Try to get to the point where all BSPs support allocating the
 *        memory from the Workspace.
 */
#if (CPU_ALLOCATE_INTERRUPT_STACK == 0)
  #define CONFIGURE_INTERRUPT_STACK_MEMORY 0
#else
  #define CONFIGURE_INTERRUPT_STACK_MEMORY \
     _Configure_From_workspace( CONFIGURE_INTERRUPT_STACK_SIZE )
#endif

/**
 *  Configure the very much optional task stack allocator initialization
 */
#ifndef CONFIGURE_TASK_STACK_ALLOCATOR_INIT
  #define CONFIGURE_TASK_STACK_ALLOCATOR_INIT NULL
#endif

/*
 *  Configure the very much optional task stack allocator and deallocator.
 */
#if !defined(CONFIGURE_TASK_STACK_ALLOCATOR) \
  && !defined(CONFIGURE_TASK_STACK_DEALLOCATOR)
  #define CONFIGURE_TASK_STACK_ALLOCATOR _Workspace_Allocate
  #define CONFIGURE_TASK_STACK_DEALLOCATOR _Workspace_Free
#elif (defined(CONFIGURE_TASK_STACK_ALLOCATOR) \
  && !defined(CONFIGURE_TASK_STACK_DEALLOCATOR)) \
    || (!defined(CONFIGURE_TASK_STACK_ALLOCATOR) \
      && defined(CONFIGURE_TASK_STACK_DEALLOCATOR))
  #error "CONFIGURE_TASK_STACK_ALLOCATOR and CONFIGURE_TASK_STACK_DEALLOCATOR must be both defined or both undefined"
#endif

/**
 *  Should the RTEMS Workspace and C Program Heap be cleared automatically
 *  at system start up?
 */
#ifndef CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY
  #ifdef BSP_ZERO_WORKSPACE_AUTOMATICALLY
    #define CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY \
            BSP_ZERO_WORKSPACE_AUTOMATICALLY
  #else
    #define CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY FALSE
  #endif
#endif

/*
 *  RTEMS Malloc configuration
 */

#include <rtems/malloc.h>

#ifdef CONFIGURE_INIT
  /**
   *  By default, RTEMS uses separate heaps for the RTEMS Workspace and
   *  the C Program Heap.  On many BSPs, these can be optionally
   *  combined provided one larger memory pool. This is particularly
   *  useful in combination with the unlimited objects configuration.
   */
  #ifdef BSP_DEFAULT_UNIFIED_WORK_AREAS
    #ifndef CONFIGURE_UNIFIED_WORK_AREAS
      #define CONFIGURE_UNIFIED_WORK_AREAS
    #endif
  #endif
  
  #ifdef CONFIGURE_UNIFIED_WORK_AREAS
    Heap_Control  *RTEMS_Malloc_Heap = &_Workspace_Area;
  #else
    Heap_Control   RTEMS_Malloc_Area;
    Heap_Control  *RTEMS_Malloc_Heap = &RTEMS_Malloc_Area;
  #endif
#endif

#ifdef CONFIGURE_INIT
  /**
   *  This configures the malloc family statistics to be available.
   *  By default only function call counts are kept.
   */
  rtems_malloc_statistics_functions_t *rtems_malloc_statistics_helpers =
    #ifndef CONFIGURE_MALLOC_STATISTICS
      NULL;
    #else
      &rtems_malloc_statistics_helpers_table;
    #endif
#endif

#ifdef CONFIGURE_INIT
  /**
   *  This configures the sbrk() support for the malloc family.
   *  By default it is assumed that the BSP provides all available
   *  RAM to the malloc family implementation so sbrk()'ing to get
   *  more memory would always fail anyway.
   */
  rtems_malloc_sbrk_functions_t *rtems_malloc_sbrk_helpers =
    #ifndef CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK
      NULL;
    #else
      &rtems_malloc_sbrk_helpers_table;
    #endif
#endif

#ifdef CONFIGURE_INIT
  /**
   *  This configures the malloc family plugin which dirties memory
   *  allocated.  This is helpful for finding unitialized data structure
   *  problems.
   */
  rtems_malloc_dirtier_t rtems_malloc_dirty_helper =
    #if defined(CONFIGURE_MALLOC_DIRTY)
      rtems_malloc_dirty_memory;
    #else
      NULL;
    #endif
#endif

/**
 *  This is a helper macro used in calculations in this file.  It is used
 *  to noted when an element is allocated from the RTEMS Workspace and adds
 *  a factor to account for heap overhead plus an alignment factor that
 *  may be applied.
 */
#define _Configure_From_workspace(_size) \
  (ssize_t)((_size) + HEAP_BLOCK_HEADER_SIZE + CPU_HEAP_ALIGNMENT - 1)

/**
 *  This is a helper macro used in stack space calculations in this file.  It
 *  may be provided by the application in case a special task stack allocator
 *  is used.  The default is allocation from the RTEMS Workspace.
 */
#ifdef CONFIGURE_TASK_STACK_FROM_ALLOCATOR
  #define _Configure_From_stackspace(_stack_size) \
    CONFIGURE_TASK_STACK_FROM_ALLOCATOR(_stack_size)
#else
  #define _Configure_From_stackspace(_stack_size) \
    _Configure_From_workspace(_stack_size)
#endif

/**
 *  Do not use the unlimited bit as part of the multiplication
 *  for memory usage.
 */
#define _Configure_Max_Objects(_max) \
  rtems_resource_maximum_per_allocation(_max)

/**
 *  This macro accounts for how memory for a set of configured objects is
 *  allocated from the Executive Workspace.
 *
 *  NOTE: It does NOT attempt to address the more complex case of unlimited
 *        objects.
 */
#define _Configure_Object_RAM(_number, _size) \
  ( _Configure_From_workspace(_Configure_Max_Objects(_number) * (_size)) + \
    _Configure_From_workspace( \
      ((_Configure_Max_Objects(_number) + 1) * sizeof(Objects_Control *)) + \
      (sizeof(void *) + sizeof(uint32_t) + sizeof(Objects_Name *)) \
    ) \
  )

/*
 *  Default User Initialization Task Table.  This table guarantees that
 *  one user initialization table is defined.
 */

#ifdef CONFIGURE_RTEMS_INIT_TASKS_TABLE

#ifdef CONFIGURE_HAS_OWN_INIT_TASK_TABLE

/*
 *  The user is defining their own table information and setting the
 *  appropriate variables.
 */

#else

#ifndef CONFIGURE_INIT_TASK_NAME
  #define CONFIGURE_INIT_TASK_NAME          rtems_build_name('U', 'I', '1', ' ')
#endif

#ifndef CONFIGURE_INIT_TASK_STACK_SIZE
  #define CONFIGURE_INIT_TASK_STACK_SIZE    CONFIGURE_MINIMUM_TASK_STACK_SIZE
#endif

#ifndef CONFIGURE_INIT_TASK_PRIORITY
  #define CONFIGURE_INIT_TASK_PRIORITY      1
#endif

#ifndef CONFIGURE_INIT_TASK_ATTRIBUTES
  #define CONFIGURE_INIT_TASK_ATTRIBUTES    RTEMS_DEFAULT_ATTRIBUTES
#endif

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

#ifndef CONFIGURE_INIT_TASK_INITIAL_MODES
  #define CONFIGURE_INIT_TASK_INITIAL_MODES RTEMS_NO_PREEMPT
#endif

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

#define CONFIGURE_INIT_TASK_TABLE Initialization_tasks

#define CONFIGURE_INIT_TASK_TABLE_SIZE \
  sizeof(CONFIGURE_INIT_TASK_TABLE) / sizeof(rtems_initialization_tasks_table)

#endif    /* CONFIGURE_HAS_OWN_INIT_TASK_TABLE */

#else     /* CONFIGURE_RTEMS_INIT_TASKS_TABLE */

#define CONFIGURE_INIT_TASK_TABLE      NULL
#define CONFIGURE_INIT_TASK_TABLE_SIZE 0
#define CONFIGURE_INIT_TASK_STACK_SIZE 0

#endif

/*
 *  Default Device Driver Table.  Each driver needed by the test is explicitly
 *  choosen by that test.  There is always a null driver entry.
 */

#define NULL_DRIVER_TABLE_ENTRY \
 { NULL, NULL, NULL, NULL, NULL, NULL }

#ifdef CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
  #include <rtems/console.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
  #include <rtems/clockdrv.h>
#endif

#ifdef CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER
  #include <rtems/timerdrv.h>
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

#ifdef CONFIGURE_INIT
  rtems_driver_address_table Device_drivers[] = {
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
#endif

#endif  /* CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE */

/*
 *  Default the number of drivers per node.  This value may be
 *  overridden by the user.
 */

#define CONFIGURE_NUMBER_OF_DRIVERS \
  ((sizeof(Device_drivers) / sizeof(rtems_driver_address_table)))

/**
 *  This specifies the maximum number of device drivers that
 *  can be installed in the system at one time.  It must account
 *  for both the statically and dynamically installed drivers.
 */
#ifndef CONFIGURE_MAXIMUM_DRIVERS
  #define CONFIGURE_MAXIMUM_DRIVERS CONFIGURE_NUMBER_OF_DRIVERS
#endif


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

/*
 * add bdbuf configuration and values for swapout task priority
 */
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

  #define CONFIGURE_LIBBLOCK_TASKS \
    (1 + CONFIGURE_SWAPOUT_WORKER_TASKS + \
    (CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS != 0))

  #define CONFIGURE_LIBBLOCK_TASK_EXTRA_STACKS \
    (CONFIGURE_LIBBLOCK_TASKS * \
    (CONFIGURE_BDBUF_TASK_STACK_SIZE <= CONFIGURE_MINIMUM_TASK_STACK_SIZE ? \
    0 : CONFIGURE_BDBUF_TASK_STACK_SIZE - CONFIGURE_MINIMUM_TASK_STACK_SIZE))

  /*
   *  Semaphores:
   *    o disk lock
   *    o bdbuf lock
   *    o bdbuf sync lock
   *    o bdbuf access condition
   *    o bdbuf transfer condition
   *    o bdbuf buffer condition
   */
  #define CONFIGURE_LIBBLOCK_SEMAPHORES 6

  #if defined(CONFIGURE_HAS_OWN_BDBUF_TABLE) || \
      defined(CONFIGURE_BDBUF_BUFFER_SIZE) || \
      defined(CONFIGURE_BDBUF_BUFFER_COUNT)
    #error BDBUF Cache does not use a buffer configuration table. Please remove.
  #endif
#else
  #define CONFIGURE_LIBBLOCK_TASKS 0
  #define CONFIGURE_LIBBLOCK_TASK_EXTRA_STACKS 0
  #define CONFIGURE_LIBBLOCK_SEMAPHORES 0
#endif /* CONFIGURE_APPLICATION_NEEDS_LIBBLOCK */

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
    #define CONFIGURE_TIMER_FOR_SHARED_MEMORY_DRIVER 1

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
      #define CONFIGURE_MEMORY_FOR_GLOBAL_OBJECTS(_global_objects) \
        _Configure_Object_RAM((_global_objects), sizeof(Objects_MP_Control))

      #ifndef CONFIGURE_MP_MAXIMUM_PROXIES
        #define CONFIGURE_MP_MAXIMUM_PROXIES            32
      #endif
      #define CONFIGURE_MEMORY_FOR_PROXIES(_proxies) \
        _Configure_Object_RAM((_proxies) + 1, sizeof(Thread_Proxy_control) )

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

    #endif /* CONFIGURE_HAS_OWN_MULTIPROCESSING_TABLE */

  #else

    #define CONFIGURE_MULTIPROCESSING_TABLE    NULL

  #endif /* CONFIGURE_MP_APPLICATION */
#endif /* RTEMS_MULTIPROCESSING */

#ifndef CONFIGURE_TIMER_FOR_SHARED_MEMORY_DRIVER
  #define CONFIGURE_TIMER_FOR_SHARED_MEMORY_DRIVER 0
#endif


/**
 * This macro specifies that the user wants to use unlimited objects for any
 * classic or posix objects that have not already been given resource limits.
 */
#if defined(CONFIGURE_UNLIMITED_OBJECTS)
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

  #ifdef RTEMS_POSIX_API
    #if !defined(CONFIGURE_MAXIMUM_POSIX_THREADS)
      #define CONFIGURE_MAXIMUM_POSIX_THREADS \
        rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
    #endif
    #if !defined(CONFIGURE_MAXIMUM_POSIX_MUTEXES)
      #define CONFIGURE_MAXIMUM_POSIX_MUTEXES \
        rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
    #endif
    #if !defined(CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES)
      #define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES \
        rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
    #endif
/*
    #if !defined(CONFIGURE_MAXIMUM_POSIX_KEYS)
      #define CONFIGURE_MAXIMUM_POSIX_KEYS \
        rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
    #endif
*/
    #if !defined(CONFIGURE_MAXIMUM_POSIX_TIMERS)
      #define CONFIGURE_MAXIMUM_POSIX_TIMERS \
        rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
    #endif
/*
    #if !defined(CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS)
      #define CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS \
        rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
    #endif
*/
    #if !defined(CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES)
      #define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES \
        rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
    #endif
    #if !defined(CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUE_DESCRIPTORS)
      #define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUE_DESCRIPTORS \
        rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
    #endif
    #if !defined(CONFIGURE_MAXIMUM_POSIX_SEMAPHORES)
      #define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES \
        rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
    #endif
    #if !defined(CONFIGURE_MAXIMUM_POSIX_BARRIERS)
      #define CONFIGURE_MAXIMUM_POSIX_BARRIERS \
        rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
    #endif
    #if !defined(CONFIGURE_MAXIMUM_POSIX_RWLOCKS)
      #define CONFIGURE_MAXIMUM_POSIX_RWLOCKS \
        rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
    #endif
    #if !defined(CONFIGURE_MAXIMUM_POSIX_SPINLOCKS)
      #define CONFIGURE_MAXIMUM_POSIX_SPINLOCKS \
        rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)
    #endif
  #endif /* RTEMS_POSIX_API */
#endif /* CONFIGURE_UNLIMITED_OBJECTS */


/*
 *  Default Configuration Table.
 */

#ifndef CONFIGURE_HAS_OWN_CONFIGURATION_TABLE

  #ifndef CONFIGURE_MAXIMUM_TASKS
    #define CONFIGURE_MAXIMUM_TASKS               0
  #endif

  #define CONFIGURE_TASKS \
    (CONFIGURE_MAXIMUM_TASKS + CONFIGURE_LIBBLOCK_TASKS)

  #ifndef CONFIGURE_DISABLE_CLASSIC_API_NOTEPADS
    #define CONFIGURE_NOTEPADS_ENABLED           TRUE
  #else
    #define CONFIGURE_NOTEPADS_ENABLED           FALSE
  #endif

  #ifndef CONFIGURE_DISABLE_CLASSIC_API_NOTEPADS
    #define CONFIGURE_MEMORY_PER_TASK_FOR_CLASSIC_API \
      _Configure_From_workspace( sizeof(RTEMS_API_Control) )
  #else
    #define CONFIGURE_MEMORY_PER_TASK_FOR_CLASSIC_API \
      _Configure_From_workspace( sizeof(RTEMS_API_Control) - \
        (RTEMS_NUMBER_NOTEPADS * sizeof(uint32_t)))
  #endif

  /**
   *  This macro calculates the memory required for task variables.
   *
   *  @note Each task variable is individually allocated from the Workspace.
   *        Hence, we do the multiplication on the configured size.
   */
  #ifndef CONFIGURE_MAXIMUM_TASK_VARIABLES
    #define CONFIGURE_MAXIMUM_TASK_VARIABLES                     0
    #define CONFIGURE_MEMORY_FOR_TASK_VARIABLES(_task_variables) 0
  #else
    #define CONFIGURE_MEMORY_FOR_TASK_VARIABLES(_task_variables) \
      (_task_variables) * \
         _Configure_From_workspace(sizeof(rtems_task_variable_t))
  #endif

  #ifndef CONFIGURE_MAXIMUM_TIMERS
    #define CONFIGURE_MAXIMUM_TIMERS             0
    #define CONFIGURE_MEMORY_FOR_TIMERS(_timers) 0
  #else
    #define CONFIGURE_MEMORY_FOR_TIMERS(_timers) \
      _Configure_Object_RAM(_timers, sizeof(Timer_Control) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_SEMAPHORES
    #define CONFIGURE_MAXIMUM_SEMAPHORES                 0
  #endif

  #define CONFIGURE_SEMAPHORES \
    (CONFIGURE_MAXIMUM_SEMAPHORES + CONFIGURE_LIBIO_SEMAPHORES + \
      CONFIGURE_TERMIOS_SEMAPHORES + CONFIGURE_LIBBLOCK_SEMAPHORES + \
      CONFIGURE_SEMAPHORES_FOR_FIFOS)

  /*
   * If there are no user or support semaphores defined, then we can assume
   * that no memory need be allocated at all for semaphores.
   */
  #if CONFIGURE_SEMAPHORES == 0
    #define CONFIGURE_MEMORY_FOR_SEMAPHORES(_semaphores) 0
  #else
    #define CONFIGURE_MEMORY_FOR_SEMAPHORES(_semaphores) \
      _Configure_Object_RAM(_semaphores, sizeof(Semaphore_Control) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_MESSAGE_QUEUES
    #define CONFIGURE_MAXIMUM_MESSAGE_QUEUES             0
    #define CONFIGURE_MEMORY_FOR_MESSAGE_QUEUES(_queues) 0
  #else
    #define CONFIGURE_MEMORY_FOR_MESSAGE_QUEUES(_queues) \
      _Configure_Object_RAM(_queues, sizeof(Message_queue_Control) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_PARTITIONS
    #define CONFIGURE_MAXIMUM_PARTITIONS                 0
    #define CONFIGURE_MEMORY_FOR_PARTITIONS(_partitions) 0
  #else
    #define CONFIGURE_MEMORY_FOR_PARTITIONS(_partitions) \
      _Configure_Object_RAM(_partitions, sizeof(Partition_Control) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_REGIONS
    #define CONFIGURE_MAXIMUM_REGIONS              0
    #define CONFIGURE_MEMORY_FOR_REGIONS(_regions) 0
  #else
    #define CONFIGURE_MEMORY_FOR_REGIONS(_regions) \
      _Configure_Object_RAM(_regions, sizeof(Region_Control) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_PORTS
    #define CONFIGURE_MAXIMUM_PORTS            0
    #define CONFIGURE_MEMORY_FOR_PORTS(_ports) 0
  #else
    #define CONFIGURE_MEMORY_FOR_PORTS(_ports) \
      _Configure_Object_RAM(_ports, sizeof(Dual_ported_memory_Control) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_PERIODS
    #define CONFIGURE_MAXIMUM_PERIODS              0
    #define CONFIGURE_MEMORY_FOR_PERIODS(_periods) 0
  #else
    #define CONFIGURE_MEMORY_FOR_PERIODS(_periods) \
      _Configure_Object_RAM(_periods, sizeof(Rate_monotonic_Control) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_BARRIERS
    #define CONFIGURE_MAXIMUM_BARRIERS               0
  #endif

  #define CONFIGURE_BARRIERS \
     (CONFIGURE_MAXIMUM_BARRIERS + CONFIGURE_BARRIERS_FOR_FIFOS)

  #if CONFIGURE_BARRIERS == 0
    #define CONFIGURE_MEMORY_FOR_BARRIERS(_barriers) 0
  #else
    #define CONFIGURE_MEMORY_FOR_BARRIERS(_barriers) \
      _Configure_Object_RAM(_barriers, sizeof(Barrier_Control) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_USER_EXTENSIONS
    #define CONFIGURE_MAXIMUM_USER_EXTENSIONS                 0
    #define CONFIGURE_MEMORY_FOR_USER_EXTENSIONS(_extensions) 0
  #else
    #define CONFIGURE_MEMORY_FOR_USER_EXTENSIONS(_extensions) \
      _Configure_Object_RAM(_extensions, sizeof(Extension_Control) )
  #endif

  #ifndef CONFIGURE_MICROSECONDS_PER_TICK
    #define CONFIGURE_MICROSECONDS_PER_TICK \
            RTEMS_MILLISECONDS_TO_MICROSECONDS(10)
  #endif

  #ifndef CONFIGURE_TICKS_PER_TIMESLICE
    #define CONFIGURE_TICKS_PER_TIMESLICE        50
  #endif

/*
 *  Initial Extension Set
 */

#ifdef CONFIGURE_INIT
#ifdef CONFIGURE_STACK_CHECKER_ENABLED
#include <rtems/stackchk.h>
#endif
#include <rtems/libcsupport.h>

#if defined(CONFIGURE_INITIAL_EXTENSIONS) || \
    defined(CONFIGURE_STACK_CHECKER_ENABLED) || \
    (defined(RTEMS_NEWLIB) && !defined(CONFIGURE_DISABLE_NEWLIB_REENTRANCY))
  rtems_extensions_table Configuration_Initial_Extensions[] = {
    #if !defined(CONFIGURE_DISABLE_NEWLIB_REENTRANCY)
      RTEMS_NEWLIB_EXTENSION,
    #endif
    #if defined(CONFIGURE_STACK_CHECKER_ENABLED)
      RTEMS_STACK_CHECKER_EXTENSION,
    #endif
    #if defined(CONFIGURE_INITIAL_EXTENSIONS)
      CONFIGURE_INITIAL_EXTENSIONS,
    #endif
  };

  #define CONFIGURE_INITIAL_EXTENSION_TABLE Configuration_Initial_Extensions
  #define CONFIGURE_NUMBER_OF_INITIAL_EXTENSIONS \
    ((sizeof(Configuration_Initial_Extensions) / \
      sizeof(rtems_extensions_table)))
#else
  #define CONFIGURE_INITIAL_EXTENSION_TABLE NULL
  #define CONFIGURE_NUMBER_OF_INITIAL_EXTENSIONS 0
#endif


#endif

/*
 *  POSIX API Configuration Parameters
 */

#ifdef RTEMS_POSIX_API

  #include <sys/types.h>
  #include <signal.h>
  #include <limits.h>
  #include <mqueue.h>
  #include <rtems/posix/barrier.h>
  #include <rtems/posix/cond.h>
  #include <rtems/posix/mqueue.h>
  #include <rtems/posix/mutex.h>
  #include <rtems/posix/key.h>
  #include <rtems/posix/psignal.h>
  #include <rtems/posix/rwlock.h>
  #include <rtems/posix/semaphore.h>
  #include <rtems/posix/spinlock.h>
  #include <rtems/posix/threadsup.h>
  #include <rtems/posix/timer.h>

  /**
   *  Account for the object control structures plus the name
   *  of the object to be duplicated.
   */
  #define _Configure_POSIX_Named_Object_RAM(_number, _size) \
    _Configure_Object_RAM( (_number), _size ) + \
    (_Configure_Max_Objects(_number) * _Configure_From_workspace(NAME_MAX) )

  #ifndef CONFIGURE_MAXIMUM_POSIX_THREADS
    #define CONFIGURE_MAXIMUM_POSIX_THREADS      0
  #endif

  #define CONFIGURE_MEMORY_PER_TASK_FOR_POSIX_API \
    _Configure_From_workspace(sizeof(POSIX_API_Control))

  #ifndef CONFIGURE_MAXIMUM_POSIX_MUTEXES
    #define CONFIGURE_MAXIMUM_POSIX_MUTEXES              0
    #define CONFIGURE_MEMORY_FOR_POSIX_MUTEXES(_mutexes) 0
  #else
    #define CONFIGURE_MEMORY_FOR_POSIX_MUTEXES(_mutexes) \
      _Configure_Object_RAM(_mutexes, sizeof(POSIX_Mutex_Control) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES
    #define CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES               0
    #define CONFIGURE_MEMORY_FOR_POSIX_CONDITION_VARIABLES(_condvars) 0
  #else
    #define CONFIGURE_MEMORY_FOR_POSIX_CONDITION_VARIABLES(_condvars) \
        _Configure_Object_RAM(_condvars, \
                            sizeof(POSIX_Condition_variables_Control) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_POSIX_KEYS
    #define CONFIGURE_MAXIMUM_POSIX_KEYS           0
    #define CONFIGURE_MEMORY_FOR_POSIX_KEYS(_keys) 0
  #else
    #define CONFIGURE_MEMORY_FOR_POSIX_KEYS(_keys) \
      (_Configure_Object_RAM(_keys, sizeof(POSIX_Keys_Control) ) \
        + (_keys) * 3 * _Configure_From_workspace(sizeof(void *) * 2))
  #endif

  #ifndef CONFIGURE_MAXIMUM_POSIX_TIMERS
    #define CONFIGURE_MAXIMUM_POSIX_TIMERS             0
    #define CONFIGURE_MEMORY_FOR_POSIX_TIMERS(_timers) 0
  #else
    #define CONFIGURE_MEMORY_FOR_POSIX_TIMERS(_timers) \
      _Configure_Object_RAM(_timers, sizeof(POSIX_Timer_Control) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS
    #define CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS                     0
    #define CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS(_queued_signals) 0
  #else
    #define CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS(_queued_signals) \
      _Configure_From_workspace( \
        (_queued_signals) * (sizeof(POSIX_signals_Siginfo_node)) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES
    #define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES                     0
    #define CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUES(_message_queues) 0
    #define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUE_DESCRIPTORS          0
    #define CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUE_DESCRIPTORS(_fds) 0
  #else
    #define CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUES(_message_queues) \
      _Configure_POSIX_Named_Object_RAM( \
         _message_queues, sizeof(POSIX_Message_queue_Control) )

    /* default to same number */
    #ifndef CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUE_DESCRIPTORS
       #define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUE_DESCRIPTORS \
               CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES
    #endif

    #define CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUE_DESCRIPTORS(_mqueue_fds) \
      _Configure_Object_RAM( \
         _mqueue_fds, sizeof(POSIX_Message_queue_Control_fd) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_POSIX_SEMAPHORES
    #define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES                 0
    #define CONFIGURE_MEMORY_FOR_POSIX_SEMAPHORES(_semaphores) 0
  #else
    #define CONFIGURE_MEMORY_FOR_POSIX_SEMAPHORES(_semaphores) \
      _Configure_POSIX_Named_Object_RAM( \
         _semaphores, sizeof(POSIX_Semaphore_Control) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_POSIX_BARRIERS
    #define CONFIGURE_MAXIMUM_POSIX_BARRIERS               0
    #define CONFIGURE_MEMORY_FOR_POSIX_BARRIERS(_barriers) 0
  #else
    #define CONFIGURE_MEMORY_FOR_POSIX_BARRIERS(_barriers) \
      _Configure_Object_RAM(_barriers, sizeof(POSIX_Barrier_Control) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_POSIX_SPINLOCKS
    #define CONFIGURE_MAXIMUM_POSIX_SPINLOCKS                0
    #define CONFIGURE_MEMORY_FOR_POSIX_SPINLOCKS(_spinlocks) 0
  #else
    #define CONFIGURE_MEMORY_FOR_POSIX_SPINLOCKS(_spinlocks) \
      _Configure_Object_RAM(_spinlocks, sizeof(POSIX_Spinlock_Control) )
  #endif

  #ifndef CONFIGURE_MAXIMUM_POSIX_RWLOCKS
    #define CONFIGURE_MAXIMUM_POSIX_RWLOCKS              0
    #define CONFIGURE_MEMORY_FOR_POSIX_RWLOCKS(_rwlocks) 0
  #else
    #define CONFIGURE_MEMORY_FOR_POSIX_RWLOCKS(_rwlocks) \
      _Configure_Object_RAM(_rwlocks, sizeof(POSIX_RWLock_Control) )
  #endif

  #ifdef CONFIGURE_POSIX_INIT_THREAD_TABLE

    #ifdef CONFIGURE_POSIX_HAS_OWN_INIT_THREAD_TABLE

      /*
       *  The user is defining their own table information and setting the
       *  appropriate variables for the POSIX Initialization Thread Table.
       */

    #else

      #ifndef CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT
        #define CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT   POSIX_Init
      #endif

      #ifndef CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE
        #define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE \
          CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE
      #endif

      #ifdef CONFIGURE_INIT
        posix_initialization_threads_table POSIX_Initialization_threads[] = {
          { CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT, \
              CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE }
        };
      #endif

      #define CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME \
              POSIX_Initialization_threads

      #define CONFIGURE_POSIX_INIT_THREAD_TABLE_SIZE \
        sizeof(CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME) / \
            sizeof(posix_initialization_threads_table)

    #endif    /* CONFIGURE_POSIX_HAS_OWN_INIT_TASK_TABLE */

  #else     /* CONFIGURE_POSIX_INIT_THREAD_TABLE */

    #define CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME NULL
    #define CONFIGURE_POSIX_INIT_THREAD_TABLE_SIZE 0

  #endif

  #define CONFIGURE_MEMORY_FOR_POSIX \
    ( CONFIGURE_MEMORY_FOR_POSIX_MUTEXES( CONFIGURE_MAXIMUM_POSIX_MUTEXES + \
          CONFIGURE_MAXIMUM_GO_CHANNELS + CONFIGURE_GO_INIT_MUTEXES) + \
      CONFIGURE_MEMORY_FOR_POSIX_CONDITION_VARIABLES( \
          CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES + \
          CONFIGURE_MAXIMUM_GO_CHANNELS + CONFIGURE_GO_INIT_CONDITION_VARIABLES) + \
      CONFIGURE_MEMORY_FOR_POSIX_KEYS( CONFIGURE_MAXIMUM_POSIX_KEYS ) + \
      CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS( \
          CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS ) + \
      CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUES( \
          CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES ) + \
      CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUE_DESCRIPTORS( \
          CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUE_DESCRIPTORS ) + \
      CONFIGURE_MEMORY_FOR_POSIX_SEMAPHORES( \
          CONFIGURE_MAXIMUM_POSIX_SEMAPHORES ) + \
      CONFIGURE_MEMORY_FOR_POSIX_BARRIERS(CONFIGURE_MAXIMUM_POSIX_BARRIERS) + \
      CONFIGURE_MEMORY_FOR_POSIX_SPINLOCKS( \
          CONFIGURE_MAXIMUM_POSIX_SPINLOCKS ) + \
      CONFIGURE_MEMORY_FOR_POSIX_RWLOCKS( \
          CONFIGURE_MAXIMUM_POSIX_RWLOCKS ) + \
      CONFIGURE_MEMORY_FOR_POSIX_TIMERS( CONFIGURE_MAXIMUM_POSIX_TIMERS ) \
     )
#else

  #define CONFIGURE_MAXIMUM_POSIX_THREADS         0
  #define CONFIGURE_MEMORY_PER_TASK_FOR_POSIX_API 0
  #define CONFIGURE_MEMORY_FOR_POSIX              0

#endif    /* RTEMS_POSIX_API */

#ifndef CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE
  #define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE    0
#endif

/*
 *  This block of defines are for applications which use GNAT/RTEMS.
 *  GNAT implements each Ada task as a POSIX thread.
 */
#ifdef CONFIGURE_GNAT_RTEMS

  /**
   *  The GNAT run-time needs something less than (10) POSIX mutexes.
   *  We may be able to get by with less but why bother.
   */
  #define CONFIGURE_GNAT_MUTEXES 10

  /**
   *  This is the maximum number of Ada tasks which can be concurrently
   *  in existence.  Twenty (20) are required to run all tests in the
   *  ACATS (formerly ACVC).
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
  #define CONFIGURE_GNAT_MUTEXES           0
  #define CONFIGURE_MAXIMUM_ADA_TASKS      0
  #define CONFIGURE_MAXIMUM_FAKE_ADA_TASKS 0
#endif

#ifdef CONFIGURE_ENABLE_GO

  #ifndef CONFIGURE_MAXIMUM_POSIX_THREADS
    #define CONFIGURE_MAXIMUM_POSIX_THREADS 1
  #endif
  #ifndef CONFIGURE_MAXIMUM_POSIX_MUTEXES
    #define CONFIGURE_MAXIMUM_POSIX_MUTEXES 1
  #endif
  #ifndef CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES
    #define CONFIGURE_MAXIMUM_CONDITION_VARIABLES 1
  #endif

  #define CONFIGURE_GO_INIT_MUTEXES 77
  #define CONFIGURE_GO_INIT_CONDITION_VARIABLES 4

  #ifndef CONFIGURE_MAXIMUM_GOROUTINES
    #define CONFIGURE_MAXIMUM_GOROUTINES 400
  #endif

  #define CONFIGURE_GOROUTINES_TASK_VARIABLES \
    (2 * CONFIGURE_MAXIMUM_GOROUTINES)

  #ifndef CONFIGURE_MAXIMUM_GO_CHANNELS
    #define CONFIGURE_MAXIMUM_GO_CHANNELS 500
  #endif

#else
  #define CONFIGURE_GO_INIT_MUTEXES             0
  #define CONFIGURE_GO_INIT_CONDITION_VARIABLES 0
  #define CONFIGURE_MAXIMUM_GOROUTINES          0
  #define CONFIGURE_GOROUTINES_TASK_VARIABLES   0
  #define CONFIGURE_MAXIMUM_GO_CHANNELS         0
#endif

#ifndef RTEMS_SCHEDSIM
/**
 *  This macro specifies the amount of memory to be reserved for the
 *  Newlib C Library reentrancy structure -- if we are using newlib.
 */

#if (defined(RTEMS_NEWLIB) && !defined(CONFIGURE_DISABLE_NEWLIB_REENTRANCY))
  #include <reent.h>

  #define CONFIGURE_MEMORY_PER_TASK_FOR_NEWLIB \
    _Configure_From_workspace(sizeof(struct _reent))
#else
  #define CONFIGURE_MEMORY_PER_TASK_FOR_NEWLIB 0
#endif

#else
  #define CONFIGURE_MEMORY_PER_TASK_FOR_NEWLIB 0
#endif

/**
 *  This is so we can account for tasks with stacks greater than minimum
 *  size.  This is in bytes.
 */
#ifndef CONFIGURE_EXTRA_TASK_STACKS
  #define CONFIGURE_EXTRA_TASK_STACKS 0
#endif

/*
 *  Calculate the RAM size based on the maximum number of objects configured.
 */

#ifndef CONFIGURE_EXECUTIVE_RAM_SIZE

/**
 *  Account for allocating the following per object
 *    + array of object control structures
 *    + local pointer table -- pointer per object plus a zero'th
 *      entry in the local pointer table.
 */

#define CONFIGURE_MEMORY_FOR_TASKS(_tasks, _number_FP_tasks) \
  ( \
    _Configure_Object_RAM(_tasks, sizeof(Thread_Control)) \
      + _Configure_Max_Objects(_tasks) \
        * ( \
            CONFIGURE_MEMORY_PER_TASK_FOR_CLASSIC_API \
              + CONFIGURE_MEMORY_PER_TASK_FOR_NEWLIB \
              + CONFIGURE_MEMORY_PER_TASK_FOR_POSIX_API \
              + CONFIGURE_MEMORY_PER_TASK_FOR_SCHEDULER \
              + _Configure_From_workspace( \
                (CONFIGURE_MAXIMUM_USER_EXTENSIONS + 1) * sizeof(void *) \
              ) \
          ) \
      + _Configure_Max_Objects(_number_FP_tasks) \
        * _Configure_From_workspace(CONTEXT_FP_SIZE) \
        * (CONTEXT_FP_SIZE != 0) \
  )

/**
 *  This defines the amount of memory configured for the multiprocessing
 *  support required by this application.
 */
#ifdef CONFIGURE_MP_APPLICATION
  #define CONFIGURE_MEMORY_FOR_MP \
    (CONFIGURE_MEMORY_FOR_PROXIES(CONFIGURE_MP_MAXIMUM_PROXIES) + \
     CONFIGURE_MEMORY_FOR_GLOBAL_OBJECTS( \
             CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS) + \
     CONFIGURE_MEMORY_FOR_TASKS(1, 1) \
  )
#else
  #define CONFIGURE_MEMORY_FOR_MP  0
#endif

/**
 *  The following macro is used to calculate the memory allocated by RTEMS
 *  for the message buffers associated with a particular message queue.
 *  There is a fixed amount of overhead per message.
 */
#define CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(_messages, _size) \
    _Configure_From_workspace( \
      (_messages) * ((_size) + sizeof(CORE_message_queue_Buffer_control)))

/**
 *  This macros is set to the amount of memory required for pending message
 *  buffers in bytes.  It should be constructed by adding together a
 *  set of values determined by CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE.
 */
#ifndef CONFIGURE_MESSAGE_BUFFER_MEMORY
  #define CONFIGURE_MESSAGE_BUFFER_MEMORY 0
#endif

/**
 *  This macro is available just in case the confdefs.h file underallocates
 *  memory for a particular application.  This lets the user add some extra
 *  memory in case something broken and underestimates.
 *
 *  It is also possible for cases where confdefs.h overallocates memory,
 *  you could substract memory from the allocated.  The estimate is just
 *  that, an estimate, and assumes worst case alignment and padding on
 *  each allocated element.  So in some cases it could be too conservative.
 *
 *  @note Historically this was used for message buffers.
 */
#ifndef CONFIGURE_MEMORY_OVERHEAD
  #define CONFIGURE_MEMORY_OVERHEAD 0
#endif

/**
 *  On architectures that use Simple Vectored Interrupts, it is RTEMS
 *  responsibility to allocate the vector table.  This avoids reserving
 *  the memory on architectures that use the Programmable Interrupt
 *  Controller Vectored Interrupts.
 */
#if (CPU_SIMPLE_VECTORED_INTERRUPTS == TRUE)
  /*
   *  This is a (hopefully) temporary hack.  On the mips, the number of
   *  vectors is NOT statically defined.  But it has to be statically
   *  defined for this to work.  This is an issue looking for a nice
   *  solution.
   */
  #if defined(__mips__)
    #define CONFIGURE_INTERRUPT_VECTOR_TABLE \
      _Configure_From_workspace( (sizeof(ISR_Handler_entry) * 256))
  #else
    #define CONFIGURE_INTERRUPT_VECTOR_TABLE \
      _Configure_From_workspace( \
        (sizeof(ISR_Handler_entry) * ISR_NUMBER_OF_VECTORS))
  #endif
#else
  #define CONFIGURE_INTERRUPT_VECTOR_TABLE 0
#endif

/**
 *  RTEMS uses one instance of an internal mutex class.  This accounts
 *  for that mutex
 */
#define CONFIGURE_API_MUTEX_MEMORY \
  _Configure_Object_RAM(1, sizeof(API_Mutex_Control))

/**
 *  This calculates the amount of memory reserved for the IDLE tasks.
 *  In an SMP system, each CPU core has its own idle task.
 */
#if defined(RTEMS_SMP)
  #define CONFIGURE_IDLE_TASKS_COUNT CONFIGURE_SMP_MAXIMUM_PROCESSORS
#else
  #define CONFIGURE_IDLE_TASKS_COUNT 1
#endif

/**
 *  This defines the formula used to compute the amount of memory
 *  reserved for IDLE task control structures.
 */
#define CONFIGURE_MEMORY_FOR_IDLE_TASK \
  CONFIGURE_MEMORY_FOR_TASKS(CONFIGURE_IDLE_TASKS_COUNT, 0)

/**
 *  This macro accounts for general RTEMS system overhead.
 */
#define CONFIGURE_MEMORY_FOR_SYSTEM_OVERHEAD \
  ( CONFIGURE_MEMORY_FOR_IDLE_TASK +                /* IDLE and stack */ \
    CONFIGURE_MEMORY_FOR_SCHEDULER +                /* Scheduler */ \
    CONFIGURE_INTERRUPT_VECTOR_TABLE +             /* interrupt vectors */ \
    CONFIGURE_INTERRUPT_STACK_MEMORY +             /* interrupt stack */ \
    CONFIGURE_API_MUTEX_MEMORY                     /* allocation mutex */ \
  )

/**
 *  This macro provides a summation of the various task and thread
 *  requirements.
 */
#define CONFIGURE_TOTAL_TASKS_AND_THREADS \
   (CONFIGURE_TASKS + \
    CONFIGURE_MAXIMUM_POSIX_THREADS + CONFIGURE_MAXIMUM_ADA_TASKS + \
    CONFIGURE_MAXIMUM_GOROUTINES)

/**
 *  This macro reserves the memory required by the statically configured
 *  user extensions.
 */
#define CONFIGURE_MEMORY_FOR_STATIC_EXTENSIONS \
     ((CONFIGURE_NEWLIB_EXTENSION * \
        _Configure_From_workspace( sizeof(User_extensions_Control))) + \
      (CONFIGURE_STACK_CHECKER_EXTENSION * \
        _Configure_From_workspace( sizeof(User_extensions_Control))) \
     )

/**
 *  This macro provides a summation of the memory required by the
 *  Classic API as configured.
 */
#define CONFIGURE_MEMORY_FOR_CLASSIC \
  (CONFIGURE_MEMORY_FOR_TASK_VARIABLES(CONFIGURE_MAXIMUM_TASK_VARIABLES + \
    CONFIGURE_GOROUTINES_TASK_VARIABLES) + \
   CONFIGURE_MEMORY_FOR_TIMERS(CONFIGURE_MAXIMUM_TIMERS + \
    CONFIGURE_TIMER_FOR_SHARED_MEMORY_DRIVER ) + \
   CONFIGURE_MEMORY_FOR_SEMAPHORES(CONFIGURE_SEMAPHORES) + \
   CONFIGURE_MEMORY_FOR_MESSAGE_QUEUES(CONFIGURE_MAXIMUM_MESSAGE_QUEUES) + \
   CONFIGURE_MEMORY_FOR_PARTITIONS(CONFIGURE_MAXIMUM_PARTITIONS) + \
   CONFIGURE_MEMORY_FOR_REGIONS( CONFIGURE_MAXIMUM_REGIONS ) + \
   CONFIGURE_MEMORY_FOR_PORTS(CONFIGURE_MAXIMUM_PORTS) + \
   CONFIGURE_MEMORY_FOR_PERIODS(CONFIGURE_MAXIMUM_PERIODS) + \
   CONFIGURE_MEMORY_FOR_BARRIERS(CONFIGURE_BARRIERS) + \
   CONFIGURE_MEMORY_FOR_USER_EXTENSIONS(CONFIGURE_MAXIMUM_USER_EXTENSIONS) \
  )

#if defined(RTEMS_SMP)
  #define CONFIGURE_MEMORY_FOR_SMP \
     (CONFIGURE_SMP_MAXIMUM_PROCESSORS * \
      _Configure_From_workspace( CONFIGURE_INTERRUPT_STACK_SIZE ) \
     )
#else
  #define CONFIGURE_MEMORY_FOR_SMP 0
#endif

/**
 *  This calculates the memory required for the executive workspace.
 */
#define CONFIGURE_EXECUTIVE_RAM_SIZE \
(( \
   CONFIGURE_MEMORY_FOR_SYSTEM_OVERHEAD + \
   CONFIGURE_MEMORY_FOR_TASKS( \
     CONFIGURE_TOTAL_TASKS_AND_THREADS, CONFIGURE_TOTAL_TASKS_AND_THREADS) + \
   CONFIGURE_MEMORY_FOR_CLASSIC + \
   CONFIGURE_MEMORY_FOR_POSIX + \
   CONFIGURE_MEMORY_FOR_STATIC_EXTENSIONS + \
   CONFIGURE_MEMORY_FOR_MP + \
   CONFIGURE_MEMORY_FOR_SMP + \
   CONFIGURE_MESSAGE_BUFFER_MEMORY + \
   (CONFIGURE_MEMORY_OVERHEAD * 1024) \
) & ~0x7)

/*
 *  Now account for any extra memory that initialization tasks or threads
 *  may have requested.
 */

/**
 *  This accounts for any extra memory required by the Classic API
 *  Initialization Task.
 */
#if (CONFIGURE_INIT_TASK_STACK_SIZE > CONFIGURE_MINIMUM_TASK_STACK_SIZE)
  #define CONFIGURE_INITIALIZATION_THREADS_STACKS_CLASSIC_PART \
      (CONFIGURE_INIT_TASK_STACK_SIZE - CONFIGURE_MINIMUM_TASK_STACK_SIZE)
#else
  #define CONFIGURE_INITIALIZATION_THREADS_STACKS_CLASSIC_PART 0
#endif

/**
 *  This accounts for any extra memory required by the POSIX API
 *  Initialization Thread.
 */
#if defined(RTEMS_POSIX_API) && \
    (CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE > \
      CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE)
  #define CONFIGURE_INITIALIZATION_THREADS_STACKS_POSIX_PART \
    (CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE - \
      CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE)
#else
  #define CONFIGURE_INITIALIZATION_THREADS_STACKS_POSIX_PART 0
#endif

/**
 *  This macro provides a summation of the various initialization task
 *  and thread stack requirements.
 */
#define CONFIGURE_INITIALIZATION_THREADS_EXTRA_STACKS \
    (CONFIGURE_INITIALIZATION_THREADS_STACKS_CLASSIC_PART + \
    CONFIGURE_INITIALIZATION_THREADS_STACKS_POSIX_PART)

#define CONFIGURE_IDLE_TASKS_STACK \
  (CONFIGURE_IDLE_TASKS_COUNT * \
    _Configure_From_stackspace( CONFIGURE_IDLE_TASK_STACK_SIZE ) )

#define CONFIGURE_TASKS_STACK \
  (_Configure_Max_Objects( CONFIGURE_TASKS ) * \
    _Configure_From_stackspace( CONFIGURE_MINIMUM_TASK_STACK_SIZE ) )

#define CONFIGURE_POSIX_THREADS_STACK \
  (_Configure_Max_Objects( CONFIGURE_MAXIMUM_POSIX_THREADS ) * \
    _Configure_From_stackspace( CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE ) )

#define CONFIGURE_GOROUTINES_STACK \
  (_Configure_Max_Objects( CONFIGURE_MAXIMUM_GOROUTINES ) * \
    _Configure_From_stackspace( CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE ) )

#define CONFIGURE_ADA_TASKS_STACK \
  (_Configure_Max_Objects( CONFIGURE_MAXIMUM_ADA_TASKS ) * \
    _Configure_From_stackspace( CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE ) )

#else /* CONFIGURE_EXECUTIVE_RAM_SIZE */

#define CONFIGURE_IDLE_TASKS_STACK 0
#define CONFIGURE_INITIALIZATION_THREADS_EXTRA_STACKS 0
#define CONFIGURE_TASKS_STACK 0
#define CONFIGURE_POSIX_THREADS_STACK 0
#define CONFIGURE_GOROUTINES_STACK 0
#define CONFIGURE_ADA_TASKS_STACK 0

#if CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK != 0
  #error "CONFIGURE_EXECUTIVE_RAM_SIZE defined with request for CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK"
#endif

#if CONFIGURE_EXTRA_TASK_STACKS != 0
  #error "CONFIGURE_EXECUTIVE_RAM_SIZE defined with request for CONFIGURE_EXTRA_TASK_STACKS"
#endif

#endif /* CONFIGURE_EXECUTIVE_RAM_SIZE */

#define CONFIGURE_STACK_SPACE_SIZE \
  ( \
    CONFIGURE_IDLE_TASKS_STACK + \
    CONFIGURE_INITIALIZATION_THREADS_EXTRA_STACKS + \
    CONFIGURE_TASKS_STACK + \
    CONFIGURE_POSIX_THREADS_STACK + \
    CONFIGURE_GOROUTINES_STACK + \
    CONFIGURE_ADA_TASKS_STACK + \
    CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK + \
    CONFIGURE_LIBBLOCK_TASK_EXTRA_STACKS + \
    CONFIGURE_EXTRA_TASK_STACKS \
  )

#ifdef CONFIGURE_INIT
  /**
   *  This is the Classic API Configuration Table.
   */
  rtems_api_configuration_table Configuration_RTEMS_API = {
    CONFIGURE_TASKS,
    CONFIGURE_NOTEPADS_ENABLED,
    CONFIGURE_MAXIMUM_TIMERS + CONFIGURE_TIMER_FOR_SHARED_MEMORY_DRIVER,
    CONFIGURE_SEMAPHORES,
    CONFIGURE_MAXIMUM_MESSAGE_QUEUES,
    CONFIGURE_MAXIMUM_PARTITIONS,
    CONFIGURE_MAXIMUM_REGIONS,
    CONFIGURE_MAXIMUM_PORTS,
    CONFIGURE_MAXIMUM_PERIODS,
    CONFIGURE_BARRIERS,
    CONFIGURE_INIT_TASK_TABLE_SIZE,
    CONFIGURE_INIT_TASK_TABLE
  };

  #ifdef RTEMS_POSIX_API
    /**
     *  This is the POSIX API Configuration Table.
     */
    posix_api_configuration_table Configuration_POSIX_API = {
      CONFIGURE_MAXIMUM_POSIX_THREADS + CONFIGURE_MAXIMUM_ADA_TASKS +
        CONFIGURE_MAXIMUM_GOROUTINES,
      CONFIGURE_MAXIMUM_POSIX_MUTEXES + CONFIGURE_GNAT_MUTEXES +
        CONFIGURE_MAXIMUM_ADA_TASKS + CONFIGURE_MAXIMUM_FAKE_ADA_TASKS +
        CONFIGURE_GO_INIT_MUTEXES + CONFIGURE_MAXIMUM_GO_CHANNELS,
      CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES +
        CONFIGURE_MAXIMUM_ADA_TASKS + CONFIGURE_MAXIMUM_FAKE_ADA_TASKS +
        CONFIGURE_GO_INIT_CONDITION_VARIABLES + CONFIGURE_MAXIMUM_GO_CHANNELS,
      CONFIGURE_MAXIMUM_POSIX_KEYS,
      CONFIGURE_MAXIMUM_POSIX_TIMERS,
      CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS,
      CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES,
      CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUE_DESCRIPTORS,
      CONFIGURE_MAXIMUM_POSIX_SEMAPHORES,
      CONFIGURE_MAXIMUM_POSIX_BARRIERS,
      CONFIGURE_MAXIMUM_POSIX_RWLOCKS,
      CONFIGURE_MAXIMUM_POSIX_SPINLOCKS,
      CONFIGURE_POSIX_INIT_THREAD_TABLE_SIZE,
      CONFIGURE_POSIX_INIT_THREAD_TABLE_NAME
    };
  #endif

  /** This variable specifies the minimum stack size for tasks in an RTEMS
   *  application.
   *
   *  @note This is left as a simple uint32_t so it can be externed as
   *        needed without requring being high enough logical to
   *        include the full configuration table.
   */
  uint32_t rtems_minimum_stack_size =
    CONFIGURE_MINIMUM_TASK_STACK_SIZE;

  /** This variable specifies the maximum priority value that
   *  a task may have.  This must be a power of 2 between 4
   *  and 256 and is specified in terms of Classic API
   *  priority values.
   *
   *  @note This is left as a simple uint8_t so it can be externed as
   *        needed without requring being high enough logical to
   *        include the full configuration table.
   */
  uint8_t rtems_maximum_priority = CONFIGURE_MAXIMUM_PRIORITY;

  /**
   *  This is the primary Configuration Table for this application.
   */
  rtems_configuration_table Configuration = {
    NULL,                                     /* filled in by BSP */
    CONFIGURE_EXECUTIVE_RAM_SIZE,             /* required RTEMS workspace */
    CONFIGURE_STACK_SPACE_SIZE,               /* required stack space */
    CONFIGURE_MAXIMUM_USER_EXTENSIONS,        /* maximum dynamic extensions */
    CONFIGURE_MICROSECONDS_PER_TICK,          /* microseconds per clock tick */
    1000 * CONFIGURE_MICROSECONDS_PER_TICK,   /* nanoseconds per clock tick */
    CONFIGURE_TICKS_PER_TIMESLICE,            /* ticks per timeslice quantum */
    CONFIGURE_IDLE_TASK_BODY,                 /* user's IDLE task */
    CONFIGURE_IDLE_TASK_STACK_SIZE,           /* IDLE task stack size */
    CONFIGURE_INTERRUPT_STACK_SIZE,           /* interrupt stack size */
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
    CONFIGURE_MAXIMUM_DRIVERS,                /* maximum device drivers */
    CONFIGURE_NUMBER_OF_DRIVERS,              /* static device drivers */
    Device_drivers,                           /* pointer to driver table */
    CONFIGURE_NUMBER_OF_INITIAL_EXTENSIONS,   /* number of static extensions */
    CONFIGURE_INITIAL_EXTENSION_TABLE,        /* pointer to static extensions */
    #if defined(RTEMS_MULTIPROCESSING)
      CONFIGURE_MULTIPROCESSING_TABLE,        /* pointer to MP config table */
    #endif
  };
#endif

#endif /* CONFIGURE_HAS_OWN_CONFIGURATION_TABLE */

#if defined(RTEMS_SMP)
  /**
   *  Instantiate the variable which specifies the number of CPUs
   *  in an SMP configuration.
   */
  #if defined(CONFIGURE_INIT)
    uint32_t rtems_configuration_smp_maximum_processors = \
        CONFIGURE_SMP_MAXIMUM_PROCESSORS;
  #else
    extern uint32_t rtems_configuration_smp_maximum_processors;
  #endif
 /*
  * Instantiate the Per CPU information based upon the user configuration.
  */
 #if defined(CONFIGURE_INIT)
   Per_CPU_Control _Per_CPU_Information[CONFIGURE_SMP_MAXIMUM_PROCESSORS];
   Per_CPU_Control *_Per_CPU_Information_p[CONFIGURE_SMP_MAXIMUM_PROCESSORS];
 #endif

#endif

/*
 *  If the user has configured a set of Classic API Initialization Tasks,
 *  then we need to install the code that runs that loop.
 */
#ifdef CONFIGURE_INIT
  #if defined(CONFIGURE_RTEMS_INIT_TASKS_TABLE) || \
      defined(CONFIGURE_HAS_OWN_INIT_TASK_TABLE)
    void (*_RTEMS_tasks_Initialize_user_tasks_p)(void) =
              _RTEMS_tasks_Initialize_user_tasks_body;
  #else
    void (*_RTEMS_tasks_Initialize_user_tasks_p)(void) = NULL;
  #endif
#endif

/*
 *  If the user has configured a set of POSIX Initialization Threads,
 *  then we need to install the code that runs that loop.
 */
#ifdef RTEMS_POSIX_API
  #ifdef CONFIGURE_INIT
    #if defined(CONFIGURE_POSIX_INIT_THREAD_TABLE) || \
        defined(CONFIGURE_POSIX_HAS_OWN_INIT_THREAD_TABLE)
      void (*_POSIX_Threads_Initialize_user_threads_p)(void) =
                _POSIX_Threads_Initialize_user_threads_body;
    #else
      void (*_POSIX_Threads_Initialize_user_threads_p)(void) = NULL;
    #endif
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

#if defined(CONFIGURE_CONFDEFS_DEBUG) && defined(CONFIGURE_INIT)
  /**
   *  This is a debug mechanism, so if you need to, the executable will
   *  have a structure with various partial values.  Add to this as you
   *  need to.  Viewing this structure in gdb combined with dumping
   *  the Configuration structures generated should help a lot in tracing
   *  down errors and analyzing where over and under allocations are.
   */
  typedef struct {
    uint32_t SYSTEM_OVERHEAD;
    uint32_t STATIC_EXTENSIONS;
    uint32_t INITIALIZATION_THREADS_STACKS;

    uint32_t PER_INTEGER_TASK;
    uint32_t FP_OVERHEAD;
    uint32_t CLASSIC;
    uint32_t POSIX;

    /* System overhead pieces */
    uint32_t INTERRUPT_VECTOR_TABLE;
    uint32_t INTERRUPT_STACK_MEMORY;
    uint32_t MEMORY_FOR_IDLE_TASK;
    uint32_t MEMORY_FOR_SCHEDULER;
    uint32_t MEMORY_PER_TASK_FOR_SCHEDULER;

    /* Classic API Pieces */
    uint32_t CLASSIC_TASKS;
    uint32_t TASK_VARIABLES;
    uint32_t TIMERS;
    uint32_t SEMAPHORES;
    uint32_t MESSAGE_QUEUES;
    uint32_t PARTITIONS;
    uint32_t REGIONS;
    uint32_t PORTS;
    uint32_t PERIODS;
    uint32_t BARRIERS;
    uint32_t USER_EXTENSIONS;

#ifdef RTEMS_POSIX_API
    /* POSIX API Pieces */
    uint32_t POSIX_MUTEXES;
    uint32_t POSIX_CONDITION_VARIABLES;
    uint32_t POSIX_KEYS;
    uint32_t POSIX_TIMERS;
    uint32_t POSIX_QUEUED_SIGNALS;
    uint32_t POSIX_MESSAGE_QUEUES;
    uint32_t POSIX_SEMAPHORES;
    uint32_t POSIX_BARRIERS;
    uint32_t POSIX_SPINLOCKS;
    uint32_t POSIX_RWLOCKS;
#endif

    /* Stack space sizes */
    uint32_t IDLE_TASKS_STACK;
    uint32_t INITIALIZATION_THREADS_EXTRA_STACKS;
    uint32_t TASKS_STACK;
    uint32_t POSIX_THREADS_STACK;
    uint32_t GOROUTINES_STACK;
    uint32_t ADA_TASKS_STACK;
    uint32_t EXTRA_MPCI_RECEIVE_SERVER_STACK;
    uint32_t EXTRA_TASK_STACKS;
  } Configuration_Debug_t;

  Configuration_Debug_t Configuration_Memory_Debug = {
    /* General Information */
    CONFIGURE_MEMORY_FOR_SYSTEM_OVERHEAD,
    CONFIGURE_MEMORY_FOR_STATIC_EXTENSIONS,
    CONFIGURE_INITIALIZATION_THREADS_EXTRA_STACKS,
    CONFIGURE_MEMORY_FOR_TASKS(1, 0),
    CONFIGURE_MEMORY_FOR_TASKS(0, 1),
    CONFIGURE_MEMORY_FOR_CLASSIC,
    CONFIGURE_MEMORY_FOR_POSIX,

    /* System overhead pieces */
    CONFIGURE_INTERRUPT_VECTOR_TABLE,
    CONFIGURE_INTERRUPT_STACK_MEMORY,
    CONFIGURE_MEMORY_FOR_IDLE_TASK,
    CONFIGURE_MEMORY_FOR_SCHEDULER,
    CONFIGURE_MEMORY_PER_TASK_FOR_SCHEDULER,

    /* Classic API Pieces */
    CONFIGURE_MEMORY_FOR_TASKS(CONFIGURE_MAXIMUM_TASKS, 0),
    CONFIGURE_MEMORY_FOR_TASK_VARIABLES(CONFIGURE_MAXIMUM_TASK_VARIABLES +
      CONFIGURE_GOROUTINES_TASK_VARIABLES),
    CONFIGURE_MEMORY_FOR_TIMERS(CONFIGURE_MAXIMUM_TIMERS),
    CONFIGURE_MEMORY_FOR_SEMAPHORES(CONFIGURE_SEMAPHORES),
    CONFIGURE_MEMORY_FOR_MESSAGE_QUEUES(CONFIGURE_MAXIMUM_MESSAGE_QUEUES),
    CONFIGURE_MEMORY_FOR_PARTITIONS(CONFIGURE_MAXIMUM_PARTITIONS),
    CONFIGURE_MEMORY_FOR_REGIONS( CONFIGURE_MAXIMUM_REGIONS ),
    CONFIGURE_MEMORY_FOR_PORTS(CONFIGURE_MAXIMUM_PORTS),
    CONFIGURE_MEMORY_FOR_PERIODS(CONFIGURE_MAXIMUM_PERIODS),
    CONFIGURE_MEMORY_FOR_BARRIERS(CONFIGURE_BARRIERS),
    CONFIGURE_MEMORY_FOR_USER_EXTENSIONS(CONFIGURE_MAXIMUM_USER_EXTENSIONS),

#ifdef RTEMS_POSIX_API
    /* POSIX API Pieces */
    CONFIGURE_MEMORY_FOR_POSIX_MUTEXES( CONFIGURE_MAXIMUM_POSIX_MUTEXES +
      CONFIGURE_MAXIMUM_GO_CHANNELS + CONFIGURE_GO_INIT_MUTEXES),
    CONFIGURE_MEMORY_FOR_POSIX_CONDITION_VARIABLES(
      CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES +
      CONFIGURE_MAXIMUM_GO_CHANNELS + CONFIGURE_GO_INIT_CONDITION_VARIABLES),
    CONFIGURE_MEMORY_FOR_POSIX_KEYS( CONFIGURE_MAXIMUM_POSIX_KEYS ),
    CONFIGURE_MEMORY_FOR_POSIX_QUEUED_SIGNALS(
      CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS ),
    CONFIGURE_MEMORY_FOR_POSIX_MESSAGE_QUEUES(
      CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES ),
    CONFIGURE_MEMORY_FOR_POSIX_SEMAPHORES( CONFIGURE_MAXIMUM_POSIX_SEMAPHORES ),
    CONFIGURE_MEMORY_FOR_POSIX_BARRIERS( CONFIGURE_MAXIMUM_POSIX_BARRIERS ),
    CONFIGURE_MEMORY_FOR_POSIX_SPINLOCKS( CONFIGURE_MAXIMUM_POSIX_SPINLOCKS ),
    CONFIGURE_MEMORY_FOR_POSIX_RWLOCKS( CONFIGURE_MAXIMUM_POSIX_RWLOCKS ),
    CONFIGURE_MEMORY_FOR_POSIX_TIMERS( CONFIGURE_MAXIMUM_POSIX_TIMERS ),
#endif

    /* Stack space sizes */
    CONFIGURE_IDLE_TASKS_STACK,
    CONFIGURE_INITIALIZATION_THREADS_EXTRA_STACKS,
    CONFIGURE_TASKS_STACK,
    CONFIGURE_POSIX_THREADS_STACK,
    CONFIGURE_GOROUTINES_STACK,
    CONFIGURE_ADA_TASKS_STACK,
    CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK,
    CONFIGURE_EXTRA_TASK_STACKS
  };
#endif

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
      (CONFIGURE_MAXIMUM_ADA_TASKS == 0) && \
      (CONFIGURE_MAXIMUM_GOROUTINES == 0)
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

/*
 *  If an attempt was made to configure POSIX objects and
 *  the POSIX API was not configured into RTEMS, error out.
 */
#if !defined(RTEMS_POSIX_API)
  #if ((CONFIGURE_MAXIMUM_POSIX_THREADS != 0) || \
       (CONFIGURE_MAXIMUM_POSIX_MUTEXES != 0) || \
       (CONFIGURE_MAXIMUM_POSIX_CONDITION_VARIABLES != 0) || \
       (CONFIGURE_MAXIMUM_POSIX_KEYS != 0) || \
       (CONFIGURE_MAXIMUM_POSIX_TIMERS != 0) || \
       (CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS != 0) || \
       (CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES != 0) || \
       (CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUE_DESCRIPTORS != 0) || \
       (CONFIGURE_MAXIMUM_POSIX_SEMAPHORES != 0) || \
       (CONFIGURE_MAXIMUM_POSIX_BARRIERS != 0) || \
       (CONFIGURE_MAXIMUM_POSIX_SPINLOCKS != 0) || \
       (CONFIGURE_MAXIMUM_POSIX_RWLOCKS != 0) || \
      defined(CONFIGURE_POSIX_INIT_THREAD_TABLE))
  #error "CONFIGURATION ERROR: POSIX API support not configured!!"
  #endif
#endif

#ifndef RTEMS_SCHEDSIM
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
#if !defined(CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE)
  #if !defined(CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER) && \
      !defined(CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER) && \
      !defined(CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER)
    #error "CONFIGURATION ERROR: Do you want the clock driver or not?!?"
   #endif
#endif
#endif

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

/*
 *  If you have fewer POSIX Message Queue Descriptors than actual
 *  POSIX Message Queues, then you will not be able to open all the
 *  queues.
 */
#if (CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUE_DESCRIPTORS < \
     CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES)
  #error "Fewer POSIX Message Queue descriptors than Queues!"
#endif

#endif
/* end of include file */
