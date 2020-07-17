/**
 * @defgroup Block Device Cache Configuration Block Device Cache Configuration
 *
 * This section describes configuration options related to the Block Device
 * Cache (bdbuf).
 *
 * @{
 */

/**
 * @brief In case this configuration option is defined, then the Block Device
 *   Cache is initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * Each option of the Block Device Cache (bdbuf) configuration can be
 * explicitly set by the user with the configuration options below.  The Block
 * Device Cache is used for example by the RFS and DOSFS filesystems.
 */
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

/**
 * @brief The value of this configuration option defines the maximum size of a
 *   buffer in bytes.
 *
 * This configuration option is an integer define.
 *
 * The default value is 4096.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be an integral multiple of :ref:`CONFIGURE_BDBUF_BUFFER_MIN_SIZE`.
 */
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE

/**
 * @brief The value of this configuration option defines the minimum size of a
 *   buffer in bytes.
 *
 * This configuration option is an integer define.
 *
 * The default value is 512.
 *
 * The value of this configuration option shall be greater than or equal to 0 and
 * less than or equal to ``UINT32_MAX``.
 */
#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE

/**
 * @brief The value of this configuration option defines the size of the cache
 *   memory in bytes.
 *
 * This configuration option is an integer define.
 *
 * The default value is 32768.
 *
 * The value of this configuration option shall be greater than or equal to 0 and
 * less than or equal to ``SIZE_MAX``.
 */
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE

/**
 * @brief The value of this configuration option defines the maximum blocks per
 *   read-ahead request.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall be greater than or equal to 0 and
 * less than or equal to ``UINT32_MAX``.
 *
 * A value of 0 disables the read-ahead task (default).  The read-ahead task
 * will issue speculative read transfers if a sequential access pattern is
 * detected.  This can improve the performance on some systems.
 */
#define CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS

/**
 * @brief The value of this configuration option defines the maximum blocks per
 *   write request.
 *
 * This configuration option is an integer define.
 *
 * The default value is 16.
 *
 * The value of this configuration option shall be greater than or equal to 0 and
 * less than or equal to ``UINT32_MAX``.
 */
#define CONFIGURE_BDBUF_MAX_WRITE_BLOCKS

/**
 * @brief The value of this configuration option defines the read-ahead task
 *   priority.
 *
 * This configuration option is an integer define.
 *
 * The default value is 15.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be a valid Classic API task priority.  The set of valid task
 *   priorities is scheduler-specific.
 *
 * * It shall be a valid Classic API task priority.  The set of valid task
 *   priorities is scheduler-specific.
 */
#define CONFIGURE_BDBUF_READ_AHEAD_TASK_PRIORITY

/**
 * @brief The value of this configuration option defines the task stack size of
 *   the Block Device Cache tasks in bytes.
 *
 * This configuration option is an integer define.
 *
 * The default value is ``RTEMS_MINIMUM_STACK_SIZE``.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to
 *   :ref:`CONFIGURE_MINIMUM_TASK_STACK_SIZE`.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 */
#define CONFIGURE_BDBUF_TASK_STACK_SIZE

/**
 * @brief The value of this configuration option defines the swapout task
 *   maximum block hold time in milliseconds.
 *
 * This configuration option is an integer define.
 *
 * The default value is 1000.
 *
 * The value of this configuration option shall be greater than or equal to 0 and
 * less than or equal to ``UINT32_MAX``.
 */
#define CONFIGURE_SWAPOUT_BLOCK_HOLD

/**
 * @brief The value of this configuration option defines the swapout task swap
 *   period in milliseconds.
 *
 * This configuration option is an integer define.
 *
 * The default value is 250.
 *
 * The value of this configuration option shall be greater than or equal to 0 and
 * less than or equal to ``UINT32_MAX``.
 */
#define CONFIGURE_SWAPOUT_SWAP_PERIOD

/**
 * @brief The value of this configuration option defines the swapout task
 *   priority.
 *
 * This configuration option is an integer define.
 *
 * The default value is 15.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be a valid Classic API task priority.  The set of valid task
 *   priorities is scheduler-specific.
 *
 * * It shall be a valid Classic API task priority.  The set of valid task
 *   priorities is scheduler-specific.
 */
#define CONFIGURE_SWAPOUT_TASK_PRIORITY

/**
 * @brief The value of this configuration option defines the swapout worker
 *   task priority.
 *
 * This configuration option is an integer define.
 *
 * The default value is 15.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be a valid Classic API task priority.  The set of valid task
 *   priorities is scheduler-specific.
 *
 * * It shall be a valid Classic API task priority.  The set of valid task
 *   priorities is scheduler-specific.
 */
#define CONFIGURE_SWAPOUT_WORKER_TASK_PRIORITY

/**
 * @brief The value of this configuration option defines the swapout worker
 *   task count.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall be greater than or equal to 0 and
 * less than or equal to ``UINT32_MAX``.
 */
#define CONFIGURE_SWAPOUT_WORKER_TASKS

/** @} */

/**
 * @defgroup BSP Related Configuration Options BSP Related Configuration Options
 *
 * This section describes configuration options related to the BSP.  Some
 * configuration options may have a BSP-specific setting which is defined by
 * ``<bsp.h>``.  The BSP-specific settings can be disabled by the
 * :ref:`CONFIGURE_DISABLE_BSP_SETTINGS` configuration option.
 *
 * @{
 */

/**
 * @brief If
 *
 * @brief * this configuration option is defined by the BSP
 *
 * @brief * and :ref:`CONFIGURE_DISABLE_BSP_SETTINGS` is undefined,
 *
 * @brief then the value of this configuration option defines the default value
 *   of :ref:`CONFIGURE_IDLE_TASK_BODY`.
 *
 * This configuration option is an initializer define.
 *
 * The default value is BSP-specific.
 *
 * The value of this configuration option shall be defined to a valid function
 * pointer of the type ``void *( *idle_body )( uintptr_t )``.
 *
 * As it has knowledge of the specific CPU model, system controller logic, and
 * peripheral buses, a BSP-specific IDLE task may be capable of turning
 * components off to save power during extended periods of no task activity.
 */
#define BSP_IDLE_TASK_BODY

/**
 * @brief If
 *
 * @brief * this configuration option is defined by the BSP
 *
 * @brief * and :ref:`CONFIGURE_DISABLE_BSP_SETTINGS` is undefined,
 *
 * @brief then the value of this configuration option defines the default value
 *   of :ref:`CONFIGURE_IDLE_TASK_SIZE`.
 *
 * This configuration option is an integer define.
 *
 * The default value is BSP-specific.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to a BSP-specific and application-specific
 *   minimum value.
 *
 * * It shall be small enough so that the IDLE task stack area calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``size_t``.
 *
 * * It shall be greater than or equal to a BSP-specific and application-specific
 *   minimum value.
 *
 * * It shall be small enough so that the IDLE task stack area calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``size_t``.
 */
#define BSP_IDLE_TASK_STACK_SIZE

/**
 * @brief If
 *
 * @brief * this configuration option is defined by the BSP
 *
 * @brief * and :ref:`CONFIGURE_DISABLE_BSP_SETTINGS` is undefined,
 *
 * @brief then the value of this configuration option is used to initialize the
 *   table of initial user extensions.
 *
 * This configuration option is an initializer define.
 *
 * The default value is BSP-specific.
 *
 * The value of this configuration option shall be a list of initializers for
 * structures of type :c:type:`rtems_extensions_table`.
 *
 * The value of this configuration option is placed after the entries of all
 * other initial user extensions.
 */
#define BSP_INITIAL_EXTENSION

/**
 * @brief If
 *
 * @brief * this configuration option is defined by the BSP
 *
 * @brief * and :ref:`CONFIGURE_DISABLE_BSP_SETTINGS` is undefined,
 *
 * @brief then the value of this configuration option defines the default value
 *   of :ref:`CONFIGURE_INTERRUPT_STACK_SIZE`.
 *
 * This configuration option is an integer define.
 *
 * The default value is BSP-specific.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to a BSP-specific and application-specific
 *   minimum value.
 *
 * * It shall be small enough so that the interrupt stack area calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``size_t``.
 *
 * * It shall be aligned according to ``CPU_INTERRUPT_STACK_ALIGNMENT``.
 *
 * * It shall be greater than or equal to a BSP-specific and application-specific
 *   minimum value.
 *
 * * It shall be small enough so that the interrupt stack area calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``size_t``.
 *
 * * It shall be aligned according to ``CPU_INTERRUPT_STACK_ALIGNMENT``.
 */
#define BSP_INTERRUPT_STACK_SIZE

/**
 * @brief If
 *
 * @brief * this configuration option is defined by the BSP
 *
 * @brief * and :ref:`CONFIGURE_DISABLE_BSP_SETTINGS` is undefined,
 *
 * @brief then the value of this configuration option is used to initialize the
 *   table of initial user extensions.
 *
 * This configuration option is an initializer define.
 *
 * The default value is BSP-specific.
 *
 * The value of this configuration option shall be a list of initializers for
 * structures of type :c:type:`rtems_extensions_table`.
 *
 * The value of this configuration option is placed before the entries of all
 * other initial user extensions (including
 * :ref:`CONFIGURE_APPLICATION_PREREQUISITE_DRIVERS`).
 */
#define CONFIGURE_BSP_PREREQUISITE_DRIVERS

/**
 * @brief In case this configuration option is defined, then the following BSP
 *   related configuration options are undefined:
 *
 * @brief * :ref:`BSP_IDLE_TASK_BODY`
 *
 * @brief * :ref:`BSP_IDLE_TASK_STACK_SIZE`
 *
 * @brief * :ref:`BSP_INITIAL_EXTENSION`
 *
 * @brief * :ref:`BSP_INTERRUPT_STACK_SIZE`
 *
 * @brief * :ref:`CONFIGURE_BSP_PREREQUISITE_DRIVERS`
 *
 * @brief * :ref:`CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK`
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 */
#define CONFIGURE_DISABLE_BSP_SETTINGS

/**
 * @brief If
 *
 * @brief * this configuration option is defined by the BSP
 *
 * @brief * and :ref:`CONFIGURE_DISABLE_BSP_SETTINGS` is undefined,
 *
 * @brief then not all memory is made available to the C Program Heap
 *   immediately at system initialization time.  When :c:func:`malloc()` or
 *   other standard memory allocation functions are unable to allocate memory,
 *   they will call the BSP supplied :c:func:`sbrk()` function to obtain more
 *   memory.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This option should not be defined by the application. Only the BSP knows how
 * it allocates memory to the C Program Heap.
 */
#define CONFIGURE_MALLOC_BSP_SUPPORTS_SBRK

/** @} */

/**
 * @defgroup Classic API Configuration Classic API Configuration
 *
 * This section describes configuration options related to the Classic API.
 *
 * @{
 */

/**
 * @brief The value of this configuration option defines the maximum number of
 *   Classic API Barriers that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.
 */
#define CONFIGURE_MAXIMUM_BARRIERS

/**
 * @brief The value of this configuration option defines the maximum number of
 *   Classic API Message Queues that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.  You have to account for the memory used to
 * store the messages of each message queue, see
 * :ref:`CONFIGURE_MESSAGE_BUFFER_MEMORY`.
 */
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES

/**
 * @brief The value of this configuration option defines the maximum number of
 *   Classic API Partitions that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.
 */
#define CONFIGURE_MAXIMUM_PARTITIONS

/**
 * @brief The value of this configuration option defines the maximum number of
 *   Classic API Periods that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.
 */
#define CONFIGURE_MAXIMUM_PERIODS

/**
 * @brief The value of this configuration option defines the maximum number of
 *   Classic API Ports that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.
 */
#define CONFIGURE_MAXIMUM_PORTS

/**
 * @brief The value of this configuration option defines the maximum number of
 *   Classic API Regions that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.
 */
#define CONFIGURE_MAXIMUM_REGIONS

/**
 * @brief The value of this configuration option defines the maximum number of
 *   Classic API Semaphore that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.
 *
 * In SMP configurations, the size of a Semaphore Control Block depends on the
 * scheduler count (see :ref:`ConfigurationSchedulerTable`).  The semaphores
 * using the :ref:`MrsP` need a ceiling priority per scheduler.
 */
#define CONFIGURE_MAXIMUM_SEMAPHORES

/**
 * @brief The value of this configuration option defines the maximum number of
 *   Classic API Tasks that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.
 *
 * The calculations for the required memory in the RTEMS Workspace for tasks
 * assume that each task has a minimum stack size and has floating point
 * support enabled.  The configuration parameter
 * ``CONFIGURE_EXTRA_TASK_STACKS`` is used to specify task stack requirements
 * *ABOVE* the minimum size required.  See :ref:`Reserve Task/Thread Stack
 * Memory Above Minimum` for more information about
 * ``CONFIGURE_EXTRA_TASK_STACKS``.
 *
 * The maximum number of POSIX threads is specified by
 * :ref:`CONFIGURE_MAXIMUM_POSIX_THREADS`.
 *
 * A future enhancement to ``<rtems/confdefs.h>`` could be to eliminate the
 * assumption that all tasks have floating point enabled. This would require
 * the addition of a new configuration parameter to specify the number of tasks
 * which enable floating point support.
 */
#define CONFIGURE_MAXIMUM_TASKS

/**
 * @brief The value of this configuration option defines the maximum number of
 *   Classic API Timers that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.
 */
#define CONFIGURE_MAXIMUM_TIMERS

/**
 * @brief The value of this configuration option defines the maximum number of
 *   Classic API User Extensions that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * This object class cannot be configured in unlimited allocation mode.
 */
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS

/** @} */

/**
 * @defgroup Classic API Initialization Task Configuration Classic API Initialization Task Configuration
 *
 * This section describes configuration options related to the Classic API
 * initialization task.
 *
 * @{
 */

/**
 * @brief The value of this configuration option defines task argument of the
 *   Classic API initialization task.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall be a valid integer of type
 * ``rtems_task_argument``.
 */
#define CONFIGURE_INIT_TASK_ARGUMENTS

/**
 * @brief The value of this configuration option defines the task attributes of
 *   the Classic API initialization task.
 *
 * This configuration option is an integer define.
 *
 * The default value is ``RTEMS_DEFAULT_ATTRIBUTES``.
 *
 * The value of this configuration option shall be a valid task attribute set.
 */
#define CONFIGURE_INIT_TASK_ATTRIBUTES

/**
 * @brief The value of this configuration option initializes the entry point of
 *   the Classic API initialization task.
 *
 * This configuration option is an initializer define.
 *
 * The default value is ``Init``.
 *
 * The value of this configuration option shall be defined to a valid function
 * pointer of the type ``void ( *entry_point )( rtems_task_argument )``.
 *
 * The application shall provide the function referenced by this configuration
 * option.
 */
#define CONFIGURE_INIT_TASK_ENTRY_POINT

/**
 * @brief The value of this configuration option defines the initial execution
 *   mode of the Classic API initialization task.
 *
 * This configuration option is an integer define.
 *
 * In SMP  configurations, the default value is ``RTEMS_DEFAULT_MODES``, otherwise
 * the default value is ``RTEMS_NO_PREEMPT``.
 *
 * The value of this configuration option shall be a valid task mode set.
 */
#define CONFIGURE_INIT_TASK_INITIAL_MODES

/**
 * @brief The value of this configuration option defines the name of the
 *   Classic API initialization task.
 *
 * This configuration option is an integer define.
 *
 * The default value is ``rtems_build_name( 'U', 'I', '1', ' ' )``.
 *
 * The value of this configuration option shall be a valid integer of type
 * ``rtems_name``.
 *
 * Use :c:func:`rtems_build_name` to define the task name.
 */
#define CONFIGURE_INIT_TASK_NAME

/**
 * @brief The value of this configuration option defines the initial priority
 *   of the Classic API initialization task.
 *
 * This configuration option is an integer define.
 *
 * The default value is 1.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be a valid Classic API task priority.  The set of valid task
 *   priorities is scheduler-specific.
 *
 * * It shall be a valid Classic API task priority.  The set of valid task
 *   priorities is scheduler-specific.
 */
#define CONFIGURE_INIT_TASK_PRIORITY

/**
 * @brief The value of this configuration option defines the task stack size of
 *   the Classic API initialization task.
 *
 * This configuration option is an integer define.
 *
 * The default value is :ref:`CONFIGURE_MINIMUM_TASK_STACK_SIZE`.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to
 *   :ref:`CONFIGURE_MINIMUM_TASK_STACK_SIZE`.
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 */
#define CONFIGURE_INIT_TASK_STACK_SIZE

/**
 * @brief In case this configuration option is defined, then exactly one
 *   Classic API initialization task is configured.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * The application shall define exactly one of the following configuration
 * options
 *
 * * `CONFIGURE_RTEMS_INIT_TASKS_TABLE`,
 *
 * * :ref:`CONFIGURE_POSIX_INIT_THREAD_TABLE`, or
 *
 * * :ref:`CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION`
 *
 * otherwise a compile time error in the configuration file will occur.
 */
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

/** @} */

/**
 * @defgroup Device Driver Configuration Device Driver Configuration
 *
 * This section describes configuration options related to the device drivers.
 * Note that network device drivers are not covered by the following options.
 *
 * @{
 */

/**
 * @brief In case this configuration option is defined, then **no** Clock
 *   Driver is initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then a Clock Driver may be
 * initialized during system initialization.
 *
 * This configuration parameter is intended to prevent the common user error of
 * using the Hello World example as the baseline for an application and leaving
 * out a clock tick source.
 *
 * The application shall define exactly one of the following configuration
 * options
 *
 * * :ref:`CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER`,
 *
 * * `CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER`, or
 *
 * * :ref:`CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER`,
 *
 * otherwise a compile time error in the configuration file will occur.
 */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

/**
 * @brief The value of this configuration option is used to initialize the
 *   Device Driver Table.
 *
 * This configuration option is an initializer define.
 *
 * The default value is the empty list.
 *
 * The value of this configuration option shall be a list of initializers for
 * structures of type :c:type:`rtems_driver_address_table`.
 *
 * The value of this configuration option is placed after the entries of other
 * device driver configuration options.
 *
 * See :ref:`CONFIGURE_APPLICATION_PREREQUISITE_DRIVERS` for an alternative
 * placement of application device driver initializers.
 */
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS

/**
 * @brief In case this configuration option is defined, then the ATA Driver is
 *   initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * Most BSPs do not include support for an ATA Driver.
 *
 * If this option is defined and the BSP does not have this device driver, then
 * the user will get a link time error for an undefined symbol.
 */
#define CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER

/**
 * @brief In case this configuration option is defined, then the Clock Driver
 *   is initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * The Clock Driver is responsible for providing a regular interrupt which
 * invokes a clock tick directive.
 *
 * The application shall define exactly one of the following configuration
 * options
 *
 * * `CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER`,
 *
 * * :ref:`CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER`, or
 *
 * * :ref:`CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER`,
 *
 * otherwise a compile time error in the configuration file will occur.
 */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/**
 * @brief In case this configuration option is defined, then the Console Driver
 *   is initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * The Console Driver is responsible for providing the :file:`/dev/console`
 * device file.  This device is used to initialize the standard input, output,
 * and error file descriptors.
 *
 * BSPs should be constructed in a manner that allows :c:func:`printk` to work
 * properly without the need for the Console Driver to be configured.
 *
 * The
 *
 * * :ref:`CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER`,
 *
 * * :ref:`CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER`, and
 *
 * * :ref:`CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER`
 *
 * configuration options are mutually exclusive.
 */
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

/**
 * @brief In case this configuration option is defined, then the Frame Buffer
 *   Driver is initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * Most BSPs do not include support for a Frame Buffer Driver. This is because
 * many boards do not include the required hardware.
 *
 * If this option is defined and the BSP does not have this device driver, then
 * the user will get a link time error for an undefined symbol.
 */
#define CONFIGURE_APPLICATION_NEEDS_FRAME_BUFFER_DRIVER

/**
 * @brief In case this configuration option is defined, then the IDE Driver is
 *   initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * Most BSPs do not include support for an IDE Driver.
 *
 * If this option is defined and the BSP does not have this device driver, then
 * the user will get a link time error for an undefined symbol.
 */
#define CONFIGURE_APPLICATION_NEEDS_IDE_DRIVER

/**
 * @brief In case this configuration option is defined, then the
 *   :file:`/dev/null` Driver is initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This device driver is supported by all BSPs.
 */
#define CONFIGURE_APPLICATION_NEEDS_NULL_DRIVER

/**
 * @brief In case this configuration option is defined, then the Real-Time
 *   Clock Driver is initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * Most BSPs do not include support for a real-time clock (RTC). This is
 * because many boards do not include the required hardware.
 *
 * If this is defined and the BSP does not have this device driver, then the
 * user will get a link time error for an undefined symbol.
 */
#define CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER

/**
 * @brief In case this configuration option is defined, then the Simple Console
 *   Driver is initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This device driver is responsible for providing the :file:`/dev/console`
 * device file.  This device is used to initialize the standard input, output,
 * and error file descriptors.
 *
 * This device driver reads via :c:func:`getchark`.
 *
 * This device driver writes via :c:func:`rtems_putc`.
 *
 * The Termios framework is not used.  There is no support to change device
 * settings, e.g. baud, stop bits, parity, etc.
 *
 * The
 *
 * * :ref:`CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER`,
 *
 * * :ref:`CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER`, and
 *
 * * :ref:`CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER`
 *
 * configuration options are mutually exclusive.
 */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

/**
 * @brief In case this configuration option is defined, then the Simple Task
 *   Console Driver is initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This device driver is responsible for providing the :file:`/dev/console`
 * device file.  This device is used to initialize the standard input, output,
 * and error file descriptors.
 *
 * This device driver reads via :c:func:`getchark`.
 *
 * This device driver writes into a write buffer.  The count of characters
 * written into the write buffer is returned.  It might be less than the
 * requested count, in case the write buffer is full.  The write is
 * non-blocking and may be called from interrupt context.  A dedicated task
 * reads from the write buffer and outputs the characters via
 * :c:func:`rtems_putc`.  This task runs with the least important priority. The
 * write buffer size is 2047 characters and it is not configurable.
 *
 * Use ``fsync(STDOUT_FILENO)`` or ``fdatasync(STDOUT_FILENO)`` to drain the
 * write buffer.
 *
 * The Termios framework is not used.  There is no support to change device
 * settings, e.g.  baud, stop bits, parity, etc.
 *
 * The
 *
 * * :ref:`CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER`,
 *
 * * :ref:`CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER`, and
 *
 * * :ref:`CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER`
 *
 * configuration options are mutually exclusive.
 */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER

/**
 * @brief In case this configuration option is defined, then the Stub Driver is
 *   initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This device driver simply provides entry points that return successful and
 * is primarily a test fixture. It is supported by all BSPs.
 */
#define CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER

/**
 * @brief In case this configuration option is defined, then the Benchmark
 *   Timer Driver is initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * The Benchmark Timer Driver is intended for the benchmark tests of the RTEMS
 * Testsuite.  Applications should not use this driver.
 *
 * The application shall define exactly one of the following configuration
 * options
 *
 * * :ref:`CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER`,
 *
 * * :ref:`CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER`, or
 *
 * * `CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER`,
 *
 * otherwise a compile time error will occur.
 */
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

/**
 * @brief In case this configuration option is defined, then the Watchdog
 *   Driver is initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * Most BSPs do not include support for a watchdog device driver. This is
 * because many boards do not include the required hardware.
 *
 * If this is defined and the BSP does not have this device driver, then the
 * user will get a link time error for an undefined symbol.
 */
#define CONFIGURE_APPLICATION_NEEDS_WATCHDOG_DRIVER

/**
 * @brief In case this configuration option is defined, then the
 *   :file:`/dev/zero` Driver is initialized during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This device driver is supported by all BSPs.
 */
#define CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER

/**
 * @brief The value of this configuration option is used to initialize the
 *   Device Driver Table.
 *
 * This configuration option is an initializer define.
 *
 * The default value is the empty list.
 *
 * The value of this configuration option shall be a list of initializers for
 * structures of type :c:type:`rtems_driver_address_table`.
 *
 * The value of this configuration option is placed after the entries defined
 * by :ref:`CONFIGURE_BSP_PREREQUISITE_DRIVERS` and before all other device
 * driver configuration options.
 *
 * See :ref:`CONFIGURE_APPLICATION_EXTRA_DRIVERS` for an alternative placement
 * of application device driver initializers.
 */
#define CONFIGURE_APPLICATION_PREREQUISITE_DRIVERS

/**
 * @brief The value of this configuration option defines the ATA task priority.
 *
 * This configuration option is an integer define.
 *
 * The default value is 140.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be a valid Classic API task priority.  The set of valid task
 *   priorities is scheduler-specific.
 *
 * * It shall be a valid Classic API task priority.  The set of valid task
 *   priorities is scheduler-specific.
 *
 * This configuration option is only evaluated if the configuration option
 * :ref:`CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER` is defined.
 */
#define CONFIGURE_ATA_DRIVER_TASK_PRIORITY

/**
 * @brief The value of this configuration option defines the number of device
 *   drivers.
 *
 * This configuration option is an integer define.
 *
 * This is computed by default, and is set to the number of device drivers
 * configured using the ``CONFIGURE_APPLICATIONS_NEEDS_XXX_DRIVER`` configuration
 * options.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be less than or equal to ``SIZE_MAX``.
 *
 * * It shall be greater than or equal than the number of statically configured
 *   device drivers.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * If the application will dynamically install device drivers, then this
 * configuration parameter shall be larger than the number of statically
 * configured device drivers. Drivers configured using the
 * ``CONFIGURE_APPLICATIONS_NEEDS_XXX_DRIVER`` configuration options are
 * statically installed.
 */
#define CONFIGURE_MAXIMUM_DRIVERS

/** @} */

/**
 * @defgroup Event Recording Configuration Event Recording Configuration
 *
 * This section describes configuration options related to the event recording.
 *
 * @{
 */

/**
 * @brief In case
 *
 * @brief * this configuration option is defined
 *
 * @brief * and :ref:`CONFIGURE_RECORD_PER_PROCESSOR_ITEMS` is properly
 *     defined,
 *
 * @brief then the event record extensions are enabled.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * The record extensions capture thread create, start, restart, delete, switch,
 * begin, exitted and terminate events.
 */
#define CONFIGURE_RECORD_EXTENSIONS_ENABLED

/**
 * @brief In case
 *
 * @brief * this configuration option is defined
 *
 * @brief * and :ref:`CONFIGURE_RECORD_PER_PROCESSOR_ITEMS` is properly
 *     defined,
 *
 * @brief * and :ref:`CONFIGURE_RECORD_FATAL_DUMP_BASE64_ZLIB` is undefined,
 *
 * @brief then the event records are dumped in Base64 encoding in a fatal error
 *   extension (see :ref:`Terminate`).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This extension can be used to produce crash dumps.
 */
#define CONFIGURE_RECORD_FATAL_DUMP_BASE64

/**
 * @brief In case
 *
 * @brief * this configuration option is defined
 *
 * @brief * and :ref:`CONFIGURE_RECORD_PER_PROCESSOR_ITEMS` is properly
 *     defined,
 *
 * @brief then the event records are compressed by zlib and dumped in Base64
 *   encoding in a fatal error extension (see :ref:`Terminate`).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * The zlib compression needs about 512KiB of RAM.  This extension can be used
 * to produce crash dumps.
 */
#define CONFIGURE_RECORD_FATAL_DUMP_BASE64_ZLIB

/**
 * @brief The value of this configuration option defines the event record item
 *   count per processor.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 16.
 *
 * * It shall be less than or equal to ``SIZE_MAX``.
 *
 * * It shall be a power of two.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * The event record buffers are statically allocated for each configured
 * processor (:ref:`CONFIGURE_MAXIMUM_PROCESSORS`).  If the value of this
 * configuration option is zero, then nothing is allocated.
 */
#define CONFIGURE_RECORD_PER_PROCESSOR_ITEMS

/** @} */

/**
 * @defgroup Filesystem Configuration Filesystem Configuration
 *
 * This section describes configuration options related to filesytems. By
 * default, the In-Memory Filesystem (IMFS) is used as the base filesystem
 * (also known as root filesystem).  In order to save some memory for your
 * application, you can disable the filesystem support with the
 * :ref:`CONFIGURE_APPLICATION_DISABLE_FILESYSTEM` configuration option.
 * Alternatively, you can strip down the features of the base filesystem with
 * the :ref:`CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM` and
 * :ref:`CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM` configuration options.  These
 * three configuration options are mutually exclusive.  They are intended for
 * an advanced application configuration.
 *
 * Features of the IMFS can be disabled and enabled with the following
 * configuration options:
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_CHMOD`
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_CHOWN`
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_LINK`
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_MKNOD`
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_MKNOD_FILE`
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_MOUNT`
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_READDIR`
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_READLINK`
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_RENAME`
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_RMNOD`
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_SYMLINK`
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_UNMOUNT`
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_UTIME`
 *
 * * :ref:`CONFIGURE_IMFS_ENABLE_MKFIFO`
 *
 * @{
 */

/**
 * @brief In case this configuration option is defined, then **no** base
 *   filesystem is initialized during system initialization and **no**
 *   filesystems are configured.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then a base filesystem and the
 * configured filesystems are initialized during system initialization.
 *
 * Filesystems shall be initialized to support file descriptor based device
 * drivers and basic input/output functions such as :c:func:`printf`.
 * Filesystems can be disabled to reduce the memory footprint of an
 * application.
 */
#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

/**
 * @brief In case this configuration option is defined, then the following
 *   configuration options will be defined as well
 *
 * @brief * :ref:`CONFIGURE_FILESYSTEM_DOSFS`,
 *
 * @brief * :ref:`CONFIGURE_FILESYSTEM_FTPFS`,
 *
 * @brief * :ref:`CONFIGURE_FILESYSTEM_IMFS`,
 *
 * @brief * :ref:`CONFIGURE_FILESYSTEM_JFFS2`,
 *
 * @brief * :ref:`CONFIGURE_FILESYSTEM_NFS`,
 *
 * @brief * :ref:`CONFIGURE_FILESYSTEM_RFS`, and
 *
 * @brief * :ref:`CONFIGURE_FILESYSTEM_TFTPFS`.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 */
#define CONFIGURE_FILESYSTEM_ALL

/**
 * @brief In case this configuration option is defined, then the DOS (FAT)
 *   filesystem is registered, so that instances of this filesystem can be
 *   mounted by the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This filesystem requires a Block Device Cache configuration, see
 * :ref:`CONFIGURE_APPLICATION_NEEDS_LIBBLOCK`.
 */
#define CONFIGURE_FILESYSTEM_DOSFS

/**
 * @brief In case this configuration option is defined, then the FTP filesystem
 *   (FTP client) is registered, so that instances of this filesystem can be
 *   mounted by the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 */
#define CONFIGURE_FILESYSTEM_FTPFS

/**
 * @brief In case this configuration option is defined, then the In-Memory
 *   Filesystem (IMFS) is registered, so that instances of this filesystem can
 *   be mounted by the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * Applications will rarely need this configuration option.  This configuration
 * option is intended for test programs.  You do not need to define this
 * configuration option for the base filesystem (also known as root
 * filesystem).
 */
#define CONFIGURE_FILESYSTEM_IMFS

/**
 * @brief In case this configuration option is defined, then the JFFS2
 *   filesystem is registered, so that instances of this filesystem can be
 *   mounted by the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 */
#define CONFIGURE_FILESYSTEM_JFFS2

/**
 * @brief In case this configuration option is defined, then the Network
 *   Filesystem (NFS) client is registered, so that instances of this
 *   filesystem can be mounted by the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 */
#define CONFIGURE_FILESYSTEM_NFS

/**
 * @brief In case this configuration option is defined, then the RTEMS
 *   Filesystem (RFS) is registered, so that instances of this filesystem can
 *   be mounted by the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This filesystem requires a Block Device Cache configuration, see
 * :ref:`CONFIGURE_APPLICATION_NEEDS_LIBBLOCK`.
 */
#define CONFIGURE_FILESYSTEM_RFS

/**
 * @brief In case this configuration option is defined, then the TFTP
 *   filesystem (TFTP client) is registered, so that instances of this
 *   filesystem can be mounted by the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 */
#define CONFIGURE_FILESYSTEM_TFTPFS

/**
 * @brief In case this configuration option is defined, then the root IMFS does
 *   not support changing the mode of files (no support for :c:func:`chmod`).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS supports changing
 * the mode of files.
 */
#define CONFIGURE_IMFS_DISABLE_CHMOD

/**
 * @brief In case this configuration option is defined, then the root IMFS does
 *   not support changing the ownership of files (no support for
 *   :c:func:`chown`).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS supports changing
 * the ownership of files.
 */
#define CONFIGURE_IMFS_DISABLE_CHOWN

/**
 * @brief In case this configuration option is defined, then the root IMFS does
 *   not support hard links (no support for :c:func:`link`).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS supports hard
 * links.
 */
#define CONFIGURE_IMFS_DISABLE_LINK

/**
 * @brief In case this configuration option is defined, then the root IMFS does
 *   not support making files (no support for :c:func:`mknod`).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS supports making
 * files.
 */
#define CONFIGURE_IMFS_DISABLE_MKNOD

/**
 * @brief In case this configuration option is defined, then the root IMFS does
 *   not support making device files.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS supports making
 * device files.
 */
#define CONFIGURE_IMFS_DISABLE_MKNOD_DEVICE

/**
 * @brief In case this configuration option is defined, then the root IMFS does
 *   not support making regular files.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS supports making
 * regular files.
 */
#define CONFIGURE_IMFS_DISABLE_MKNOD_FILE

/**
 * @brief In case this configuration option is defined, then the root IMFS does
 *   not support mounting other filesystems (no support for :c:func:`mount`).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS supports mounting
 * other filesystems.
 */
#define CONFIGURE_IMFS_DISABLE_MOUNT

/**
 * @brief In case this configuration option is defined, then the root IMFS does
 *   not support reading directories (no support for :c:func:`readdir`).  It is
 *   still possible to open files in a directory.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS supports reading
 * directories.
 */
#define CONFIGURE_IMFS_DISABLE_READDIR

/**
 * @brief In case this configuration option is defined, then the root IMFS does
 *   not support reading symbolic links (no support for :c:func:`readlink`).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS supports reading
 * symbolic links.
 */
#define CONFIGURE_IMFS_DISABLE_READLINK

/**
 * @brief In case this configuration option is defined, then the root IMFS does
 *   not support renaming files (no support for :c:func:`rename`).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS supports renaming
 * files.
 */
#define CONFIGURE_IMFS_DISABLE_RENAME

/**
 * @brief In case this configuration option is defined, then the root IMFS does
 *   not support removing files (no support for :c:func:`rmnod`).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS supports removing
 * files.
 */
#define CONFIGURE_IMFS_DISABLE_RMNOD

/**
 * @brief In case this configuration option is defined, then the root IMFS does
 *   not support creating symbolic links (no support for :c:func:`symlink`).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS supports creating
 * symbolic links.
 */
#define CONFIGURE_IMFS_DISABLE_SYMLINK

/**
 * @brief In case this configuration option is defined, then the root IMFS does
 *   not support unmounting other filesystems (no support for
 *   :c:func:`unmount`).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS supports
 * unmounting other filesystems.
 */
#define CONFIGURE_IMFS_DISABLE_UNMOUNT

/**
 * @brief In case this configuration option is defined, then the root IMFS does
 *   not support changing file times (no support for :c:func:`utime`).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS supports changing
 * file times.
 */
#define CONFIGURE_IMFS_DISABLE_UTIME

/**
 * @brief In case this configuration option is defined, then the root IMFS
 *   supports making FIFOs.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the root IMFS does not support
 * making FIFOs (no support for :c:func:`mkfifo`).
 */
#define CONFIGURE_IMFS_ENABLE_MKFIFO

/**
 * @brief The value of this configuration option defines the block size for
 *   in-memory files managed by the IMFS.
 *
 * This configuration option is an integer define.
 *
 * The default value is 128.
 *
 * The value of this configuration option shall be an element of {16, 32, 64, 128,
 * 256, 512}.
 *
 * The configured block size has two impacts. The first is the average amount
 * of unused memory in the last block of each file.  For example, when the
 * block size is 512, on average one-half of the last block of each file will
 * remain unused and the memory is wasted. In contrast, when the block size is
 * 16, the average unused memory per file is only 8 bytes. However, it requires
 * more allocations for the same size file and thus more overhead per block for
 * the dynamic memory management.
 *
 * Second, the block size has an impact on the maximum size file that can be
 * stored in the IMFS. With smaller block size, the maximum file size is
 * correspondingly smaller. The following shows the maximum file size possible
 * based on the configured block size:
 *
 * * when the block size is 16 bytes, the maximum file size is 1,328 bytes.
 *
 * * when the block size is 32 bytes, the maximum file size is 18,656 bytes.
 *
 * * when the block size is 64 bytes, the maximum file size is 279,488 bytes.
 *
 * * when the block size is 128 bytes, the maximum file size is 4,329,344
 *   bytes.
 *
 * * when the block size is 256 bytes, the maximum file size is 68,173,568
 *   bytes.
 *
 * * when the block size is 512 bytes, the maximum file size is 1,082,195,456
 *   bytes.
 */
#define CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK

/**
 * @brief In case this configuration option is defined, then an IMFS with a
 *   reduced feature set will be the base filesystem (also known as root
 *   filesystem).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * In case this configuration option is defined, then the following
 * configuration options will be defined as well
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_CHMOD`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_CHOWN`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_LINK`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_MKNOD_FILE`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_MOUNT`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_READDIR`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_READLINK`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_RENAME`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_RMNOD`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_SYMLINK`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_UTIME`, and
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_UNMOUNT`.
 *
 * In addition, a simplified path evaluation is enabled.  It allows only a look
 * up of absolute paths.
 *
 * This configuration of the IMFS is basically a device-only filesystem.  It is
 * comparable in functionality to the pseudo-filesystem name space provided
 * before RTEMS release 4.5.0.
 */
#define CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM

/**
 * @brief In case this configuration option is defined, then an IMFS with a
 *   reduced feature set will be the base filesystem (also known as root
 *   filesystem).
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * In case this configuration option is defined, then the following
 * configuration options will be defined as well
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_CHMOD`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_CHOWN`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_LINK`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_READLINK`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_RENAME`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_SYMLINK`,
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_UTIME`, and
 *
 * * :ref:`CONFIGURE_IMFS_DISABLE_UNMOUNT`.
 */
#define CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM

/** @} */

/**
 * @defgroup General System Configuration General System Configuration
 *
 * This section describes general system configuration options.
 *
 * @{
 */

/**
 * @brief In case this configuration option is defined, then the memory areas
 *   used for the RTEMS Workspace and the C Program Heap are dirtied with a
 *   ``0xCF`` byte pattern during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * Dirtying memory can add significantly to system initialization time.  It may
 * assist in finding code that incorrectly assumes the contents of free memory
 * areas is cleared to zero during system initialization.  In case
 * :ref:`CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY` is also defined, then the
 * memory is first dirtied and then zeroed.
 *
 * See also :ref:`CONFIGURE_MALLOC_DIRTY`.
 */
#define CONFIGURE_DIRTY_MEMORY

/**
 * @brief In case this configuration option is defined, then the Newlib
 *   reentrancy support per thread is disabled and a global reentrancy
 *   structure is used.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * You can enable this option to reduce the size of the :term:`TCB`.  Use this
 * option with care, since it can lead to race conditions and undefined system
 * behaviour.  For example, :c:data:`errno` is no longer a thread-local
 * variable if this option is enabled.
 */
#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

/**
 * @brief The value of this configuration option defines the RTEMS Workspace
 *   size in bytes.
 *
 * This configuration option is an integer define.
 *
 * If this configuration option is undefined, then the RTEMS Workspace and task
 * stack space size is calculated by ``<rtems/confdefs.h>`` based on the values
 * configuration options.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to ``UINTPTR_MAX``.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * This is an advanced configuration option.  Use it only if you know exactly
 * what you are doing.
 */
#define CONFIGURE_EXECUTIVE_RAM_SIZE

/**
 * @brief The value of this configuration option defines the number of bytes
 *   the applications wishes to add to the task stack requirements calculated
 *   by ``<rtems/confdefs.h>``.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 *
 * This parameter is very important.  If the application creates tasks with
 * stacks larger then the minimum, then that memory is **not** accounted for by
 * ``<rtems/confdefs.h>``.
 */
#define CONFIGURE_EXTRA_TASK_STACKS

/**
 * @brief The value of this configuration option is used to initialize the
 *   table of initial user extensions.
 *
 * This configuration option is an initializer define.
 *
 * The default value is the empty list.
 *
 * The value of this configuration option shall be a list of initializers for
 * structures of type :c:type:`rtems_extensions_table`.
 *
 * The value of this configuration option is placed before the entries of
 * :ref:`BSP_INITIAL_EXTENSION` and after the entries of all other initial user
 * extensions.
 */
#define CONFIGURE_INITIAL_EXTENSIONS

/**
 * @brief The value of this configuration option defines the size of an
 *   interrupt stack in bytes.
 *
 * This configuration option is an integer define.
 *
 * The default value is :ref:`BSP_INTERRUPT_STACK_SIZE` in case it is defined,
 * otherwise the default value is ``CPU_STACK_MINIMUM_SIZE``.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to a BSP-specific and application-specific
 *   minimum value.
 *
 * * It shall be small enough so that the interrupt stack area calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``size_t``.
 *
 * * It shall be aligned according to ``CPU_INTERRUPT_STACK_ALIGNMENT``.
 *
 * * It shall be greater than or equal to a BSP-specific and application-specific
 *   minimum value.
 *
 * * It shall be small enough so that the interrupt stack area calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``size_t``.
 *
 * * It shall be aligned according to ``CPU_INTERRUPT_STACK_ALIGNMENT``.
 *
 * There is one interrupt stack available for each configured processor
 * (:ref:`CONFIGURE_MAXIMUM_PROCESSORS`).  The interrupt stack areas are
 * statically allocated in a special linker section
 * (``.rtemsstack.interrupt``). The placement of this linker section is
 * BSP-specific.
 *
 * Some BSPs use the interrupt stack as the initialization stack which is used
 * to perform the sequential system initialization before the multithreading is
 * started.
 *
 * The interrupt stacks are covered by the :ref:`stack checker
 * <CONFIGURE_STACK_CHECKER_ENABLED>`.  However, using a too small interrupt
 * stack size may still result in undefined behaviour.
 *
 * In releases before RTEMS 5.1 the default value was
 * :ref:`CONFIGURE_MINIMUM_TASK_STACK_SIZE` instead of
 * ``CPU_STACK_MINIMUM_SIZE``.
 */
#define CONFIGURE_INTERRUPT_STACK_SIZE

/**
 * @brief In case this configuration option is defined, then each memory area
 *   returned by C Program Heap allocator functions such as :c:func:`malloc` is
 *   dirtied with a ``0xCF`` byte pattern before it is handed over to the
 *   application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * The dirtying performed by this option is carried out for each successful
 * memory allocation from the C Program Heap in contrast to
 * :ref:`CONFIGURE_DIRTY_MEMORY` which dirties the memory only once during the
 * system initialization.
 */
#define CONFIGURE_MALLOC_DIRTY

/**
 * @brief The value of this configuration option defines the maximum number of
 *   file like objects that can be concurrently open.
 *
 * This configuration option is an integer define.
 *
 * The default value is 3.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to ``SIZE_MAX``.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * The default value of three file descriptors allows RTEMS to support standard
 * input, output, and error I/O streams on ``/dev/console``.
 */
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS

/**
 * @brief The value of this configuration option defines the maximum number of
 *   processors an application intends to use.  The number of actually
 *   available processors depends on the hardware and may be less.  It is
 *   recommended to use the smallest value suitable for the application in
 *   order to save memory. Each processor needs an IDLE task stack and
 *   interrupt stack for example.
 *
 * This configuration option is an integer define.
 *
 * The default value is 1.
 *
 * The value of this configuration option shall be greater than or equal to 0 and
 * less than or equal to ``CPU_MAXIMUM_PROCESSORS``.
 *
 * If there are more processors available than configured, the rest will be
 * ignored.
 *
 * This configuration option is only evaluated in SMP configurations (e.g.
 * RTEMS was built with the ``--enable-smp`` build configuration option).  In
 * all other configurations it has no effect.
 */
#define CONFIGURE_MAXIMUM_PROCESSORS

/**
 * @brief The value of this configuration option defines the maximum thread
 *   name size including the terminating ``NUL`` character.
 *
 * This configuration option is an integer define.
 *
 * The default value is 16.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to ``SIZE_MAX``.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * The default value was chosen for Linux compatibility, see
 * `PTHREAD_SETNAME_NP(3)
 * <http://man7.org/linux/man-pages/man3/pthread_setname_np.3.html>`_.
 *
 * The size of the thread control block is increased by the maximum thread name
 * size.
 *
 * This configuration option is available since RTEMS 5.1.
 */
#define CONFIGURE_MAXIMUM_THREAD_NAME_SIZE

/**
 * @brief The value of this configuration option defines the number of
 *   kilobytes the application wishes to add to the RTEMS Workspace size
 *   calculated by ``<rtems/confdefs.h>``.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the RTEMS Workspace size calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``uintptr_t``.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the RTEMS Workspace size calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``uintptr_t``.
 *
 * This configuration option should only be used when it is suspected that a
 * bug in ``<rtems/confdefs.h>`` has resulted in an underestimation.  Typically
 * the memory allocation will be too low when an application does not account
 * for all message queue buffers or task stacks, see
 * :ref:`CONFIGURE_MESSAGE_BUFFER_MEMORY`.
 */
#define CONFIGURE_MEMORY_OVERHEAD

/**
 * @brief The value of this configuration option defines the number of bytes
 *   reserved for message queue buffers in the RTEMS Workspace.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the RTEMS Workspace size calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``uintptr_t``.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the RTEMS Workspace size calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``uintptr_t``.
 *
 * The configuration options :ref:`CONFIGURE_MAXIMUM_MESSAGE_QUEUES` and
 * :ref:`CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES` define only how many message
 * queues can be created by the application.  The memory for the message
 * buffers is configured by this option.  For each message queue you have to
 * reserve some memory for the message buffers.  The size dependes on the
 * maximum number of pending messages and the maximum size of the messages of a
 * message queue.  Use the ``CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE()`` macro to
 * specify the message buffer memory for each message queue and sum them up to
 * define the value for ``CONFIGURE_MAXIMUM_MESSAGE_QUEUES``.
 *
 * The interface for the ``CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE()`` help macro
 * is as follows:
 *
 * .. code-block:: c
 *
 *     CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( max_messages, max_msg_size )
 *
 * Where ``max_messages`` is the maximum number of pending messages and
 * ``max_msg_size`` is the maximum size in bytes of the messages of the
 * corresponding message queue.  Both parameters shall be compile time
 * constants.  Not using this help macro (e.g. just using ``max_messages *
 * max_msg_size``) may result in an underestimate of the RTEMS Workspace size.
 *
 * The following example illustrates how the
 * `CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE()` help macro can be used to assist in
 * calculating the message buffer memory required.  In this example, there are
 * two message queues used in this application.  The first message queue has a
 * maximum of 24 pending messages with the message structure defined by the
 * type ``one_message_type``.  The other message queue has a maximum of 500
 * pending messages with the message structure defined by the type
 * ``other_message_type``.
 *
 * .. code-block:: c
 *
 *     #define CONFIGURE_MESSAGE_BUFFER_MEMORY ( \
 *     CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( \       24, \       sizeof(
 *     one_message_type ) \     ) \     + CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE(
 *     \       500, \       sizeof( other_message_type ) \     ) \   )
 */
#define CONFIGURE_MESSAGE_BUFFER_MEMORY

/**
 * @brief The value of this configuration option defines the length of time in
 *   microseconds between clock ticks (clock tick quantum).
 *
 * @brief When the clock tick quantum value is too low, the system will spend
 *   so much time processing clock ticks that it does not have processing time
 *   available to perform application work. In this case, the system will
 *   become unresponsive.
 *
 * @brief The lowest practical time quantum varies widely based upon the speed
 *   of the target hardware and the architectural overhead associated with
 *   interrupts. In general terms, you do not want to configure it lower than
 *   is needed for the application.
 *
 * @brief The clock tick quantum should be selected such that it all blocking
 *   and delay times in the application are evenly divisible by it. Otherwise,
 *   rounding errors will be introduced which may negatively impact the
 *   application.
 *
 * This configuration option is an integer define.
 *
 * The default value is 10000.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to a Clock Driver specific value.
 *
 * * It shall be less than or equal to a Clock Driver specific value.
 *
 * * The resulting clock ticks per second should be an integer.
 *
 * This configuration option has no impact if the Clock Driver is not
 * configured, see :ref:`CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER`.
 *
 * There may be Clock Driver specific limits on the resolution or maximum value
 * of a clock tick quantum.
 */
#define CONFIGURE_MICROSECONDS_PER_TICK

/**
 * @brief The value of this configuration option defines the minimum stack size
 *   in bytes for every user task or thread in the system.
 *
 * This configuration option is an integer define.
 *
 * The default value is ``CPU_STACK_MINIMUM_SIZE``.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 *
 * * It shall be greater than or equal to a BSP-specific and application-specific
 *   minimum value.
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 *
 * * It shall be greater than or equal to a BSP-specific and application-specific
 *   minimum value.
 *
 * Adjusting this parameter should be done with caution.  Examining the actual
 * stack usage using the stack checker usage reporting facility is recommended
 * (see also :ref:`CONFIGURE_STACK_CHECKER_ENABLED`).
 *
 * This parameter can be used to lower the minimum from that recommended. This
 * can be used in low memory systems to reduce memory consumption for stacks.
 * However, this shall be done with caution as it could increase the
 * possibility of a blown task stack.
 *
 * This parameter can be used to increase the minimum from that recommended.
 * This can be used in higher memory systems to reduce the risk of stack
 * overflow without performing analysis on actual consumption.
 *
 * By default, this configuration parameter defines also the minimum stack size
 * of POSIX threads.  This can be changed with the
 * :ref:`CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE` configuration option.
 *
 * In releases before RTEMS 5.1 the ``CONFIGURE_MINIMUM_TASK_STACK_SIZE`` was
 * used to define the default value of :ref:`CONFIGURE_INTERRUPT_STACK_SIZE`.
 */
#define CONFIGURE_MINIMUM_TASK_STACK_SIZE

/**
 * @brief In case this configuration option is defined, then the stack checker
 *   is enabled.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * The stack checker performs run-time stack bounds checking.  This increases
 * the time required to create tasks as well as adding overhead to each context
 * switch.
 *
 * In 4.9 and older, this configuration option was named ``STACK_CHECKER_ON``.
 */
#define CONFIGURE_STACK_CHECKER_ENABLED

/**
 * @brief The value of this configuration option defines the length of the
 *   timeslice quantum in ticks for each task.
 *
 * This configuration option is an integer define.
 *
 * The default value is 50.
 *
 * The value of this configuration option shall be greater than or equal to 0 and
 * less than or equal to ``UINT32_MAX``.
 *
 * This configuration option has no impact if the Clock Driver is not
 * configured, see :ref:`CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER`.
 */
#define CONFIGURE_TICKS_PER_TIMESLICE

/**
 * @brief In case this configuration option is defined, then the RTEMS
 *   Workspace and the C Program Heap will be one pool of memory.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then there will be separate memory
 * pools for the RTEMS Workspace and C Program Heap.
 *
 * Having separate pools does have some advantages in the event a task blows a
 * stack or writes outside its memory area. However, in low memory systems the
 * overhead of the two pools plus the potential for unused memory in either
 * pool is very undesirable.
 *
 * In high memory environments, this is desirable when you want to use the
 * :ref:`ConfigUnlimitedObjects` option.  You will be able to create objects
 * until you run out of all available memory rather then just until you run out
 * of RTEMS Workspace.
 */
#define CONFIGURE_UNIFIED_WORK_AREAS

/**
 * @brief If :ref:`CONFIGURE_UNLIMITED_OBJECTS` is defined, then the value of
 *   this configuration option defines the default objects maximum of all
 *   object classes supporting :ref:`ConfigUnlimitedObjects` to
 *   ``rtems_resource_unlimited(CONFIGURE_UNLIMITED_ALLOCATION_SIZE)``.
 *
 * This configuration option is an integer define.
 *
 * The default value is 8.
 *
 * The value of this configuration option shall meet the constraints of all object
 * classes to which it is applied.
 *
 * By allowing users to declare all resources as being unlimited the user can
 * avoid identifying and limiting the resources used.
 *
 * The object maximum of each class can be configured also individually using
 * the :c:func:`rtems_resource_unlimited` macro.
 */
#define CONFIGURE_UNLIMITED_ALLOCATION_SIZE

/**
 * @brief In case this configuration option is defined, then unlimited objects
 *   are used by default.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * When using unlimited objects, it is common practice to also specify
 * :ref:`CONFIGURE_UNIFIED_WORK_AREAS` so the system operates with a single
 * pool of memory for both RTEMS Workspace and C Program Heap.
 *
 * This option does not override an explicit configuration for a particular
 * object class by the user.
 *
 * See also :ref:`CONFIGURE_UNLIMITED_ALLOCATION_SIZE`.
 */
#define CONFIGURE_UNLIMITED_OBJECTS

/**
 * @brief In case this configuration option is defined, then the system
 *   initialization is verbose.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * You may use this feature to debug system initialization issues.  The
 * :c:func:`printk` function is used to print the information.
 */
#define CONFIGURE_VERBOSE_SYSTEM_INITIALIZATION

/**
 * @brief In case this configuration option is defined, then the memory areas
 *   used for the RTEMS Workspace and the C Program Heap are zeroed with a
 *   ``0x00`` byte pattern during system initialization.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * Zeroing memory can add significantly to the system initialization time. It
 * is not necessary for RTEMS but is often assumed by support libraries.  In
 * case :ref:`CONFIGURE_DIRTY_MEMORY` is also defined, then the memory is first
 * dirtied and then zeroed.
 */
#define CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY

/** @} */

/**
 * @defgroup Idle Task Configuration Idle Task Configuration
 *
 * This section describes configuration options related to the idle tasks.
 *
 * @{
 */

/**
 * @brief The value of this configuration option initializes the IDLE thread
 *   body.
 *
 * This configuration option is an initializer define.
 *
 * If :ref:`BSP_IDLE_TASK_BODY` is defined, then this will be the default value,
 * otherwise the default value is ``_CPU_Thread_Idle_body``.
 *
 * The value of this configuration option shall be defined to a valid function
 * pointer of the type ``void *( *idle_body )( uintptr_t )``.
 *
 * IDLE threads shall not block.  A blocking IDLE thread results in undefined
 * system behaviour because the scheduler assume that at least one ready thread
 * exists.
 *
 * IDLE threads can be used to initialize the application, see configuration
 * option :ref:`CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION`.
 */
#define CONFIGURE_IDLE_TASK_BODY

/**
 * @brief This configuration option is defined to indicate that the user has
 *   configured **no** user initialization tasks or threads and that the user
 *   provided IDLE task will perform application initialization and then
 *   transform itself into an IDLE task.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the user is assumed to provide
 * one or more initialization tasks.
 *
 * If you use this option be careful, the user IDLE task **cannot** block at
 * all during the initialization sequence.  Further, once application
 * initialization is complete, it shall make itself preemptible and enter an
 * idle body loop.
 *
 * The IDLE task shall run at the lowest priority of all tasks in the system.
 *
 * If this configuration option is defined, then it is mandatory to configure a
 * user IDLE task with the :ref:`CONFIGURE_IDLE_TASK_BODY` configuration
 * option, otherwise a compile time error in the configuration file will occur.
 *
 * The application shall define exactly one of the following configuration
 * options
 *
 * * :ref:`CONFIGURE_RTEMS_INIT_TASKS_TABLE`,
 *
 * * :ref:`CONFIGURE_POSIX_INIT_THREAD_TABLE`, or
 *
 * * `CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION`
 *
 * otherwise a compile time error in the configuration file will occur.
 */
#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

/**
 * @brief The value of this configuration option defines the task stack size
 *   for an IDLE task.
 *
 * This configuration option is an integer define.
 *
 * The default value is :ref:`CONFIGURE_MINIMUM_TASK_STACK_SIZE`.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to a BSP-specific and application-specific
 *   minimum value.
 *
 * * It shall be small enough so that the IDLE task stack area calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``size_t``.
 *
 * * It shall be greater than or equal to a BSP-specific and application-specific
 *   minimum value.
 *
 * * It shall be small enough so that the IDLE task stack area calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``size_t``.
 *
 * In SMP configurations, there is one IDLE task per configured processor, see
 * :ref:`CONFIGURE_MAXIMUM_PROCESSORS`.
 */
#define CONFIGURE_IDLE_TASK_STACK_SIZE

/** @} */

/**
 * @defgroup Multiprocessing Configuration Multiprocessing Configuration
 *
 * This section describes multiprocessing related configuration options.  The
 * options are only used if RTEMS was built with the
 * ``--enable-multiprocessing`` build configuration option.  Additionally, this
 * class of configuration options are only applicable if the configuration
 * option :ref:`CONFIGURE_MP_APPLICATION` is defined.  The multiprocessing
 * (MPCI) support must not be confused with the SMP support.
 *
 * @{
 */

/**
 * @brief This configuration option is defined to indicate that the application
 *   intends to be part of a multiprocessing configuration.  Additional
 *   configuration options are assumed to be provided.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the multiprocessing services
 * are not initialized.
 *
 * This configuration option shall be undefined if the multiprocessing support
 * is not enabled (e.g. RTEMS was built without the
 * ``--enable-multiprocessing`` build configuration option).  Otherwise a
 * compile time error in the configuration file will occur.
 */
#define CONFIGURE_MP_APPLICATION

/**
 * @brief The value of this configuration option defines the number of bytes
 *   the applications wishes to add to the MPCI task stack on top of
 *   :ref:`CONFIGURE_MINIMUM_TASK_STACK_SIZE`.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to ``UINT32_MAX``.
 *
 * * It shall be small enough so that the MPCI receive server stack area
 *   calculation carried out by ``<rtems/confdefs.h>`` does not overflow an
 *   integer of type ``size_t``.
 *
 * This configuration option is only evaluated if
 * :ref:`CONFIGURE_MP_APPLICATION` is defined.
 */
#define CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK

/**
 * @brief The value of this configuration option defines the maximum number of
 *   concurrently active global objects in a multiprocessor system.
 *
 * This configuration option is an integer define.
 *
 * The default value is 32.
 *
 * The value of this configuration option shall be greater than or equal to 0 and
 * less than or equal to ``UINT32_MAX``.
 *
 * This value corresponds to the total number of objects which can be created
 * with the ``RTEMS_GLOBAL`` attribute.
 *
 * This configuration option is only evaluated if
 * :ref:`CONFIGURE_MP_APPLICATION` is defined.
 */
#define CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS

/**
 * @brief The value of this configuration option defines the maximum number of
 *   nodes in a multiprocessor system.
 *
 * This configuration option is an integer define.
 *
 * The default value is 2.
 *
 * The value of this configuration option shall be greater than or equal to 0 and
 * less than or equal to ``UINT32_MAX``.
 *
 * This configuration option is only evaluated if
 * :ref:`CONFIGURE_MP_APPLICATION` is defined.
 */
#define CONFIGURE_MP_MAXIMUM_NODES

/**
 * @brief The value of this configuration option defines the maximum number of
 *   concurrently active thread/task proxies on this node in a multiprocessor
 *   system.
 *
 * This configuration option is an integer define.
 *
 * The default value is 32.
 *
 * The value of this configuration option shall be greater than or equal to 0 and
 * less than or equal to ``UINT32_MAX``.
 *
 * Since a proxy is used to represent a remote task/thread which is blocking on
 * this node. This configuration parameter reflects the maximum number of
 * remote tasks/threads which can be blocked on objects on this node, see
 * :ref:`MPCIProxies`.
 *
 * This configuration option is only evaluated if
 * :ref:`CONFIGURE_MP_APPLICATION` is defined.
 */
#define CONFIGURE_MP_MAXIMUM_PROXIES

/**
 * @brief The value of this configuration option initializes the MPCI
 *   Configuration Table.
 *
 * This configuration option is an initializer define.
 *
 * The default value is ``&MPCI_table``.
 *
 * The value of this configuration option shall be a pointer to
 * :c:type:`rtems_mpci_table`.
 *
 * RTEMS provides a Shared Memory MPCI Device Driver which can be used on any
 * Multiprocessor System assuming the BSP provides the proper set of supporting
 * methods.
 *
 * This configuration option is only evaluated if
 * :ref:`CONFIGURE_MP_APPLICATION` is defined.
 */
#define CONFIGURE_MP_MPCI_TABLE_POINTER

/**
 * @brief The value of this configuration option defines the node number of
 *   this node in a multiprocessor system.
 *
 * This configuration option is an integer define.
 *
 * The default value is ``NODE_NUMBER``.
 *
 * The value of this configuration option shall be greater than or equal to 0 and
 * less than or equal to ``UINT32_MAX``.
 *
 * In the RTEMS Multiprocessing Test Suite, the node number is derived from the
 * Makefile variable ``NODE_NUMBER``. The same code is compiled with the
 * ``NODE_NUMBER`` set to different values. The test programs behave
 * differently based upon their node number.
 *
 * This configuration option is only evaluated if
 * :ref:`CONFIGURE_MP_APPLICATION` is defined.
 */
#define CONFIGURE_MP_NODE_NUMBER

/** @} */

/**
 * @defgroup POSIX API Configuration POSIX API Configuration
 *
 * This section describes configuration options related to the POSIX API.  Most
 * POSIX API objects are available by default since RTEMS 5.1.  The queued
 * signals and timers are only available if RTEMS was built with the
 * ``--enable-posix`` build configuration option.
 *
 * @{
 */

/**
 * @brief The value of this configuration option defines the maximum number of
 *   POSIX API Keys that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.
 */
#define CONFIGURE_MAXIMUM_POSIX_KEYS

/**
 * @brief The value of this configuration option defines the maximum number of
 *   key value pairs used by POSIX API Keys that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is :ref:`CONFIGURE_MAXIMUM_POSIX_KEYS` *
 * :ref:`CONFIGURE_MAXIMUM_TASKS` + :ref:`CONFIGURE_MAXIMUM_POSIX_THREADS`.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.
 *
 * A key value pair is created by :c:func:`pthread_setspecific` if the value is
 * not :c:macro:`NULL`, otherwise it is deleted.
 */
#define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS

/**
 * @brief The value of this configuration option defines the maximum number of
 *   POSIX API Message Queues that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the RTEMS Workspace size calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``uintptr_t``.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the RTEMS Workspace size calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``uintptr_t``.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.  You have to account for the memory used to
 * store the messages of each message queue, see
 * :ref:`CONFIGURE_MESSAGE_BUFFER_MEMORY`.
 */
#define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES

/**
 * @brief The value of this configuration option defines the maximum number of
 *   POSIX API Queued Signals that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the RTEMS Workspace size calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``uintptr_t``.
 *
 * * It shall be zero if the POSIX API is not enabled (e.g. RTEMS was built
 *   without the ``--enable-posix`` build configuration option).  Otherwise a
 *   compile time error in the configuration file will occur.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the RTEMS Workspace size calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``uintptr_t``.
 *
 * * It shall be zero if the POSIX API is not enabled (e.g. RTEMS was built
 *   without the ``--enable-posix`` build configuration option).  Otherwise a
 *   compile time error in the configuration file will occur.
 *
 * Unlimited objects are not available for queued signals.
 *
 * Queued signals are only available if RTEMS was built with the
 * ``--enable-posix`` build configuration option.
 */
#define CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS

/**
 * @brief The value of this configuration option defines the maximum number of
 *   POSIX API Named Semaphores that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the RTEMS Workspace size calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``uintptr_t``.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the RTEMS Workspace size calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``uintptr_t``.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.
 *
 * Named semaphores are created with :c:func:`sem_open()`.  Semaphores
 * initialized with :c:func:`sem_init()` are not affected by this configuration
 * option since the storage space for these semaphores is user-provided.
 */
#define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES

/**
 * @brief The value of this configuration option defines the maximum number of
 *   POSIX API Shared Memory objects that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the RTEMS Workspace size calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``uintptr_t``.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the RTEMS Workspace size calculation carried
 *   out by ``<rtems/confdefs.h>`` does not overflow an integer of type
 *   ``uintptr_t``.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.
 */
#define CONFIGURE_MAXIMUM_POSIX_SHMS

/**
 * @brief The value of this configuration option defines the maximum number of
 *   POSIX API Threads that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.
 *
 * This calculations for the required memory in the RTEMS Workspace for threads
 * assume that each thread has a minimum stack size and has floating point
 * support enabled.  The configuration option
 * :ref:`CONFIGURE_EXTRA_TASK_STACKS` is used to specify thread stack
 * requirements **above** the minimum size required.  See :ref:`Reserve
 * Task/Thread Stack Memory Above Minimum` for more information about
 * ``CONFIGURE_EXTRA_TASK_STACKS``.
 *
 * The maximum number of Classic API Tasks is specified by
 * :ref:`CONFIGURE_MAXIMUM_TASKS`.
 *
 * All POSIX threads have floating point enabled.
 */
#define CONFIGURE_MAXIMUM_POSIX_THREADS

/**
 * @brief The value of this configuration option defines the maximum number of
 *   POSIX API Timers that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is 0.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to 65535.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be zero if the POSIX API is not enabled (e.g. RTEMS was built
 *   without the ``--enable-posix`` build configuration option).  Otherwise a
 *   compile time error in the configuration file will occur.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It may be defined through :c:func:`rtems_resource_unlimited` the enable
 *   unlimited objects for this object class, if the value passed to
 *   :c:func:`rtems_resource_unlimited` satisfies all other constraints of this
 *   configuration option.
 *
 * * It shall be zero if the POSIX API is not enabled (e.g. RTEMS was built
 *   without the ``--enable-posix`` build configuration option).  Otherwise a
 *   compile time error in the configuration file will occur.
 *
 * This object class can be configured in unlimited allocation mode, see
 * :ref:`ConfigUnlimitedObjects`.
 *
 * Timers are only available if RTEMS was built with the ``--enable-posix``
 * build configuration option.
 */
#define CONFIGURE_MAXIMUM_POSIX_TIMERS

/**
 * @brief The value of this configuration option defines the minimum stack size
 *   in bytes for every POSIX thread in the system.
 *
 * This configuration option is an integer define.
 *
 * The default value is two times the value of
 * :ref:`CONFIGURE_MINIMUM_TASK_STACK_SIZE`.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 *
 * * It shall be greater than or equal to a BSP-specific and application-specific
 *   minimum value.
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 *
 * * It shall be greater than or equal to a BSP-specific and application-specific
 *   minimum value.
 */
#define CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE

/** @} */

/**
 * @defgroup POSIX Initialization Thread Configuration POSIX Initialization Thread Configuration
 *
 * This section describes configuration options related to the POSIX
 * initialization thread.
 *
 * @{
 */

/**
 * @brief The value of this configuration option initializes the entry point of
 *   the POSIX API initialization thread.
 *
 * This configuration option is an initializer define.
 *
 * The default value is ``POSIX_Init``.
 *
 * The value of this configuration option shall be defined to a valid function
 * pointer of the type ``void *( *entry_point )( void * )``.
 *
 * The application shall provide the function referenced by this configuration
 * option.
 */
#define CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT

/**
 * @brief The value of this configuration option defines the thread stack size
 *   of the POSIX API initialization thread.
 *
 * This configuration option is an integer define.
 *
 * The default value is :ref:`CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE`.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to
 *   :ref:`CONFIGURE_MINIMUM_TASK_STACK_SIZE`.
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 *
 * * It shall be small enough so that the task stack space calculation carried out
 *   by ``<rtems/confdefs.h>`` does not overflow an integer of type ``uintptr_t``.
 */
#define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE

/**
 * @brief In case this configuration option is defined, then exactly one POSIX
 *   initialization thread is configured.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * The application shall define exactly one of the following configuration
 * options
 *
 * * :ref:`CONFIGURE_RTEMS_INIT_TASKS_TABLE`,
 *
 * * `CONFIGURE_POSIX_INIT_THREAD_TABLE`, or
 *
 * * :ref:`CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION`
 *
 * otherwise a compile time error in the configuration file will occur.
 */
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

/** @} */

/**
 * @defgroup General Scheduler Configuration General Scheduler Configuration
 *
 * This section describes configuration options related to selecting a
 * scheduling algorithm for an application.  A scheduler configuration is
 * optional and only necessary in very specific circumstances.  A normal
 * application configuration does not need any of the configuration options
 * described in this section.
 *
 * By default, the :ref:`Deterministic Priority Scheduler <SchedulerPriority>`
 * algorithm is used in uniprocessor configurations.  In case SMP is enabled
 * and the configured maximum processors (:ref:`CONFIGURE_MAXIMUM_PROCESSORS
 * <CONFIGURE_MAXIMUM_PROCESSORS>`) is greater than one, then the
 * :ref:`Earliest Deadline First (EDF) SMP Scheduler <SchedulerSMPEDF>` is
 * selected as the default scheduler algorithm.
 *
 * For the :ref:`schedulers built into RTEMS <SchedulingConcepts>`, the
 * configuration is straightforward.  All that is required is to define the
 * configuration option which specifies which scheduler you want for in your
 * application.
 *
 * The pluggable scheduler interface also enables the user to provide their own
 * scheduling algorithm.  If you choose to do this, you must define multiple
 * configuration option.
 *
 * @{
 */

/**
 * @brief The value of this configuration option defines the maximum number
 *   Constant Bandwidth Servers that can be concurrently active.
 *
 * This configuration option is an integer define.
 *
 * The default value is :ref:`CONFIGURE_MAXIMUM_TASKS`.
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be greater than or equal to 0.
 *
 * * It shall be less than or equal to ``SIZE_MAX``.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * * It shall be less than or equal to a BSP-specific and application-specific
 *   value which depends on the size of the memory available to the application.
 *
 * This configuration option is only evaluated if the configuration option
 * :ref:`CONFIGURE_SCHEDULER_CBS` is defined.
 */
#define CONFIGURE_CBS_MAXIMUM_SERVERS

/**
 * @brief For the following schedulers
 *
 * @brief * :ref:`SchedulerPriority`, which is the default in uniprocessor
 *     configurations and can be configured through the
 *     :ref:`CONFIGURE_SCHEDULER_PRIORITY` configuration option,
 *
 * @brief * :ref:`SchedulerSMPPriority` which can be configured through the
 *     :ref:`CONFIGURE_SCHEDULER_PRIORITY_SMP` configuration option, and
 *
 * @brief * :ref:`SchedulerSMPPriorityAffinity` which can be configured through
 *     the :ref:`CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP` configuration
 *     option
 *
 * @brief this configuration option specifies the maximum numeric priority of
 *   any task for these schedulers and one less that the number of priority
 *   levels for these schedulers.  For all other schedulers provided by RTEMS,
 *   this configuration option has no effect.
 *
 * This configuration option is an integer define.
 *
 * The default value is 255.
 *
 * The value of this configuration option shall be an element of {3, 7, 31, 63,
 * 127, 255}.
 *
 * The numerically greatest priority is the logically lowest priority in the
 * system and will thus be used by the IDLE task.
 *
 * Priority zero is reserved for internal use by RTEMS and is not available to
 * applications.
 *
 * Reducing the number of priorities through this configuration option reduces
 * the amount of memory allocated by the schedulers listed above.  These
 * schedulers use a chain control structure per priority and this structure
 * consists of three pointers.  On a 32-bit architecture, the allocated memory
 * is 12 bytes * (``CONFIGURE_MAXIMUM_PRIORITY`` + 1), e.g. 3072 bytes for 256
 * priority levels (default), 48 bytes for 4 priority levels
 * (``CONFIGURE_MAXIMUM_PRIORITY == 3``).
 *
 * The default value is 255, because RTEMS shall support 256 priority levels to
 * be compliant with various standards.  These priorities range from 0 to 255.
 */
#define CONFIGURE_MAXIMUM_PRIORITY

/**
 * @brief The value of this configuration option is used to initialize the
 *   initial scheduler to processor assignments.
 *
 * This configuration option is an initializer define.
 *
 * The default value of this configuration option is computed so that the default
 * scheduler is assigned to each configured processor (up to 32).
 *
 * The value of this configuration option shall satisfy all of the following
 * constraints:
 *
 * * It shall be a list of the following macros:
 *
 *   * ``RTEMS_SCHEDULER_ASSIGN( processor_index, attributes )``
 *
 *   * :c:macro:`RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER`
 *
 * * It shall be a list of exactly :ref:`CONFIGURE_MAXIMUM_PROCESSORS` elements.
 *
 * This configuration option is only evaluated in SMP configurations.
 *
 * This is an advanced configuration option, see
 * :ref:`ConfigurationSchedulersClustered`.
 */
#define CONFIGURE_SCHEDULER_ASSIGNMENTS

/**
 * @brief In case this configuration option is defined, then :ref:`Constant
 *   Bandwidth Server (CBS) Scheduler <SchedulerCBS>` algorithm is made
 *   available to the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * In case no explicit :ref:`clustered scheduler configuration
 * <ConfigurationSchedulersClustered>` is present, then it is used as the
 * scheduler for exactly one processor.
 */
#define CONFIGURE_SCHEDULER_CBS

/**
 * @brief In case this configuration option is defined, then :ref:`Earliest
 *   Deadline First (EDF) Scheduler <SchedulerEDF>` algorithm is made available
 *   to the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * In case no explicit :ref:`clustered scheduler configuration
 * <ConfigurationSchedulersClustered>` is present, then it is used as the
 * scheduler for exactly one processor.
 */
#define CONFIGURE_SCHEDULER_EDF

/**
 * @brief In case this configuration option is defined, then :ref:`Earliest
 *   Deadline First (EDF) SMP Scheduler <SchedulerSMPEDF>` algorithm is made
 *   available to the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * This scheduler algorithm is only available when RTEMS is built with SMP
 * support enabled.
 *
 * In case no explicit :ref:`clustered scheduler configuration
 * <ConfigurationSchedulersClustered>` is present, then it is used as the
 * scheduler for up to 32 processors.
 *
 * This scheduler algorithm is the default in SMP configurations if
 * :ref:`CONFIGURE_MAXIMUM_PROCESSORS` is greater than one.
 */
#define CONFIGURE_SCHEDULER_EDF_SMP

/**
 * @brief The value of this configuration option defines the name of the
 *   default scheduler.
 *
 * This configuration option is an integer define.
 *
 * The default value is
 *
 * * ``"MEDF"`` for the :ref:`EDF SMP Scheduler <SchedulerSMPEDF>`,
 *
 * * ``"MPA "`` for the :ref:`Arbitrary Processor Affinity Priority SMP Scheduler
 *   <SchedulerSMPPriorityAffinity>`,
 *
 * * ``"MPD "`` for the :ref:`Deterministic Priority SMP Scheduler
 *   <SchedulerSMPPriority>`,
 *
 * * ``"MPS "`` for the :ref:`Simple Priority SMP Scheduler
 *   <SchedulerSMPPrioritySimple>`,
 *
 * * ``"UCBS"`` for the :ref:`Uniprocessor CBS Scheduler <SchedulerCBS>`,
 *
 * * ``"UEDF"`` for the :ref:`Uniprocessor EDF Scheduler <SchedulerEDF>`,
 *
 * * ``"UPD "`` for the :ref:`Uniprocessor Deterministic Priority Scheduler
 *   <SchedulerPriority>`, and
 *
 * * ``"UPS "`` for the :ref:`Uniprocessor Simple Priority Scheduler
 *   <SchedulerPrioritySimple>`.
 *
 * The value of this configuration option shall be a valid integer of type
 * ``rtems_name``.
 *
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * Schedulers can be identified via c:func:`rtems_scheduler_ident`.
 *
 * Use :c:func:`rtems_build_name` to define the scheduler name.
 */
#define CONFIGURE_SCHEDULER_NAME

/**
 * @brief In case this configuration option is defined, then
 *   :ref:`Deterministic Priority Scheduler <SchedulerPriority>` algorithm is
 *   made available to the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * In case no explicit :ref:`clustered scheduler configuration
 * <ConfigurationSchedulersClustered>` is present, then it is used as the
 * scheduler for exactly one processor.
 *
 * This scheduler algorithm is the default when
 * :ref:`CONFIGURE_MAXIMUM_PROCESSORS` is exactly one.
 *
 * The memory allocated for this scheduler depends on the
 * :ref:`CONFIGURE_MAXIMUM_PRIORITY` configuration option.
 */
#define CONFIGURE_SCHEDULER_PRIORITY

/**
 * @brief In case this configuration option is defined, then :ref:`Arbitrary
 *   Processor Affinity SMP Scheduler <SchedulerSMPPriorityAffinity>` algorithm
 *   is made available to the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * This scheduler algorithm is only available when RTEMS is built with SMP
 * support enabled.
 *
 * In case no explicit :ref:`clustered scheduler configuration
 * <ConfigurationSchedulersClustered>` is present, then it is used as the
 * scheduler for up to 32 processors.
 *
 * The memory allocated for this scheduler depends on the
 * :ref:`CONFIGURE_MAXIMUM_PRIORITY` configuration option.
 */
#define CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP

/**
 * @brief In case this configuration option is defined, then
 *   :ref:`Deterministic Priority SMP Scheduler <SchedulerSMPPriority>`
 *   algorithm is made available to the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * This scheduler algorithm is only available when RTEMS is built with SMP
 * support enabled.
 *
 * In case no explicit :ref:`clustered scheduler configuration
 * <ConfigurationSchedulersClustered>` is present, then it is used as the
 * scheduler for up to 32 processors.
 *
 * The memory allocated for this scheduler depends on the
 * :ref:`CONFIGURE_MAXIMUM_PRIORITY` configuration option.
 */
#define CONFIGURE_SCHEDULER_PRIORITY_SMP

/**
 * @brief In case this configuration option is defined, then :ref:`Simple
 *   Priority Scheduler <SchedulerPrioritySimple>` algorithm is made available
 *   to the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * In case no explicit :ref:`clustered scheduler configuration
 * <ConfigurationSchedulersClustered>` is present, then it is used as the
 * scheduler for exactly one processor.
 */
#define CONFIGURE_SCHEDULER_SIMPLE

/**
 * @brief In case this configuration option is defined, then :ref:`Simple
 *   Priority SMP Scheduler <SchedulerSMPPrioritySimple>` algorithm is made
 *   available to the application. application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * This scheduler algorithm is only available when RTEMS is built with SMP
 * support enabled.
 *
 * In case no explicit :ref:`clustered scheduler configuration
 * <ConfigurationSchedulersClustered>` is present, then it is used as the
 * scheduler for up to 32 processors.
 */
#define CONFIGURE_SCHEDULER_SIMPLE_SMP

/**
 * @brief In case this configuration option is defined, then Strong APA
 *   algorithm is made available to the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * This scheduler algorithm is only available when RTEMS is built with SMP
 * support enabled.
 *
 * This scheduler algorithm is not correctly implemented.  Do not use it.
 */
#define CONFIGURE_SCHEDULER_STRONG_APA

/**
 * @brief In case this configuration option is defined, then the user shall
 *   provide a scheduler algorithm to the application.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * RTEMS allows the application to provide its own task/thread scheduling
 * algorithm. In order to do this, one shall define
 * ``CONFIGURE_SCHEDULER_USER`` to indicate the application provides its own
 * scheduling algorithm. If ``CONFIGURE_SCHEDULER_USER`` is defined then the
 * following additional macros shall be defined:
 *
 * * ``CONFIGURE_SCHEDULER`` shall be defined to a static definition of the
 *   scheduler data structures of the user scheduler.
 *
 * * ``CONFIGURE_SCHEDULER_TABLE_ENTRIES`` shall be defined to a scheduler
 *   table entry initializer for the user scheduler.
 *
 * * ``CONFIGURE_SCHEDULER_USER_PER_THREAD`` shall be defined to the type of
 *   the per-thread information of the user scheduler.
 *
 * At this time, the mechanics and requirements for writing a new scheduler are
 * evolving and not fully documented.  It is recommended that you look at the
 * existing Deterministic Priority Scheduler in
 * ``cpukit/score/src/schedulerpriority*.c`` for guidance.  For guidance on the
 * configuration macros, please examine ``cpukit/sapi/include/confdefs.h`` for
 * how these are defined for the Deterministic Priority Scheduler.
 */
#define CONFIGURE_SCHEDULER_USER

/** @} */

/**
 * @defgroup Task Stack Allocator Configuration Task Stack Allocator Configuration
 *
 * This section describes configuration options related to the task stack
 * allocator.  RTEMS allows the application or BSP to define its own allocation
 * and deallocation methods for task stacks. This can be used to place task
 * stacks in special areas of memory or to utilize a Memory Management Unit so
 * that stack overflows are detected in hardware.
 *
 * @{
 */

/**
 * @brief The value of this configuration option initializes the stack
 *   allocator allocate handler.
 *
 * This configuration option is an initializer define.
 *
 * The default value is ``_Workspace_Allocate``, which indicates that task stacks
 * will be allocated from the RTEMS Workspace.
 *
 * The value of this configuration option shall be defined to a valid function
 * pointer of the type ``void *( *allocate )( size_t )``.
 *
 * A correctly configured system shall configure the following to be
 * consistent:
 *
 * * :ref:`CONFIGURE_TASK_STACK_ALLOCATOR_INIT`
 *
 * * `CONFIGURE_TASK_STACK_ALLOCATOR`
 *
 * * :ref:`CONFIGURE_TASK_STACK_DEALLOCATOR`
 */
#define CONFIGURE_TASK_STACK_ALLOCATOR

/**
 * @brief The value of this configuration option initializes the stack
 *   allocator initialization handler.
 *
 * This configuration option is an initializer define.
 *
 * The default value is ``NULL``.
 *
 * The value of this configuration option shall be defined to a valid function
 * pointer of the type ``void ( *initialize )( size_t )`` or to ``NULL``.
 *
 * A correctly configured system shall configure the following to be
 * consistent:
 *
 * * `CONFIGURE_TASK_STACK_ALLOCATOR_INIT`
 *
 * * :ref:`CONFIGURE_TASK_STACK_ALLOCATOR`
 *
 * * :ref:`CONFIGURE_TASK_STACK_DEALLOCATOR`
 */
#define CONFIGURE_TASK_STACK_ALLOCATOR_INIT

/**
 * @brief The value of this configuration option initializes the stack
 *   allocator deallocate handler.
 *
 * This configuration option is an initializer define.
 *
 * The default value is ``_Workspace_Free``, which indicates that task stacks will
 * be allocated from the RTEMS Workspace.
 *
 * The value of this configuration option shall be defined to a valid function
 * pointer of the type ``void ( *deallocate )( void * )``.
 *
 * A correctly configured system shall configure the following to be
 * consistent:
 *
 * * :ref:`CONFIGURE_TASK_STACK_ALLOCATOR_INIT`
 *
 * * :ref:`CONFIGURE_TASK_STACK_ALLOCATOR`
 *
 * * `CONFIGURE_TASK_STACK_DEALLOCATOR`
 */
#define CONFIGURE_TASK_STACK_DEALLOCATOR

/**
 * @brief The value of this configuration option is used to calculate the task
 *   stack space size.
 *
 * This configuration option is an initializer define.
 *
 * The default value is a macro which supports the system heap allocator.
 *
 * The value of this configuration option shall be defined to a macro which
 * accepts exactly one parameter and returns an unsigned integer.  The parameter
 * will be an allocation size and the macro shall return this size plus the
 * overhead of the allocator to manage an allocation request for this size.
 *
 * This configuration option may be used if a custom task stack allocator is
 * configured, see :ref:`CONFIGURE_TASK_STACK_ALLOCATOR`.
 */
#define CONFIGURE_TASK_STACK_FROM_ALLOCATOR

/**
 * @brief In case this configuration option is defined, then the system is
 *   informed that the task stack allocator does not use the RTEMS Workspace.
 *
 * This configuration option is a boolean feature define.
 *
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * This configuration option may be used if a custom task stack allocator is
 * configured, see :ref:`CONFIGURE_TASK_STACK_ALLOCATOR`.
 */
#define CONFIGURE_TASK_STACK_ALLOCATOR_AVOIDS_WORK_SPACE

/** @} */
