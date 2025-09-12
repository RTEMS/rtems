/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplDoxygen
 *
 * @brief This header file documents the application configuration options.
 */

/*
 * Copyright (C) 2019, 2023 embedded brains GmbH & Co. KG
 * Copyright (C) 2010 Gedare Bloom
 * Copyright (C) 1988, 2023 On-Line Applications Research Corporation (OAR)
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

/**
 * @defgroup RTEMSApplConfig Application Configuration Options
 *
 * @ingroup RTEMSAPI
 */

/* Generated from spec:/acfg/if/group-bdbuf */

/**
 * @defgroup RTEMSApplConfigBlockDeviceCacheConfiguration \
 *   Block Device Cache Configuration
 *
 * @ingroup RTEMSApplConfig
 *
 * This section describes configuration options related to the Block Device
 * Cache (bdbuf).
 *
 * @{
 */

/* Generated from spec:/acfg/if/appl-needs-libblock */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
 *
 * In case this configuration option is defined, then the Block Device Cache is
 * initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * Each option of the Block Device Cache (bdbuf) configuration can be
 * explicitly set by the user with the configuration options below.  The Block
 * Device Cache is used for example by the RFS and DOSFS filesystems.
 */
#define CONFIGURE_APPLICATION_NEEDS_LIBBLOCK

/* Generated from spec:/acfg/if/bdbuf-buffer-max-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_BDBUF_BUFFER_MAX_SIZE
 *
 * The value of this configuration option defines the maximum size of a buffer
 * in bytes.
 *
 * @par Default Value
 * The default value is 4096.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be an integral multiple of
 *   @ref CONFIGURE_BDBUF_BUFFER_MIN_SIZE.
 * @endparblock
 */
#define CONFIGURE_BDBUF_BUFFER_MAX_SIZE

/* Generated from spec:/acfg/if/bdbuf-buffer-min-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_BDBUF_BUFFER_MIN_SIZE
 *
 * The value of this configuration option defines the minimum size of a buffer
 * in bytes.
 *
 * @par Default Value
 * The default value is 512.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/integer">UINT32_MAX</a>.
 * @endparblock
 */
#define CONFIGURE_BDBUF_BUFFER_MIN_SIZE

/* Generated from spec:/acfg/if/bdbuf-cache-memory-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_BDBUF_CACHE_MEMORY_SIZE
 *
 * The value of this configuration option defines the size of the cache memory
 * in bytes.
 *
 * @par Default Value
 * The default value is 32768.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/limits">SIZE_MAX</a>.
 * @endparblock
 */
#define CONFIGURE_BDBUF_CACHE_MEMORY_SIZE

/* Generated from spec:/acfg/if/bdbuf-max-read-ahead-blocks */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS
 *
 * The value of this configuration option defines the maximum blocks per
 * read-ahead request.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/integer">UINT32_MAX</a>.
 * @endparblock
 *
 * @par Notes
 * A value of 0 disables the read-ahead task (default).  The read-ahead task
 * will issue speculative read transfers if a sequential access pattern is
 * detected.  This can improve the performance on some systems.
 */
#define CONFIGURE_BDBUF_MAX_READ_AHEAD_BLOCKS

/* Generated from spec:/acfg/if/bdbuf-max-write-blocks */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_BDBUF_MAX_WRITE_BLOCKS
 *
 * The value of this configuration option defines the maximum blocks per write
 * request.
 *
 * @par Default Value
 * The default value is 16.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/integer">UINT32_MAX</a>.
 * @endparblock
 */
#define CONFIGURE_BDBUF_MAX_WRITE_BLOCKS

/* Generated from spec:/acfg/if/bdbuf-read-ahead-task-priority */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_BDBUF_READ_AHEAD_TASK_PRIORITY
 *
 * The value of this configuration option defines the read-ahead task priority.
 *
 * @par Default Value
 * The default value is 15.
 *
 * @par Constraints
 * The value of the configuration option shall be a valid Classic API task
 * priority.  The set of valid task priorities depends on the scheduler
 * configuration.
 */
#define CONFIGURE_BDBUF_READ_AHEAD_TASK_PRIORITY

/* Generated from spec:/acfg/if/bdbuf-task-stack-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_BDBUF_TASK_STACK_SIZE
 *
 * The value of this configuration option defines the task stack size of the
 * Block Device Cache tasks in bytes.
 *
 * @par Default Value
 * The default value is #RTEMS_MINIMUM_STACK_SIZE.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   @ref CONFIGURE_MINIMUM_TASK_STACK_SIZE.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option shall be small enough so that the
 *   task stack space calculation carried out by ``<rtems/confdefs.h>`` does
 *   not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/integer">uintptr_t</a>.
 * @endparblock
 */
#define CONFIGURE_BDBUF_TASK_STACK_SIZE

/* Generated from spec:/acfg/if/bdbuf-swapout-block-hold */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_SWAPOUT_BLOCK_HOLD
 *
 * The value of this configuration option defines the swapout task maximum
 * block hold time in milliseconds.
 *
 * @par Default Value
 * The default value is 1000.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/integer">UINT32_MAX</a>.
 * @endparblock
 */
#define CONFIGURE_SWAPOUT_BLOCK_HOLD

/* Generated from spec:/acfg/if/bdbuf-swapout-swap-period */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_SWAPOUT_SWAP_PERIOD
 *
 * The value of this configuration option defines the swapout task swap period
 * in milliseconds.
 *
 * @par Default Value
 * The default value is 250.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/integer">UINT32_MAX</a>.
 * @endparblock
 */
#define CONFIGURE_SWAPOUT_SWAP_PERIOD

/* Generated from spec:/acfg/if/bdbuf-swapout-task-priority */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_SWAPOUT_TASK_PRIORITY
 *
 * The value of this configuration option defines the swapout task priority.
 *
 * @par Default Value
 * The default value is 15.
 *
 * @par Constraints
 * The value of the configuration option shall be a valid Classic API task
 * priority.  The set of valid task priorities depends on the scheduler
 * configuration.
 */
#define CONFIGURE_SWAPOUT_TASK_PRIORITY

/* Generated from spec:/acfg/if/bdbuf-swapout-worker-tasks */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_SWAPOUT_WORKER_TASKS
 *
 * The value of this configuration option defines the swapout worker task
 * count.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/integer">UINT32_MAX</a>.
 * @endparblock
 */
#define CONFIGURE_SWAPOUT_WORKER_TASKS

/* Generated from spec:/acfg/if/bdbuf-swapout-worker-taskp-riority */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_SWAPOUT_WORKER_TASK_PRIORITY
 *
 * The value of this configuration option defines the swapout worker task
 * priority.
 *
 * @par Default Value
 * The default value is 15.
 *
 * @par Constraints
 * The value of the configuration option shall be a valid Classic API task
 * priority.  The set of valid task priorities depends on the scheduler
 * configuration.
 */
#define CONFIGURE_SWAPOUT_WORKER_TASK_PRIORITY

/** @} */

/* Generated from spec:/acfg/if/group-classic */

/**
 * @defgroup RTEMSApplConfigClassicAPIConfiguration Classic API Configuration
 *
 * @ingroup RTEMSApplConfig
 *
 * This section describes configuration options related to the Classic API.
 *
 * @{
 */

/* Generated from spec:/acfg/if/max-barriers */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_BARRIERS
 *
 * The value of this configuration option defines the maximum number of Classic
 * API Barriers that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 * @endparblock
 *
 * @par Notes
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.
 */
#define CONFIGURE_MAXIMUM_BARRIERS

/* Generated from spec:/acfg/if/max-message-queues */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_MESSAGE_QUEUES
 *
 * The value of this configuration option defines the maximum number of Classic
 * API Message Queues that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 * @endparblock
 *
 * @par Notes
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.  You have to account for the memory used to store the messages
 * of each message queue, see @ref CONFIGURE_MESSAGE_BUFFER_MEMORY.
 */
#define CONFIGURE_MAXIMUM_MESSAGE_QUEUES

/* Generated from spec:/acfg/if/max-partitions */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_PARTITIONS
 *
 * The value of this configuration option defines the maximum number of Classic
 * API Partitions that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 * @endparblock
 *
 * @par Notes
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.
 */
#define CONFIGURE_MAXIMUM_PARTITIONS

/* Generated from spec:/acfg/if/max-periods */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_PERIODS
 *
 * The value of this configuration option defines the maximum number of Classic
 * API Periods that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 * @endparblock
 *
 * @par Notes
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.
 */
#define CONFIGURE_MAXIMUM_PERIODS

/* Generated from spec:/acfg/if/max-ports */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_PORTS
 *
 * The value of this configuration option defines the maximum number of Classic
 * API Ports that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 * @endparblock
 *
 * @par Notes
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.
 */
#define CONFIGURE_MAXIMUM_PORTS

/* Generated from spec:/acfg/if/max-regions */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_REGIONS
 *
 * The value of this configuration option defines the maximum number of Classic
 * API Regions that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 * @endparblock
 *
 * @par Notes
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.
 */
#define CONFIGURE_MAXIMUM_REGIONS

/* Generated from spec:/acfg/if/max-semaphores */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_SEMAPHORES
 *
 * The value of this configuration option defines the maximum number of Classic
 * API Semaphore that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.
 *
 * In SMP configurations, the size of a Semaphore Control Block depends on the
 * scheduler count (see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/scheduler-clustered.html#configuration-step-3-scheduler-table">Configuration
 * Step 3 - Scheduler Table</a>).  The semaphores using the <a
 * href="https://docs.rtems.org/branches/master/c-user/key_concepts.html#multiprocessor-resource-sharing-protocol-mrsp">Multiprocessor
 * Resource Sharing Protocol (MrsP)</a> need a ceiling priority per scheduler.
 * @endparblock
 */
#define CONFIGURE_MAXIMUM_SEMAPHORES

/* Generated from spec:/acfg/if/max-tasks */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_TASKS
 *
 * The value of this configuration option defines the maximum number of Classic
 * API Tasks that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option shall be small enough so that the
 *   task stack space calculation carried out by ``<rtems/confdefs.h>`` does
 *   not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/integer">uintptr_t</a>.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.
 *
 * The calculations for the required memory in the RTEMS Workspace for tasks
 * assume that each task has a minimum stack size and has floating point
 * support enabled.  The configuration option @ref CONFIGURE_EXTRA_TASK_STACKS
 * is used to specify task stack requirements *above* the minimum size
 * required.
 *
 * The maximum number of POSIX threads is specified by @ref
 * CONFIGURE_MAXIMUM_POSIX_THREADS.
 *
 * A future enhancement to ``<rtems/confdefs.h>`` could be to eliminate the
 * assumption that all tasks have floating point enabled. This would require
 * the addition of a new configuration parameter to specify the number of tasks
 * which enable floating point support.
 * @endparblock
 */
#define CONFIGURE_MAXIMUM_TASKS

/* Generated from spec:/acfg/if/max-timers */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_TIMERS
 *
 * The value of this configuration option defines the maximum number of Classic
 * API Timers that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 * @endparblock
 *
 * @par Notes
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.
 */
#define CONFIGURE_MAXIMUM_TIMERS

/* Generated from spec:/acfg/if/max-user-extensions */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_USER_EXTENSIONS
 *
 * The value of this configuration option defines the maximum number of Classic
 * API User Extensions that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 * @endparblock
 *
 * @par Notes
 * This object class cannot be configured in unlimited allocation mode.
 */
#define CONFIGURE_MAXIMUM_USER_EXTENSIONS

/* Generated from spec:/acfg/if/min-tasks-with-user-provided-storage */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MINIMUM_TASKS_WITH_USER_PROVIDED_STORAGE
 *
 * The value of this configuration option defines the minimum count of Classic
 * API Tasks which are constructed by rtems_task_construct().
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to @ref
 *   CONFIGURE_MAXIMUM_TASKS.
 * @endparblock
 *
 * @par Notes
 * By default, the calculation for the required memory in the RTEMS Workspace
 * for tasks assumes that all Classic API Tasks are created by
 * rtems_task_create().  This configuration option can be used to reduce the
 * required memory for the system-provided task storage areas since tasks
 * constructed by rtems_task_construct() use a user-provided task storage area.
 */
#define CONFIGURE_MINIMUM_TASKS_WITH_USER_PROVIDED_STORAGE

/** @} */

/* Generated from spec:/acfg/if/group-classicinit */

/**
 * @defgroup RTEMSApplConfigClassicAPIInitializationTaskConfiguration \
 *   Classic API Initialization Task Configuration
 *
 * @ingroup RTEMSApplConfig
 *
 * This section describes configuration options related to the Classic API
 * initialization task.
 *
 * @{
 */

/* Generated from spec:/acfg/if/init-task-arguments */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_INIT_TASK_ARGUMENTS
 *
 * The value of this configuration option defines task argument of the Classic
 * API initialization task.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * The value of the configuration option shall be convertible to an integer of
 * type ::rtems_task_argument.
 */
#define CONFIGURE_INIT_TASK_ARGUMENTS

/* Generated from spec:/acfg/if/init-task-attributes */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_INIT_TASK_ATTRIBUTES
 *
 * The value of this configuration option defines the task attributes of the
 * Classic API initialization task.
 *
 * @par Default Value
 * The default value is #RTEMS_DEFAULT_ATTRIBUTES.
 *
 * @par Constraints
 * The value of the configuration option shall be a valid task attribute set.
 */
#define CONFIGURE_INIT_TASK_ATTRIBUTES

/* Generated from spec:/acfg/if/init-task-construct-storage-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_INIT_TASK_CONSTRUCT_STORAGE_SIZE
 *
 * The value of this configuration option defines the task storage size of the
 * Classic API initialization task.
 *
 * @par Default Value
 * This configuration option has no default value.  If it is not specified,
 * then the Classic API initialization task will be created with the stack size
 * defined by the @ref CONFIGURE_INIT_TASK_STACK_SIZE configuration option.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   @ref CONFIGURE_MINIMUM_TASK_STACK_SIZE.
 *
 * - The value of the configuration option shall be defined using
 *   RTEMS_TASK_STORAGE_SIZE().
 * @endparblock
 *
 * @par Notes
 * @parblock
 * If this configuration option is specified, then
 *
 * * a task storage area of the specified size is statically allocated by
 *   ``<rtems/confdefs.h>`` for the Classic API initialization task,
 *
 * * the Classic API initialization task is constructed by
 *   rtems_task_construct() instead of using rtems_task_create(),
 *
 * * the maximum thread-local storage size defined by @ref
 *   CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE is used for the Classic API
 *   initialization task,
 *
 * * the Classic API initialization task should be accounted for in @ref
 *   CONFIGURE_MINIMUM_TASKS_WITH_USER_PROVIDED_STORAGE, and
 *
 * * the task storage area used for the Classic API initialization task is not
 *   reclaimed by the system if the task is deleted.
 *
 * The
 *
 * * @ref CONFIGURE_INIT_TASK_STACK_SIZE and
 *
 * * `CONFIGURE_INIT_TASK_CONSTRUCT_STORAGE_SIZE`
 *
 * configuration options are mutually exclusive.
 * @endparblock
 */
#define CONFIGURE_INIT_TASK_CONSTRUCT_STORAGE_SIZE

/* Generated from spec:/acfg/if/init-task-entrypoint */

/**
 * @brief This configuration option is an initializer define.
 *
 * @anchor CONFIGURE_INIT_TASK_ENTRY_POINT
 *
 * The value of this configuration option initializes the entry point of the
 * Classic API initialization task.
 *
 * @par Default Value
 * The default value is `Init`.
 *
 * @par Constraints
 * The value of the configuration option shall be defined to a valid function
 * pointer of the type `void ( *entry_point )( rtems_task_argument )`.
 *
 * @par Notes
 * The application shall provide the function referenced by this configuration
 * option.
 */
#define CONFIGURE_INIT_TASK_ENTRY_POINT

/* Generated from spec:/acfg/if/init-task-initial-modes */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_INIT_TASK_INITIAL_MODES
 *
 * The value of this configuration option defines the initial execution mode of
 * the Classic API initialization task.
 *
 * @par Default Value
 * In SMP  configurations, the default value is #RTEMS_DEFAULT_MODES otherwise
 * the default value is #RTEMS_NO_PREEMPT.
 *
 * @par Constraints
 * The value of the configuration option shall be a valid task mode set.
 */
#define CONFIGURE_INIT_TASK_INITIAL_MODES

/* Generated from spec:/acfg/if/init-task-name */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_INIT_TASK_NAME
 *
 * The value of this configuration option defines the name of the Classic API
 * initialization task.
 *
 * @par Default Value
 * The default value is `rtems_build_name( 'U', 'I', '1', ' ' )`.
 *
 * @par Constraints
 * The value of the configuration option shall be convertible to an integer of
 * type ::rtems_name.
 *
 * @par Notes
 * Use rtems_build_name() to define the task name.
 */
#define CONFIGURE_INIT_TASK_NAME

/* Generated from spec:/acfg/if/init-task-priority */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_INIT_TASK_PRIORITY
 *
 * The value of this configuration option defines the initial priority of the
 * Classic API initialization task.
 *
 * @par Default Value
 * The default value is 1.
 *
 * @par Constraints
 * The value of the configuration option shall be a valid Classic API task
 * priority.  The set of valid task priorities depends on the scheduler
 * configuration.
 */
#define CONFIGURE_INIT_TASK_PRIORITY

/* Generated from spec:/acfg/if/init-task-stack-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_INIT_TASK_STACK_SIZE
 *
 * The value of this configuration option defines the task stack size of the
 * Classic API initialization task.
 *
 * @par Default Value
 * The default value is @ref CONFIGURE_MINIMUM_TASK_STACK_SIZE.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   @ref CONFIGURE_MINIMUM_TASK_STACK_SIZE.
 *
 * - The value of the configuration option shall be small enough so that the
 *   task stack space calculation carried out by ``<rtems/confdefs.h>`` does
 *   not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/integer">uintptr_t</a>.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * The
 *
 * * `CONFIGURE_INIT_TASK_STACK_SIZE` and
 *
 * * @ref CONFIGURE_INIT_TASK_CONSTRUCT_STORAGE_SIZE
 *
 * configuration options are mutually exclusive.
 * @endparblock
 */
#define CONFIGURE_INIT_TASK_STACK_SIZE

/* Generated from spec:/acfg/if/rtems-init-tasks-table */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_RTEMS_INIT_TASKS_TABLE
 *
 * In case this configuration option is defined, then exactly one Classic API
 * initialization task is configured.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * The application shall define at least one of the following configuration
 * options
 *
 * * `CONFIGURE_RTEMS_INIT_TASKS_TABLE`,
 *
 * * @ref CONFIGURE_POSIX_INIT_THREAD_TABLE, or
 *
 * * @ref CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION
 *
 * otherwise a compile time error in the configuration file will occur.
 *
 * The Classic API initialization task performs the <a
 * href="https://docs.rtems.org/branches/master/c-user/initialization/operations.html">Global
 * Construction</a>.
 * @endparblock
 */
#define CONFIGURE_RTEMS_INIT_TASKS_TABLE

/** @} */

/* Generated from spec:/acfg/if/group-devdrv */

/**
 * @defgroup RTEMSApplConfigDeviceDriverConfiguration \
 *   Device Driver Configuration
 *
 * @ingroup RTEMSApplConfig
 *
 * This section describes configuration options related to the device drivers.
 * Note that network device drivers are not covered by the following options.
 *
 * @{
 */

/* Generated from spec:/acfg/if/appl-does-not-need-clock-driver */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER
 *
 * In case this configuration option is defined, then **no** Clock Driver is
 * initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then a Clock Driver may be
 * initialized during system initialization.
 *
 * @par Notes
 * @parblock
 * This configuration parameter is intended to prevent the common user error of
 * using the Hello World example as the baseline for an application and leaving
 * out a clock tick source.
 *
 * The application shall define exactly one of the following configuration
 * options
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER,
 *
 * * `CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER`, or
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER,
 *
 * otherwise a compile time error in the configuration file will occur.
 * @endparblock
 */
#define CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER

/* Generated from spec:/acfg/if/appl-extra-drivers */

/**
 * @brief This configuration option is an initializer define.
 *
 * @anchor CONFIGURE_APPLICATION_EXTRA_DRIVERS
 *
 * The value of this configuration option is used to initialize the Device
 * Driver Table.
 *
 * @par Default Value
 * The default value is the empty list.
 *
 * @par Constraints
 * The value of the configuration option shall be a list of initializers for
 * structures of type ::rtems_driver_address_table.
 *
 * @par Notes
 * @parblock
 * The value of this configuration option is placed after the entries of other
 * device driver configuration options.
 *
 * See @ref CONFIGURE_APPLICATION_PREREQUISITE_DRIVERS for an alternative
 * placement of application device driver initializers.
 * @endparblock
 */
#define CONFIGURE_APPLICATION_EXTRA_DRIVERS

/* Generated from spec:/acfg/if/appl-needs-ata-driver */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
 *
 * In case this configuration option is defined, then the ATA Driver is
 * initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * Most BSPs do not include support for an ATA Driver.
 *
 * If this option is defined and the BSP does not have this device driver, then
 * the user will get a link time error for an undefined symbol.
 * @endparblock
 */
#define CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER

/* Generated from spec:/acfg/if/appl-needs-clock-driver */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
 *
 * In case this configuration option is defined, then the Clock Driver is
 * initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * The Clock Driver is responsible for providing a regular interrupt which
 * invokes a clock tick directive.
 *
 * The application shall define exactly one of the following configuration
 * options
 *
 * * `CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER`,
 *
 * * @ref CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER, or
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER,
 *
 * otherwise a compile time error in the configuration file will occur.
 * @endparblock
 */
#define CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER

/* Generated from spec:/acfg/if/appl-needs-console-driver */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
 *
 * In case this configuration option is defined, then the Console Driver is
 * initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * The Console Driver is responsible for providing the `/dev/console` device
 * file.  This device is used to initialize the standard input, output, and
 * error file descriptors.
 *
 * BSPs should be constructed in a manner that allows printk() to work properly
 * without the need for the Console Driver to be configured.
 *
 * The
 *
 * * `CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER`,
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER, and
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER
 *
 * configuration options are mutually exclusive.
 * @endparblock
 */
#define CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER

/* Generated from spec:/acfg/if/appl-needs-framebuffer-driver */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_NEEDS_FRAME_BUFFER_DRIVER
 *
 * In case this configuration option is defined, then the Frame Buffer Driver
 * is initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * Most BSPs do not include support for a Frame Buffer Driver. This is because
 * many boards do not include the required hardware.
 *
 * If this option is defined and the BSP does not have this device driver, then
 * the user will get a link time error for an undefined symbol.
 * @endparblock
 */
#define CONFIGURE_APPLICATION_NEEDS_FRAME_BUFFER_DRIVER

/* Generated from spec:/acfg/if/appl-needs-ide-driver */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_NEEDS_IDE_DRIVER
 *
 * In case this configuration option is defined, then the IDE Driver is
 * initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * Most BSPs do not include support for an IDE Driver.
 *
 * If this option is defined and the BSP does not have this device driver, then
 * the user will get a link time error for an undefined symbol.
 * @endparblock
 */
#define CONFIGURE_APPLICATION_NEEDS_IDE_DRIVER

/* Generated from spec:/acfg/if/appl-needs-null-driver */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_NEEDS_NULL_DRIVER
 *
 * In case this configuration option is defined, then the `/dev/null` Driver is
 * initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * This device driver is supported by all BSPs.
 */
#define CONFIGURE_APPLICATION_NEEDS_NULL_DRIVER

/* Generated from spec:/acfg/if/appl-needs-rtc-driver */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER
 *
 * In case this configuration option is defined, then the Real-Time Clock
 * Driver is initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * Most BSPs do not include support for a real-time clock (RTC). This is
 * because many boards do not include the required hardware.
 *
 * If this is defined and the BSP does not have this device driver, then the
 * user will get a link time error for an undefined symbol.
 * @endparblock
 */
#define CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER

/* Generated from spec:/acfg/if/appl-needs-simple-console-driver */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
 *
 * In case this configuration option is defined, then the Simple Console Driver
 * is initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * This device driver is responsible for providing the `/dev/console` device
 * file.  This device is used to initialize the standard input, output, and
 * error file descriptors.
 *
 * This device driver reads via getchark().
 *
 * This device driver writes via rtems_putc().
 *
 * The Termios framework is not used.  There is no support to change device
 * settings, e.g. baud, stop bits, parity, etc.
 *
 * The
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER,
 *
 * * `CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER`, and
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER
 *
 * configuration options are mutually exclusive.
 * @endparblock
 */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER

/* Generated from spec:/acfg/if/appl-needs-simple-task-console-driver */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER
 *
 * In case this configuration option is defined, then the Simple Task Console
 * Driver is initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * This device driver is responsible for providing the `/dev/console` device
 * file.  This device is used to initialize the standard input, output, and
 * error file descriptors.
 *
 * This device driver reads via getchark().
 *
 * This device driver writes into a write buffer.  The count of characters
 * written into the write buffer is returned.  It might be less than the
 * requested count, in case the write buffer is full.  The write is
 * non-blocking and may be called from interrupt context.  A dedicated task
 * reads from the write buffer and outputs the characters via rtems_putc().
 * This task runs with the least important priority. The write buffer size is
 * 2047 characters and it is not configurable.
 *
 * Use `fsync( STDOUT_FILENO )` or `fdatasync( STDOUT_FILENO )` to drain the
 * write buffer.
 *
 * The Termios framework is not used.  There is no support to change device
 * settings, e.g.  baud, stop bits, parity, etc.
 *
 * The
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER,
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER, and
 *
 * * `CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER`
 *
 * configuration options are mutually exclusive.
 * @endparblock
 */
#define CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER

/* Generated from spec:/acfg/if/appl-needs-stub-driver */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
 *
 * In case this configuration option is defined, then the Stub Driver is
 * initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * This device driver simply provides entry points that return successful and
 * is primarily a test fixture. It is supported by all BSPs.
 */
#define CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER

/* Generated from spec:/acfg/if/appl-needs-timer-driver */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER
 *
 * In case this configuration option is defined, then the Benchmark Timer
 * Driver is initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * The Benchmark Timer Driver is intended for the benchmark tests of the RTEMS
 * Testsuite.  Applications should not use this driver.
 *
 * The application shall define exactly one of the following configuration
 * options
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER,
 *
 * * @ref CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER, or
 *
 * * `CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER`,
 *
 * otherwise a compile time error will occur.
 * @endparblock
 */
#define CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER

/* Generated from spec:/acfg/if/appl-needs-watchdog-driver */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_NEEDS_WATCHDOG_DRIVER
 *
 * In case this configuration option is defined, then the Watchdog Driver is
 * initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * Most BSPs do not include support for a watchdog device driver. This is
 * because many boards do not include the required hardware.
 *
 * If this is defined and the BSP does not have this device driver, then the
 * user will get a link time error for an undefined symbol.
 * @endparblock
 */
#define CONFIGURE_APPLICATION_NEEDS_WATCHDOG_DRIVER

/* Generated from spec:/acfg/if/appl-needs-zero-driver */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER
 *
 * In case this configuration option is defined, then the `/dev/zero` Driver is
 * initialized during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * This device driver is supported by all BSPs.
 */
#define CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER

/* Generated from spec:/acfg/if/appl-prerequisite-drivers */

/**
 * @brief This configuration option is an initializer define.
 *
 * @anchor CONFIGURE_APPLICATION_PREREQUISITE_DRIVERS
 *
 * The value of this configuration option is used to initialize the Device
 * Driver Table.
 *
 * @par Default Value
 * The default value is the empty list.
 *
 * @par Constraints
 * The value of the configuration option shall be a list of initializers for
 * structures of type ::rtems_driver_address_table.
 *
 * @par Notes
 * @parblock
 * The value of this configuration option is placed after the entries defined
 * by #CONFIGURE_BSP_PREREQUISITE_DRIVERS and before all other device driver
 * configuration options.
 *
 * See @ref CONFIGURE_APPLICATION_EXTRA_DRIVERS for an alternative placement of
 * application device driver initializers.
 * @endparblock
 */
#define CONFIGURE_APPLICATION_PREREQUISITE_DRIVERS

/* Generated from spec:/acfg/if/ata-driver-task-priority */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_ATA_DRIVER_TASK_PRIORITY
 *
 * The value of this configuration option defines the ATA task priority.
 *
 * @par Default Value
 * The default value is 140.
 *
 * @par Constraints
 * The value of the configuration option shall be a valid Classic API task
 * priority.  The set of valid task priorities depends on the scheduler
 * configuration.
 *
 * @par Notes
 * This configuration option is only evaluated if the configuration option @ref
 * CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER is defined.
 */
#define CONFIGURE_ATA_DRIVER_TASK_PRIORITY

/* Generated from spec:/acfg/if/exception-to-signal-mapping */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_EXCEPTION_TO_SIGNAL_MAPPING
 *
 * In case this configuration option is defined, then the machine exception to
 * POSIX signal mapping is configured during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * This device driver is responsible for setting up a mapping from machine
 * exceptions to POSIX signals so that applications may consume them and alter
 * task execution as necessary.
 *
 * This is especially useful for applications written in Ada or C++.
 * @endparblock
 */
#define CONFIGURE_EXCEPTION_TO_SIGNAL_MAPPING

/* Generated from spec:/acfg/if/max-drivers */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_DRIVERS
 *
 * The value of this configuration option defines the number of device drivers.
 *
 * @par Default Value
 * @parblock
 * This is computed by default, and is set to the number of statically
 * configured device drivers configured using the following configuration
 * options:
 *
 * * @ref CONFIGURE_APPLICATION_EXTRA_DRIVERS
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_ATA_DRIVER
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_CLOCK_DRIVER
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_CONSOLE_DRIVER
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_FRAME_BUFFER_DRIVER
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_IDE_DRIVER
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_LIBBLOCK
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_NULL_DRIVER
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_RTC_DRIVER
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_SIMPLE_CONSOLE_DRIVER
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_SIMPLE_TASK_CONSOLE_DRIVER
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_STUB_DRIVER
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_TIMER_DRIVER
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_WATCHDOG_DRIVER
 *
 * * @ref CONFIGURE_APPLICATION_NEEDS_ZERO_DRIVER
 *
 * * @ref CONFIGURE_APPLICATION_PREREQUISITE_DRIVERS
 *
 * If the @ref CONFIGURE_DISABLE_BSP_SETTINGS configuration option is not
 * defined and the BSP provides #CONFIGURE_BSP_PREREQUISITE_DRIVERS, then the
 * BSP-provided prerequisite device drivers are also taken into account.
 * @endparblock
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/limits">SIZE_MAX</a>.
 *
 * - The value of the configuration option shall be greater than or equal than
 *   the number of statically configured device drivers.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 * @endparblock
 *
 * @par Notes
 * If the application will dynamically install device drivers, then the
 * configuration option value shall be larger than the number of statically
 * configured device drivers.
 */
#define CONFIGURE_MAXIMUM_DRIVERS

/** @} */

/* Generated from spec:/acfg/if/group-eventrecord */

/**
 * @defgroup RTEMSApplConfigEventRecordingConfiguration \
 *   Event Recording Configuration
 *
 * @ingroup RTEMSApplConfig
 *
 * This section describes configuration options related to the event recording.
 *
 * @{
 */

/* Generated from spec:/acfg/if/record-extensions-enabled */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_RECORD_EXTENSIONS_ENABLED
 *
 * In case
 *
 * * this configuration option is defined
 *
 * * and @ref CONFIGURE_RECORD_PER_PROCESSOR_ITEMS is properly defined,
 *
 * then the event record extensions are enabled.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * The record extensions capture thread create, start, restart, delete, switch,
 * begin, exitted and terminate events.
 */
#define CONFIGURE_RECORD_EXTENSIONS_ENABLED

/* Generated from spec:/acfg/if/record-fatal-dump-base64 */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_RECORD_FATAL_DUMP_BASE64
 *
 * In case
 *
 * * this configuration option is defined
 *
 * * and @ref CONFIGURE_RECORD_PER_PROCESSOR_ITEMS is properly defined,
 *
 * * and @ref CONFIGURE_RECORD_FATAL_DUMP_BASE64_ZLIB is undefined,
 *
 * then the event records are dumped in Base64 encoding in a fatal error
 * extension (see <a
 * href="https://docs.rtems.org/branches/master/c-user/fatal-error/background.html#terminate">System
 * Termination Procedure</a>).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * This extension can be used to produce crash dumps.
 */
#define CONFIGURE_RECORD_FATAL_DUMP_BASE64

/* Generated from spec:/acfg/if/record-fatal-dump-base64-zlib */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_RECORD_FATAL_DUMP_BASE64_ZLIB
 *
 * In case
 *
 * * this configuration option is defined
 *
 * * and @ref CONFIGURE_RECORD_PER_PROCESSOR_ITEMS is properly defined,
 *
 * then the event records are compressed by zlib and dumped in Base64 encoding
 * in a fatal error extension (see <a
 * href="https://docs.rtems.org/branches/master/c-user/fatal-error/background.html#terminate">System
 * Termination Procedure</a>).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * The zlib compression needs about 512KiB of RAM.  This extension can be used
 * to produce crash dumps.
 */
#define CONFIGURE_RECORD_FATAL_DUMP_BASE64_ZLIB

/* Generated from spec:/acfg/if/record-interrupts-enabled */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_RECORD_INTERRUPTS_ENABLED
 *
 * In case
 *
 * * this configuration option is defined
 *
 * * and @ref CONFIGURE_RECORD_PER_PROCESSOR_ITEMS is properly defined,
 *
 * then the interrupt event recording is enabled.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * The interrupt event recording generates interrupt entry and exit events when
 * interrupt entries are dispatched.
 */
#define CONFIGURE_RECORD_INTERRUPTS_ENABLED

/* Generated from spec:/acfg/if/record-per-processor-items */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_RECORD_PER_PROCESSOR_ITEMS
 *
 * The value of this configuration option defines the event record item count
 * per processor.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   16.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/limits">SIZE_MAX</a>.
 *
 * - The value of the configuration option shall be a power of two.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 * @endparblock
 *
 * @par Notes
 * The event record buffers are statically allocated for each configured
 * processor (@ref CONFIGURE_MAXIMUM_PROCESSORS).  If the value of this
 * configuration option is zero, then nothing is allocated.
 */
#define CONFIGURE_RECORD_PER_PROCESSOR_ITEMS

/** @} */

/* Generated from spec:/acfg/if/group-face */

/**
 * @defgroup RTEMSApplConfigFACETechnicalStandardRelatedConfiguration \
 *   FACE Technical Standard Related Configuration
 *
 * @ingroup RTEMSApplConfig
 *
 * This section describes configuration options related to adapting RTEMS
 * behavior to be aligned with the FACE Technical Standard. The FACE Technical
 * Standard is a product of the FACE Consortium which operates under the Open
 * Group. The FACE Consortium was founded by avionics organizations to improve
 * the portability of cockpit software across various platforms. It addresses
 * technical and business concerns.
 *
 * Most important from an RTEMS perspective, the FACE Technical Standard
 * defines four POSIX profiles: Security, Safety Base, Safety Extended, and the
 * General Purpose Profile. Each has an increasingly larger subset of POSIX
 * APIs. In the Security and Safety profiles, ARINC 653 is required. It is
 * optional in the General Purpose Profile.
 *
 * The RTEMS Project has been tracking alignment with the FACE POSIX profiles
 * and they are included in the "RTEMS POSIX 1003.1 Compliance Guide."
 *
 * @{
 */

/* Generated from spec:/acfg/if/posix-timer-face-behavior */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_POSIX_TIMERS_FACE_BEHAVIOR
 *
 * If this configuration option is defined, then POSIX timers may not be
 * created to use the CLOCK_REALTIME.  Per POSIX, this is allowed behavior but
 * per the FACE Technical Standard, it is not. Using POSIX timers based on
 * CLOCK_REALTIME (e.g., time of day) is unsafe for real-time safety systems as
 * setting CLOCK_REALTIME will perturb any active timers.
 *
 * If this option is not defined, POSIX timers may be created to use the
 * CLOCK_REALTIME in compliance with the POSIX specification.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 */
#define CONFIGURE_POSIX_TIMERS_FACE_BEHAVIOR

/** @} */

/* Generated from spec:/acfg/if/group-filesystem */

/**
 * @defgroup RTEMSApplConfigFilesystemConfiguration Filesystem Configuration
 *
 * @ingroup RTEMSApplConfig
 *
 * This section describes configuration options related to filesytems. By
 * default, the In-Memory Filesystem (IMFS) is used as the base filesystem
 * (also known as root filesystem).  In order to save some memory for your
 * application, you can disable the filesystem support with the @ref
 * CONFIGURE_APPLICATION_DISABLE_FILESYSTEM configuration option.
 * Alternatively, you can strip down the features of the base filesystem with
 * the @ref CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM and @ref
 * CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM configuration options.  These three
 * configuration options are mutually exclusive.  They are intended for an
 * advanced application configuration.
 *
 * Features of the IMFS can be disabled and enabled with the following
 * configuration options:
 *
 * * @ref CONFIGURE_IMFS_DISABLE_CHMOD
 *
 * * @ref CONFIGURE_IMFS_DISABLE_CHOWN
 *
 * * @ref CONFIGURE_IMFS_DISABLE_LINK
 *
 * * @ref CONFIGURE_IMFS_DISABLE_MKNOD
 *
 * * @ref CONFIGURE_IMFS_DISABLE_MKNOD_FILE
 *
 * * @ref CONFIGURE_IMFS_DISABLE_MOUNT
 *
 * * @ref CONFIGURE_IMFS_DISABLE_READDIR
 *
 * * @ref CONFIGURE_IMFS_DISABLE_READLINK
 *
 * * @ref CONFIGURE_IMFS_DISABLE_RENAME
 *
 * * @ref CONFIGURE_IMFS_DISABLE_RMNOD
 *
 * * @ref CONFIGURE_IMFS_DISABLE_SYMLINK
 *
 * * @ref CONFIGURE_IMFS_DISABLE_UNMOUNT
 *
 * * @ref CONFIGURE_IMFS_DISABLE_UTIME
 *
 * * @ref CONFIGURE_IMFS_ENABLE_MKFIFO
 *
 * @{
 */

/* Generated from spec:/acfg/if/appl-disable-filesystem */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_APPLICATION_DISABLE_FILESYSTEM
 *
 * In case this configuration option is defined, then **no base filesystem** is
 * initialized during system initialization and **no filesystems** are
 * configured.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then a base filesystem and the
 * configured filesystems are initialized during system initialization.
 *
 * @par Notes
 * Filesystems shall be initialized to support file descriptor based device
 * drivers and basic input/output functions such as printf(). Filesystems can
 * be disabled to reduce the memory footprint of an application.
 */
#define CONFIGURE_APPLICATION_DISABLE_FILESYSTEM

/* Generated from spec:/acfg/if/filesystem-all */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_FILESYSTEM_ALL
 *
 * In case this configuration option is defined, then the following
 * configuration options will be defined as well
 *
 * * @ref CONFIGURE_FILESYSTEM_DOSFS,
 *
 * * @ref CONFIGURE_FILESYSTEM_FTPFS,
 *
 * * @ref CONFIGURE_FILESYSTEM_IMFS,
 *
 * * @ref CONFIGURE_FILESYSTEM_JFFS2,
 *
 * * @ref CONFIGURE_FILESYSTEM_NFS,
 *
 * * @ref CONFIGURE_FILESYSTEM_RFS, and
 *
 * * @ref CONFIGURE_FILESYSTEM_TFTPFS.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 */
#define CONFIGURE_FILESYSTEM_ALL

/* Generated from spec:/acfg/if/filesystem-dosfs */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_FILESYSTEM_DOSFS
 *
 * In case this configuration option is defined, then the DOS (FAT) filesystem
 * is registered, so that instances of this filesystem can be mounted by the
 * application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * This filesystem requires a Block Device Cache configuration, see @ref
 * CONFIGURE_APPLICATION_NEEDS_LIBBLOCK.
 */
#define CONFIGURE_FILESYSTEM_DOSFS

/* Generated from spec:/acfg/if/filesystem-ftpfs */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_FILESYSTEM_FTPFS
 *
 * In case this configuration option is defined, then the FTP filesystem (FTP
 * client) is registered, so that instances of this filesystem can be mounted
 * by the application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 */
#define CONFIGURE_FILESYSTEM_FTPFS

/* Generated from spec:/acfg/if/filesystem-imfs */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_FILESYSTEM_IMFS
 *
 * In case this configuration option is defined, then the In-Memory Filesystem
 * (IMFS) is registered, so that instances of this filesystem can be mounted by
 * the application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * Applications will rarely need this configuration option.  This configuration
 * option is intended for test programs.  You do not need to define this
 * configuration option for the base filesystem (also known as root
 * filesystem).
 */
#define CONFIGURE_FILESYSTEM_IMFS

/* Generated from spec:/acfg/if/filesystem-jffs2 */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_FILESYSTEM_JFFS2
 *
 * In case this configuration option is defined, then the JFFS2 filesystem is
 * registered, so that instances of this filesystem can be mounted by the
 * application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 */
#define CONFIGURE_FILESYSTEM_JFFS2

/* Generated from spec:/acfg/if/filesystem-nfs */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_FILESYSTEM_NFS
 *
 * In case this configuration option is defined, then the Network Filesystem
 * (NFS) client is registered, so that instances of this filesystem can be
 * mounted by the application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 */
#define CONFIGURE_FILESYSTEM_NFS

/* Generated from spec:/acfg/if/filesystem-rfs */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_FILESYSTEM_RFS
 *
 * In case this configuration option is defined, then the RTEMS Filesystem
 * (RFS) is registered, so that instances of this filesystem can be mounted by
 * the application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * This filesystem requires a Block Device Cache configuration, see @ref
 * CONFIGURE_APPLICATION_NEEDS_LIBBLOCK.
 */
#define CONFIGURE_FILESYSTEM_RFS

/* Generated from spec:/acfg/if/filesystem-tftpfs */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_FILESYSTEM_TFTPFS
 *
 * In case this configuration option is defined, then the TFTP filesystem (TFTP
 * client) is registered, so that instances of this filesystem can be mounted
 * by the application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 */
#define CONFIGURE_FILESYSTEM_TFTPFS

/* Generated from spec:/acfg/if/imfs-disable-chmod */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_DISABLE_CHMOD
 *
 * In case this configuration option is defined, then the root IMFS does not
 * support changing the mode of files (no support for chmod()).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS supports
 * changing the mode of files.
 */
#define CONFIGURE_IMFS_DISABLE_CHMOD

/* Generated from spec:/acfg/if/imfs-disable-chown */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_DISABLE_CHOWN
 *
 * In case this configuration option is defined, then the root IMFS does not
 * support changing the ownership of files (no support for chown()).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS supports
 * changing the ownership of files.
 */
#define CONFIGURE_IMFS_DISABLE_CHOWN

/* Generated from spec:/acfg/if/imfs-disable-link */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_DISABLE_LINK
 *
 * In case this configuration option is defined, then the root IMFS does not
 * support hard links (no support for link()).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS supports hard
 * links.
 */
#define CONFIGURE_IMFS_DISABLE_LINK

/* Generated from spec:/acfg/if/imfs-disable-mknod */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_DISABLE_MKNOD
 *
 * In case this configuration option is defined, then the root IMFS does not
 * support making files (no support for mknod()).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS supports
 * making files.
 */
#define CONFIGURE_IMFS_DISABLE_MKNOD

/* Generated from spec:/acfg/if/imfs-disable-mknod-device */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_DISABLE_MKNOD_DEVICE
 *
 * In case this configuration option is defined, then the root IMFS does not
 * support making device files.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS supports
 * making device files.
 */
#define CONFIGURE_IMFS_DISABLE_MKNOD_DEVICE

/* Generated from spec:/acfg/if/imfs-disable-mknod-file */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_DISABLE_MKNOD_FILE
 *
 * In case this configuration option is defined, then the root IMFS does not
 * support making regular files.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS supports
 * making regular files.
 */
#define CONFIGURE_IMFS_DISABLE_MKNOD_FILE

/* Generated from spec:/acfg/if/imfs-disable-mount */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_DISABLE_MOUNT
 *
 * In case this configuration option is defined, then the root IMFS does not
 * support mounting other filesystems (no support for mount()).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS supports
 * mounting other filesystems.
 */
#define CONFIGURE_IMFS_DISABLE_MOUNT

/* Generated from spec:/acfg/if/imfs-disable-readdir */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_DISABLE_READDIR
 *
 * In case this configuration option is defined, then the root IMFS does not
 * support reading directories (no support for readdir()).  It is still
 * possible to open files in a directory.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS supports
 * reading directories.
 */
#define CONFIGURE_IMFS_DISABLE_READDIR

/* Generated from spec:/acfg/if/imfs-disable-readlink */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_DISABLE_READLINK
 *
 * In case this configuration option is defined, then the root IMFS does not
 * support reading symbolic links (no support for readlink()).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS supports
 * reading symbolic links.
 */
#define CONFIGURE_IMFS_DISABLE_READLINK

/* Generated from spec:/acfg/if/imfs-disable-rename */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_DISABLE_RENAME
 *
 * In case this configuration option is defined, then the root IMFS does not
 * support renaming files (no support for rename()).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS supports
 * renaming files.
 */
#define CONFIGURE_IMFS_DISABLE_RENAME

/* Generated from spec:/acfg/if/imfs-disable-rmnod */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_DISABLE_RMNOD
 *
 * In case this configuration option is defined, then the root IMFS does not
 * support removing files (no support for rmnod()).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS supports
 * removing files.
 */
#define CONFIGURE_IMFS_DISABLE_RMNOD

/* Generated from spec:/acfg/if/imfs-disable-symlink */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_DISABLE_SYMLINK
 *
 * In case this configuration option is defined, then the root IMFS does not
 * support creating symbolic links (no support for symlink()).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS supports
 * creating symbolic links.
 */
#define CONFIGURE_IMFS_DISABLE_SYMLINK

/* Generated from spec:/acfg/if/imfs-disable-unmount */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_DISABLE_UNMOUNT
 *
 * In case this configuration option is defined, then the root IMFS does not
 * support unmounting other filesystems (no support for unmount()).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS supports
 * unmounting other filesystems.
 */
#define CONFIGURE_IMFS_DISABLE_UNMOUNT

/* Generated from spec:/acfg/if/imfs-disable-utime */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_DISABLE_UTIME
 *
 * In case this configuration option is defined, then the root IMFS does not
 * support changing file times (no support for utime()).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS supports
 * changing file times.
 */
#define CONFIGURE_IMFS_DISABLE_UTIME

/* Generated from spec:/acfg/if/imfs-enable-mkfifo */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IMFS_ENABLE_MKFIFO
 *
 * In case this configuration option is defined, then the root IMFS supports
 * making FIFOs.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the root IMFS does not
 * support making FIFOs (no support for mkfifo()).
 */
#define CONFIGURE_IMFS_ENABLE_MKFIFO

/* Generated from spec:/acfg/if/imfs-memfile-bytes-per-block */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK
 *
 * The value of this configuration option defines the block size for in-memory
 * files managed by the IMFS.
 *
 * @par Default Value
 * The default value is 128.
 *
 * @par Constraints
 * The value of the configuration option shall be equal to 16, 32, 64, 128,
 * 256, or 512.
 *
 * @par Notes
 * @parblock
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
 * @endparblock
 */
#define CONFIGURE_IMFS_MEMFILE_BYTES_PER_BLOCK

/* Generated from spec:/acfg/if/jffs2-delayed-write-task-priority */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_JFFS2_DELAYED_WRITE_TASK_PRIORITY
 *
 * The value of this configuration option defines the JFFS2 delayed write task
 * priority.
 *
 * @par Default Value
 * The default value is 15.
 *
 * @par Constraints
 * The value of the configuration option shall be a valid Classic API task
 * priority.  The set of valid task priorities depends on the scheduler
 * configuration.
 */
#define CONFIGURE_JFFS2_DELAYED_WRITE_TASK_PRIORITY

/* Generated from spec:/acfg/if/use-devfs-as-base-filesystem */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM
 *
 * In case this configuration option is defined, then an IMFS with a reduced
 * feature set will be the base filesystem (also known as root filesystem).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * In case this configuration option is defined, then the following
 * configuration options will be defined as well
 *
 * * @ref CONFIGURE_IMFS_DISABLE_CHMOD,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_CHOWN,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_LINK,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_MKNOD_FILE,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_MOUNT,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_READDIR,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_READLINK,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_RENAME,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_RMNOD,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_SYMLINK,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_UTIME, and
 *
 * * @ref CONFIGURE_IMFS_DISABLE_UNMOUNT.
 *
 * In addition, a simplified path evaluation is enabled.  It allows only a look
 * up of absolute paths.
 *
 * This configuration of the IMFS is basically a device-only filesystem.  It is
 * comparable in functionality to the pseudo-filesystem name space provided
 * before RTEMS release 4.5.0.
 * @endparblock
 */
#define CONFIGURE_USE_DEVFS_AS_BASE_FILESYSTEM

/* Generated from spec:/acfg/if/use-miniimfs-as-base-filesystem */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM
 *
 * In case this configuration option is defined, then an IMFS with a reduced
 * feature set will be the base filesystem (also known as root filesystem).
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * In case this configuration option is defined, then the following
 * configuration options will be defined as well
 *
 * * @ref CONFIGURE_IMFS_DISABLE_CHMOD,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_CHOWN,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_LINK,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_READLINK,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_RENAME,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_SYMLINK,
 *
 * * @ref CONFIGURE_IMFS_DISABLE_UTIME, and
 *
 * * @ref CONFIGURE_IMFS_DISABLE_UNMOUNT.
 * @endparblock
 */
#define CONFIGURE_USE_MINIIMFS_AS_BASE_FILESYSTEM

/** @} */

/* Generated from spec:/acfg/if/group-general */

/**
 * @defgroup RTEMSApplConfigGeneralSystemConfiguration \
 *   General System Configuration
 *
 * @ingroup RTEMSApplConfig
 *
 * This section describes general system configuration options.
 *
 * @{
 */

/* Generated from spec:/acfg/if/dirty-memory */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_DIRTY_MEMORY
 *
 * In case this configuration option is defined, then the memory areas used for
 * the RTEMS Workspace and the C Program Heap are dirtied with a `0xCF` byte
 * pattern during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * Dirtying memory can add significantly to system initialization time.  It may
 * assist in finding code that incorrectly assumes the contents of free memory
 * areas is cleared to zero during system initialization.  In case @ref
 * CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY is also defined, then the memory is
 * first dirtied and then zeroed.
 *
 * See also @ref CONFIGURE_MALLOC_DIRTY.
 * @endparblock
 */
#define CONFIGURE_DIRTY_MEMORY

/* Generated from spec:/acfg/if/disable-bsp-settings */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_DISABLE_BSP_SETTINGS
 *
 * In case this configuration option is defined, then the optional BSP provided
 * settings listed below are disabled.
 *
 * The optional BSP provided default values for the following application
 * configuration options are disabled:
 *
 * * @ref CONFIGURE_IDLE_TASK_BODY
 *
 * * @ref CONFIGURE_IDLE_TASK_STACK_SIZE
 *
 * * @ref CONFIGURE_INTERRUPT_STACK_SIZE
 *
 * The optional BSP provided initial extension set is disabled (see initial
 * extension sets).  The optional BSP provided prerequisite IO device drivers
 * are disabled (see Device Driver Configuration).  The optional BSP provided
 * support for sbrk() is disabled.
 *
 * This configuration option provides an all or nothing choice with respect to
 * the optional BSP provided settings.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 */
#define CONFIGURE_DISABLE_BSP_SETTINGS

/* Generated from spec:/acfg/if/disable-newlib-reentrancy */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_DISABLE_NEWLIB_REENTRANCY
 *
 * In case this configuration option is defined, then the Newlib reentrancy
 * support per thread is disabled and a global reentrancy structure is used.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * You can enable this option to reduce the size of the `TCB`.  Use this option
 * with care, since it can lead to race conditions and undefined system
 * behaviour.  For example, #errno is no longer a thread-local variable if this
 * option is enabled.
 */
#define CONFIGURE_DISABLE_NEWLIB_REENTRANCY

/* Generated from spec:/acfg/if/executive-ram-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_EXECUTIVE_RAM_SIZE
 *
 * The value of this configuration option defines the RTEMS Workspace size in
 * bytes.
 *
 * @par Default Value
 * If this configuration option is undefined, then the RTEMS Workspace and task
 * stack space size is calculated by ``<rtems/confdefs.h>`` based on the values
 * configuration options.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/integer">UINTPTR_MAX</a>.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 * @endparblock
 *
 * @par Notes
 * This is an advanced configuration option.  Use it only if you know exactly
 * what you are doing.
 */
#define CONFIGURE_EXECUTIVE_RAM_SIZE

/* Generated from spec:/acfg/if/extra-task-stacks */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_EXTRA_TASK_STACKS
 *
 * The value of this configuration option defines the number of bytes the
 * applications wishes to add to the task stack requirements calculated by
 * ``<rtems/confdefs.h>``.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be small enough so that the
 *   task stack space calculation carried out by ``<rtems/confdefs.h>`` does
 *   not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/integer">uintptr_t</a>.
 * @endparblock
 *
 * @par Notes
 * This parameter is very important.  If the application creates tasks with
 * stacks larger then the minimum, then that memory is **not** accounted for by
 * ``<rtems/confdefs.h>``.
 */
#define CONFIGURE_EXTRA_TASK_STACKS

/* Generated from spec:/acfg/if/init */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_INIT
 *
 * While this configuration option is defined, when the ``<rtems/confdefs.h>``
 * is included, the system settings defined by present application
 * configuration options are statically allocated and initialized.  All user
 * provided application configuration options defined before the include of
 * ``<rtems/confdefs.h>`` are evaluated.  They define the actual system
 * settings.
 *
 * @par Default Configuration
 * There is no default configuration associated with this configuration option.
 * If ``<rtems/confdefs.h>`` is included and this configuration option is not
 * defined, then only white space is included.
 */
#define CONFIGURE_INIT

/* Generated from spec:/acfg/if/initial-extensions */

/**
 * @brief This configuration option is an initializer define.
 *
 * @anchor CONFIGURE_INITIAL_EXTENSIONS
 *
 * The value of this configuration option is used to initialize the table of
 * initial user extensions.
 *
 * @par Default Value
 * The default value is the empty list.
 *
 * @par Constraints
 * The value of the configuration option shall be a list of initializers for
 * structures of type ::rtems_extensions_table.
 *
 * @par Notes
 * @parblock
 * The value of this configuration option is placed before the entries of
 * #BSP_INITIAL_EXTENSION and after the entries of all other initial user
 * extensions.
 *
 * It is recommended that applications provide a fatal extension to customize
 * the <a
 * href="https://docs.rtems.org/branches/master/c-user/fatal-error/background.html#terminate">System
 * Termination Procedure</a>.
 * @endparblock
 */
#define CONFIGURE_INITIAL_EXTENSIONS

/* Generated from spec:/acfg/if/interrupt-stack-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_INTERRUPT_STACK_SIZE
 *
 * The value of this configuration option defines the size of an interrupt
 * stack in bytes.
 *
 * @par Default Value
 * If the @ref CONFIGURE_DISABLE_BSP_SETTINGS configuration option is not
 * defined and #BSP_INTERRUPT_STACK_SIZE is provided by the BSP, then the
 * default value is defined by #BSP_INTERRUPT_STACK_SIZE, otherwise the default
 * value is #CPU_STACK_MINIMUM_SIZE.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to a
 *   BSP-specific and application-specific minimum value.
 *
 * - The value of the configuration option shall be small enough so that the
 *   interrupt stack area calculation carried out by ``<rtems/confdefs.h>``
 *   does not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/size_t">size_t</a>.
 *
 * - The value of the configuration option shall be aligned according to
 *   #CPU_INTERRUPT_STACK_ALIGNMENT.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * There is one interrupt stack available for each configured processor (@ref
 * CONFIGURE_MAXIMUM_PROCESSORS).  The interrupt stack areas are statically
 * allocated in a special linker section (`.rtemsstack.interrupt`). The
 * placement of this linker section is BSP-specific.
 *
 * Some BSPs use the interrupt stack as the initialization stack which is used
 * to perform the sequential system initialization before the multithreading is
 * started.
 *
 * The interrupt stacks are covered by the stack checker, see @ref
 * CONFIGURE_STACK_CHECKER_ENABLED.  However, using a too small interrupt stack
 * size may still result in undefined behaviour.
 *
 * The interrupt stack size may depend on the interrupt priority settings, see
 * also rtems_interrupt_set_priority().
 *
 * In releases before RTEMS 5.1 the default value was @ref
 * CONFIGURE_MINIMUM_TASK_STACK_SIZE instead of #CPU_STACK_MINIMUM_SIZE.
 * @endparblock
 */
#define CONFIGURE_INTERRUPT_STACK_SIZE

/* Generated from spec:/acfg/if/malloc-dirty */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_MALLOC_DIRTY
 *
 * In case this configuration option is defined, then each memory area returned
 * by C Program Heap allocator functions such as malloc() is dirtied with a
 * `0xCF` byte pattern before it is handed over to the application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * The dirtying performed by this option is carried out for each successful
 * memory allocation from the C Program Heap in contrast to @ref
 * CONFIGURE_DIRTY_MEMORY which dirties the memory only once during the system
 * initialization.
 */
#define CONFIGURE_MALLOC_DIRTY

/* Generated from spec:/acfg/if/max-file-descriptors */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_FILE_DESCRIPTORS
 *
 * The value of this configuration option defines the maximum number of file
 * like objects that can be concurrently open.
 *
 * @par Default Value
 * The default value is 3.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/limits">SIZE_MAX</a>.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 * @endparblock
 *
 * @par Notes
 * The default value of three file descriptors allows RTEMS to support standard
 * input, output, and error I/O streams on `/dev/console`.
 */
#define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS

/* Generated from spec:/acfg/if/max-processors */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_PROCESSORS
 *
 * The value of this configuration option defines the maximum number of
 * processors an application intends to use.  The number of actually available
 * processors depends on the hardware and may be less.  It is recommended to
 * use the smallest value suitable for the application in order to save memory.
 * Each processor needs an IDLE task stack and interrupt stack for example.
 *
 * @par Default Value
 * The default value is 1.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   one.
 *
 * - The value of the configuration option shall be less than or equal to
 *   #CPU_MAXIMUM_PROCESSORS.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * If there are more processors available than configured, the rest will be
 * ignored.
 *
 * This configuration option is only evaluated in SMP configurations of RTEMS
 * (e.g. RTEMS was built with the SMP build configuration option enabled). In
 * all other configurations it has no effect.
 * @endparblock
 */
#define CONFIGURE_MAXIMUM_PROCESSORS

/* Generated from spec:/acfg/if/max-thread-local-storage-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE
 *
 * If the value of this configuration option is greater than zero, then it
 * defines the maximum thread-local storage size, otherwise the thread-local
 * storage size is defined by the linker depending on the thread-local storage
 * objects used by the application in the statically-linked executable.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/limits">SIZE_MAX</a>.
 *
 * - The value of the configuration option shall be an integral multiple of
 *   #RTEMS_TASK_STORAGE_ALIGNMENT.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * This configuration option can be used to reserve space for the dynamic
 * linking of modules with thread-local storage objects.
 *
 * If the thread-local storage size defined by the thread-local storage objects
 * used by the application in the statically-linked executable is greater than
 * a non-zero value of this configuration option, then a fatal error will occur
 * during system initialization.
 *
 * Use RTEMS_ALIGN_UP() and #RTEMS_TASK_STORAGE_ALIGNMENT to adjust the size to
 * meet the minimum alignment requirement of a thread-local storage area.
 *
 * The actual thread-local storage size is determined when the application
 * executable is linked.  The `rtems-exeinfo` command line tool included in the
 * RTEMS Tools can be used to obtain the thread-local storage size and
 * alignment of an application executable.
 * @endparblock
 */
#define CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE

/* Generated from spec:/acfg/if/max-thread-name-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_THREAD_NAME_SIZE
 *
 * The value of this configuration option defines the maximum thread name size
 * including the terminating `NUL` character.
 *
 * @par Default Value
 * The default value is 16.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/limits">SIZE_MAX</a>.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * The default value was chosen for Linux compatibility, see <a
 * href="http://man7.org/linux/man-pages/man3/pthread_setname_np.3.html">pthread_setname_np()</a>.
 *
 * The size of the thread control block is increased by the maximum thread name
 * size.
 *
 * This configuration option is available since RTEMS 5.1.
 * @endparblock
 */
#define CONFIGURE_MAXIMUM_THREAD_NAME_SIZE

/* Generated from spec:/acfg/if/memory-overhead */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MEMORY_OVERHEAD
 *
 * The value of this configuration option defines the number of kilobytes the
 * application wishes to add to the RTEMS Workspace size calculated by
 * ``<rtems/confdefs.h>``.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option shall be small enough so that the
 *   RTEMS Workspace size calculation carried out by ``<rtems/confdefs.h>``
 *   does not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/integer">uintptr_t</a>.
 * @endparblock
 *
 * @par Notes
 * This configuration option should only be used when it is suspected that a
 * bug in ``<rtems/confdefs.h>`` has resulted in an underestimation.  Typically
 * the memory allocation will be too low when an application does not account
 * for all message queue buffers or task stacks, see @ref
 * CONFIGURE_MESSAGE_BUFFER_MEMORY.
 */
#define CONFIGURE_MEMORY_OVERHEAD

/* Generated from spec:/acfg/if/message-buffer-memory */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MESSAGE_BUFFER_MEMORY
 *
 * The value of this configuration option defines the number of bytes reserved
 * for message queue buffers in the RTEMS Workspace.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option shall be small enough so that the
 *   RTEMS Workspace size calculation carried out by ``<rtems/confdefs.h>``
 *   does not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/integer">uintptr_t</a>.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * The configuration options @ref CONFIGURE_MAXIMUM_MESSAGE_QUEUES and @ref
 * CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES define only how many message queues
 * can be created by the application.  The memory for the message buffers is
 * configured by this option.  For each message queue you have to reserve some
 * memory for the message buffers.  The size depends on the maximum number of
 * pending messages and the maximum size of the messages of a message queue.
 * Use the `CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE()` macro to specify the message
 * buffer memory for each message queue and sum them up to define the value for
 * `CONFIGURE_MAXIMUM_MESSAGE_QUEUES`.
 *
 * The interface for the `CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE()` help macro is
 * as follows:
 *
 * @code
 * CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( max_messages, max_msg_size )
 * @endcode
 *
 * Where `max_messages` is the maximum number of pending messages and
 * `max_msg_size` is the maximum size in bytes of the messages of the
 * corresponding message queue.  Both parameters shall be compile time
 * constants.  Not using this help macro (e.g. just using `max_messages *
 * max_msg_size`) may result in an underestimate of the RTEMS Workspace size.
 *
 * The following example illustrates how the
 * `CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE()` help macro can be used to assist in
 * calculating the message buffer memory required.  In this example, there are
 * two message queues used in this application.  The first message queue has a
 * maximum of 24 pending messages with the message structure defined by the
 * type `one_message_type`.  The other message queue has a maximum of 500
 * pending messages with the message structure defined by the type
 * `other_message_type`.
 *
 * @code
 * #define CONFIGURE_MESSAGE_BUFFER_MEMORY ( \
 *     CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( \
 *       24, \
 *       sizeof( one_message_type ) \
 *     ) \
 *     + CONFIGURE_MESSAGE_BUFFERS_FOR_QUEUE( \
 *       500, \
 *       sizeof( other_message_type ) \
 *     ) \
 *   )
 * @endcode
 * @endparblock
 */
#define CONFIGURE_MESSAGE_BUFFER_MEMORY

/* Generated from spec:/acfg/if/microseconds-per-tick */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MICROSECONDS_PER_TICK
 *
 * The value of this configuration option defines the length of time in
 * microseconds between clock ticks (clock tick quantum).
 *
 * When the clock tick quantum value is too low, the system will spend so much
 * time processing clock ticks that it does not have processing time available
 * to perform application work. In this case, the system will become
 * unresponsive.
 *
 * The lowest practical time quantum varies widely based upon the speed of the
 * target hardware and the architectural overhead associated with interrupts.
 * In general terms, you do not want to configure it lower than is needed for
 * the application.
 *
 * The clock tick quantum should be selected such that it all blocking and
 * delay times in the application are evenly divisible by it. Otherwise,
 * rounding errors will be introduced which may negatively impact the
 * application.
 *
 * @par Default Value
 * The default value is 10000.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to a
 *   value defined by the Clock Driver.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   value defined by the Clock Driver.
 *
 * - The resulting clock ticks per second should be an integer.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * This configuration option has no impact if the Clock Driver is not
 * configured, see @ref CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER.
 *
 * There may be Clock Driver specific limits on the resolution or maximum value
 * of a clock tick quantum.
 * @endparblock
 */
#define CONFIGURE_MICROSECONDS_PER_TICK

/* Generated from spec:/acfg/if/min-task-stack-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MINIMUM_TASK_STACK_SIZE
 *
 * The value of this configuration option defines the minimum stack size in
 * bytes for every user task or thread in the system.
 *
 * @par Default Value
 * The default value is #CPU_STACK_MINIMUM_SIZE.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be small enough so that the
 *   task stack space calculation carried out by ``<rtems/confdefs.h>`` does
 *   not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/integer">uintptr_t</a>.
 *
 * - The value of the configuration option shall be greater than or equal to a
 *   BSP-specific and application-specific minimum value.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * Adjusting this parameter should be done with caution.  Examining the actual
 * stack usage using the stack checker usage reporting facility is recommended
 * (see also @ref CONFIGURE_STACK_CHECKER_ENABLED).
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
 * of POSIX threads.  This can be changed with the @ref
 * CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE configuration option.
 *
 * In releases before RTEMS 5.1 the `CONFIGURE_MINIMUM_TASK_STACK_SIZE` was
 * used to define the default value of @ref CONFIGURE_INTERRUPT_STACK_SIZE.
 * @endparblock
 */
#define CONFIGURE_MINIMUM_TASK_STACK_SIZE

/* Generated from spec:/acfg/if/stack-checker-enabled */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_STACK_CHECKER_ENABLED
 *
 * In case this configuration option is defined, then the stack checker is
 * enabled.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * The stack checker performs run-time stack bounds checking.  This increases
 * the time required to create tasks as well as adding overhead to each context
 * switch.
 *
 * In 4.9 and older, this configuration option was named `STACK_CHECKER_ON`.
 * @endparblock
 */
#define CONFIGURE_STACK_CHECKER_ENABLED

/* Generated from spec:/acfg/if/ticks-per-time-slice */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_TICKS_PER_TIMESLICE
 *
 * The value of this configuration option defines the length of the timeslice
 * quantum in ticks for each task.
 *
 * @par Default Value
 * The default value is 50.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   one.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/integer">UINT32_MAX</a>.
 * @endparblock
 *
 * @par Notes
 * This configuration option has no impact if the Clock Driver is not
 * configured, see @ref CONFIGURE_APPLICATION_DOES_NOT_NEED_CLOCK_DRIVER.
 */
#define CONFIGURE_TICKS_PER_TIMESLICE

/* Generated from spec:/acfg/if/unified-work-areas */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_UNIFIED_WORK_AREAS
 *
 * In case this configuration option is defined, then the RTEMS Workspace and
 * the C Program Heap will be one pool of memory.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then there will be separate
 * memory pools for the RTEMS Workspace and C Program Heap.
 *
 * @par Notes
 * @parblock
 * Having separate pools does have some advantages in the event a task blows a
 * stack or writes outside its memory area. However, in low memory systems the
 * overhead of the two pools plus the potential for unused memory in either
 * pool is very undesirable.
 *
 * In high memory environments, this is desirable when you want to use the <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a> option.  You will be able to create objects until you run out of
 * all available memory rather then just until you run out of RTEMS Workspace.
 * @endparblock
 */
#define CONFIGURE_UNIFIED_WORK_AREAS

/* Generated from spec:/acfg/if/unlimited-allocation-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_UNLIMITED_ALLOCATION_SIZE
 *
 * If @ref CONFIGURE_UNLIMITED_OBJECTS is defined, then the value of this
 * configuration option defines the default objects maximum of all object
 * classes supporting <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a> to `rtems_resource_unlimited(
 * CONFIGURE_UNLIMITED_ALLOCATION_SIZE )`.
 *
 * @par Default Value
 * The default value is 8.
 *
 * @par Constraints
 * The value of the configuration option shall meet the constraints of all
 * object classes to which it is applied.
 *
 * @par Notes
 * @parblock
 * By allowing users to declare all resources as being unlimited the user can
 * avoid identifying and limiting the resources used.
 *
 * The object maximum of each class can be configured also individually using
 * the rtems_resource_unlimited() macro.
 * @endparblock
 */
#define CONFIGURE_UNLIMITED_ALLOCATION_SIZE

/* Generated from spec:/acfg/if/unlimited-objects */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_UNLIMITED_OBJECTS
 *
 * In case this configuration option is defined, then unlimited objects are
 * used by default.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * When using unlimited objects, it is common practice to also specify @ref
 * CONFIGURE_UNIFIED_WORK_AREAS so the system operates with a single pool of
 * memory for both RTEMS Workspace and C Program Heap.
 *
 * This option does not override an explicit configuration for a particular
 * object class by the user.
 *
 * See also @ref CONFIGURE_UNLIMITED_ALLOCATION_SIZE.
 * @endparblock
 */
#define CONFIGURE_UNLIMITED_OBJECTS

/* Generated from spec:/acfg/if/verbose-system-init */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_VERBOSE_SYSTEM_INITIALIZATION
 *
 * In case this configuration option is defined, then the system initialization
 * is verbose.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * You may use this feature to debug system initialization issues.  The
 * printk() function is used to print the information.
 */
#define CONFIGURE_VERBOSE_SYSTEM_INITIALIZATION

/* Generated from spec:/acfg/if/zero-workspace-automatically */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY
 *
 * In case this configuration option is defined, then the memory areas used for
 * the RTEMS Workspace and the C Program Heap are zeroed with a `0x00` byte
 * pattern during system initialization.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * Zeroing memory can add significantly to the system initialization time. It
 * is not necessary for RTEMS but is often assumed by support libraries.  In
 * case @ref CONFIGURE_DIRTY_MEMORY is also defined, then the memory is first
 * dirtied and then zeroed.
 */
#define CONFIGURE_ZERO_WORKSPACE_AUTOMATICALLY

/** @} */

/* Generated from spec:/acfg/if/group-idle */

/**
 * @defgroup RTEMSApplConfigIdleTaskConfiguration Idle Task Configuration
 *
 * @ingroup RTEMSApplConfig
 *
 * This section describes configuration options related to the idle tasks.
 *
 * @{
 */

/* Generated from spec:/acfg/if/idle-task-body */

/**
 * @brief This configuration option is an initializer define.
 *
 * @anchor CONFIGURE_IDLE_TASK_BODY
 *
 * The value of this configuration option initializes the IDLE thread body.
 *
 * @par Default Value
 * If the @ref CONFIGURE_DISABLE_BSP_SETTINGS configuration option is not
 * defined and #BSP_IDLE_TASK_BODY is provided by the BSP, then the default
 * value is defined by #BSP_IDLE_TASK_BODY, otherwise the default value is
 * `_CPU_Thread_Idle_body`.
 *
 * @par Constraints
 * The value of the configuration option shall be defined to a valid function
 * pointer of the type `void *( *idle_body )( uintptr_t )`.
 *
 * @par Notes
 * @parblock
 * IDLE threads shall not block.  A blocking IDLE thread results in undefined
 * system behaviour because the scheduler assume that at least one ready thread
 * exists.
 *
 * IDLE threads can be used to initialize the application, see configuration
 * option @ref CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION.
 *
 * The BSP may have knowledge of the specific CPU model, system controller
 * logic, and peripheral buses, so a BSP-specific IDLE task may be capable of
 * turning components off to save power during extended periods of no task
 * activity.
 * @endparblock
 */
#define CONFIGURE_IDLE_TASK_BODY

/* Generated from spec:/acfg/if/idle-task-init-appl */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION
 *
 * This configuration option is defined to indicate that the user has
 * configured **no** user initialization tasks or threads and that the user
 * provided IDLE task will perform application initialization and then
 * transform itself into an IDLE task.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the user is assumed to
 * provide one or more initialization tasks.
 *
 * @par Notes
 * @parblock
 * If you use this option be careful, the user IDLE task **cannot** block at
 * all during the initialization sequence.  Further, once application
 * initialization is complete, it shall make itself preemptible and enter an
 * idle body loop.
 *
 * The IDLE task shall run at the lowest priority of all tasks in the system.
 *
 * If this configuration option is defined, then it is mandatory to configure a
 * user IDLE task with the @ref CONFIGURE_IDLE_TASK_BODY configuration option,
 * otherwise a compile time error in the configuration file will occur.
 *
 * The application shall define at least one of the following configuration
 * options
 *
 * * @ref CONFIGURE_RTEMS_INIT_TASKS_TABLE,
 *
 * * @ref CONFIGURE_POSIX_INIT_THREAD_TABLE, or
 *
 * * `CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION`
 *
 * otherwise a compile time error in the configuration file will occur.
 *
 * If no Classic API initialization task and no POSIX API initialization thread
 * is configured, then no <a
 * href="https://docs.rtems.org/branches/master/c-user/initialization/operations.html">Global
 * Construction</a> is performed.
 * @endparblock
 */
#define CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION

/* Generated from spec:/acfg/if/idle-task-stack-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_IDLE_TASK_STACK_SIZE
 *
 * The value of this configuration option defines the task stack size for an
 * IDLE task.
 *
 * @par Default Value
 * If the @ref CONFIGURE_DISABLE_BSP_SETTINGS configuration option is not
 * defined and #BSP_IDLE_TASK_STACK_SIZE is provided by the BSP, then the
 * default value is defined by #BSP_IDLE_TASK_STACK_SIZE, otherwise the default
 * value is defined by the @ref CONFIGURE_MINIMUM_TASK_STACK_SIZE configuration
 * option.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to a
 *   BSP-specific and application-specific minimum value.
 *
 * - The value of the configuration option shall be small enough so that the
 *   IDLE task stack area calculation carried out by ``<rtems/confdefs.h>``
 *   does not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/size_t">size_t</a>.
 * @endparblock
 *
 * @par Notes
 * In SMP configurations, there is one IDLE task per configured processor, see
 * @ref CONFIGURE_MAXIMUM_PROCESSORS.
 */
#define CONFIGURE_IDLE_TASK_STACK_SIZE

/* Generated from spec:/acfg/if/idle-task-storage-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_IDLE_TASK_STORAGE_SIZE
 *
 * If this configuration option is specified, then the task storage areas for
 * the IDLE tasks are statically allocated by ``<rtems/confdefs.h>``.  The
 * value of this configuration option defines the size in bytes of the task
 * storage area of each IDLE task in the system.
 *
 * @par Default Value
 * This configuration option has no default value.  If it is not specified,
 * then the task storage area for each IDLE task will allocated from the RTEMS
 * Workspace or through a custom IDLE task stack allocator.
 *
 * @par Constraints
 * The value of the configuration option shall be greater than or equal to @ref
 * CONFIGURE_IDLE_TASK_STACK_SIZE.
 *
 * @par Notes
 * @parblock
 * By default, the IDLE task storage areas are allocated from the RTEMS
 * Workspace.  Applications which do not want to use a heap allocator can use
 * this configuration option to use statically allocated memory for the IDLE
 * task storage areas.  The task storage area contains the task stack, the
 * thread-local storage, and the floating-point context on architectures with a
 * separate floating-point context.  The size of the thread-local storage area
 * is defined at link time or by the @ref
 * CONFIGURE_MAXIMUM_THREAD_LOCAL_STORAGE_SIZE configuration option.  You have
 * to estimate the actual thread-local storage size if you want to use this
 * configuration option.  If the IDLE task stack size would be less than the
 * value defined by the @ref CONFIGURE_IDLE_TASK_STACK_SIZE configuration
 * option, for example because the thread-local storage size is larger than
 * expected, then the system terminates with the INTERNAL_ERROR_CORE fatal
 * source and the INTERNAL_ERROR_IDLE_THREAD_STACK_TOO_SMALL fatal code during
 * system initialization.
 *
 * The value of this configuration option is passed to
 * RTEMS_TASK_STORAGE_SIZE() by ``<rtems/confdefs.h>`` to determine the actual
 * size of the statically allocated area to take architecture-specific
 * overheads into account.
 *
 * The
 *
 * * `CONFIGURE_IDLE_TASK_STORAGE_SIZE`, and
 *
 * * @ref CONFIGURE_TASK_STACK_ALLOCATOR_FOR_IDLE
 *
 * configuration options are mutually exclusive.
 * @endparblock
 */
#define CONFIGURE_IDLE_TASK_STORAGE_SIZE

/** @} */

/* Generated from spec:/acfg/if/group-mpci */

/**
 * @defgroup RTEMSApplConfigMultiprocessingConfiguration \
 *   Multiprocessing Configuration
 *
 * @ingroup RTEMSApplConfig
 *
 * This section describes multiprocessing related configuration options. The
 * options are only used if RTEMS was built when the multiprocessing build
 * configuration option is enabled. The multiprocessing configuration is
 * distinct from the SMP configuration.  Additionally, this class of
 * configuration options are only applicable if the configuration option @ref
 * CONFIGURE_MP_APPLICATION is defined.  The multiprocessing (MPCI) support
 * must not be confused with the SMP support.
 *
 * @{
 */

/* Generated from spec:/acfg/if/mp-extra-server-stack */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK
 *
 * The value of this configuration option defines the number of bytes the
 * applications wishes to add to the MPCI task stack on top of @ref
 * CONFIGURE_MINIMUM_TASK_STACK_SIZE.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/integer">UINT32_MAX</a>.
 *
 * - The value of the configuration option shall be small enough so that the
 *   MPCI receive server stack area calculation carried out by
 *   ``<rtems/confdefs.h>`` does not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/size_t">size_t</a>.
 * @endparblock
 *
 * @par Notes
 * This configuration option is only evaluated if @ref CONFIGURE_MP_APPLICATION
 * is defined.
 */
#define CONFIGURE_EXTRA_MPCI_RECEIVE_SERVER_STACK

/* Generated from spec:/acfg/if/mp-appl */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_MP_APPLICATION
 *
 * This configuration option is defined to indicate that the application
 * intends to be part of a multiprocessing configuration.  Additional
 * configuration options are assumed to be provided.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the multiprocessing services
 * are not initialized.
 *
 * @par Notes
 * This configuration option shall be undefined if the multiprocessing support
 * is not enabled (e.g. RTEMS was built without the multiprocessing build
 * configuration option enabled).  Otherwise a compile time error in the
 * configuration file will occur.
 */
#define CONFIGURE_MP_APPLICATION

/* Generated from spec:/acfg/if/mp-max-global-objects */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS
 *
 * The value of this configuration option defines the maximum number of
 * concurrently active global objects in a multiprocessor system.
 *
 * @par Default Value
 * The default value is 32.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/integer">UINT32_MAX</a>.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * This value corresponds to the total number of objects which can be created
 * with the #RTEMS_GLOBAL attribute.
 *
 * This configuration option is only evaluated if @ref CONFIGURE_MP_APPLICATION
 * is defined.
 * @endparblock
 */
#define CONFIGURE_MP_MAXIMUM_GLOBAL_OBJECTS

/* Generated from spec:/acfg/if/mp-max-nodes */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MP_MAXIMUM_NODES
 *
 * The value of this configuration option defines the maximum number of nodes
 * in a multiprocessor system.
 *
 * @par Default Value
 * The default value is 2.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/integer">UINT32_MAX</a>.
 * @endparblock
 *
 * @par Notes
 * This configuration option is only evaluated if @ref CONFIGURE_MP_APPLICATION
 * is defined.
 */
#define CONFIGURE_MP_MAXIMUM_NODES

/* Generated from spec:/acfg/if/mp-max-proxies */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MP_MAXIMUM_PROXIES
 *
 * The value of this configuration option defines the maximum number of
 * concurrently active thread/task proxies on this node in a multiprocessor
 * system.
 *
 * @par Default Value
 * The default value is 32.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/integer">UINT32_MAX</a>.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * Since a proxy is used to represent a remote task/thread which is blocking on
 * this node. This configuration parameter reflects the maximum number of
 * remote tasks/threads which can be blocked on objects on this node, see <a
 * href="https://docs.rtems.org/branches/master/c-user/multiprocessing/background.html#proxies">Proxies</a>.
 *
 * This configuration option is only evaluated if @ref CONFIGURE_MP_APPLICATION
 * is defined.
 * @endparblock
 */
#define CONFIGURE_MP_MAXIMUM_PROXIES

/* Generated from spec:/acfg/if/mp-mpci-table-pointer */

/**
 * @brief This configuration option is an initializer define.
 *
 * @anchor CONFIGURE_MP_MPCI_TABLE_POINTER
 *
 * The value of this configuration option initializes the MPCI Configuration
 * Table.
 *
 * @par Default Value
 * The default value is `&MPCI_table`.
 *
 * @par Constraints
 * The value of the configuration option shall be a pointer to
 * ::rtems_mpci_table.
 *
 * @par Notes
 * @parblock
 * RTEMS provides a Shared Memory MPCI Device Driver which can be used on any
 * Multiprocessor System assuming the BSP provides the proper set of supporting
 * methods.
 *
 * This configuration option is only evaluated if @ref CONFIGURE_MP_APPLICATION
 * is defined.
 * @endparblock
 */
#define CONFIGURE_MP_MPCI_TABLE_POINTER

/* Generated from spec:/acfg/if/mp-node-number */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MP_NODE_NUMBER
 *
 * The value of this configuration option defines the node number of this node
 * in a multiprocessor system.
 *
 * @par Default Value
 * The default value is `NODE_NUMBER`.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/integer">UINT32_MAX</a>.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * In the RTEMS Multiprocessing Test Suite, the node number is derived from the
 * Makefile variable `NODE_NUMBER`. The same code is compiled with the
 * `NODE_NUMBER` set to different values. The test programs behave differently
 * based upon their node number.
 *
 * This configuration option is only evaluated if @ref CONFIGURE_MP_APPLICATION
 * is defined.
 * @endparblock
 */
#define CONFIGURE_MP_NODE_NUMBER

/** @} */

/* Generated from spec:/acfg/if/group-posix */

/**
 * @defgroup RTEMSApplConfigPOSIXAPIConfiguration POSIX API Configuration
 *
 * @ingroup RTEMSApplConfig
 *
 * This section describes configuration options related to the POSIX API.  Most
 * POSIX API objects are available by default since RTEMS 5.1.  The queued
 * signals and timers are only available if RTEMS was built with the enable
 * POSIX build configuration option.
 *
 * @{
 */

/* Generated from spec:/acfg/if/max-posix-keys */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_POSIX_KEYS
 *
 * The value of this configuration option defines the maximum number of POSIX
 * API Keys that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 * @endparblock
 *
 * @par Notes
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.
 */
#define CONFIGURE_MAXIMUM_POSIX_KEYS

/* Generated from spec:/acfg/if/max-posix-key-value-pairs */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS
 *
 * The value of this configuration option defines the maximum number of key
 * value pairs used by POSIX API Keys that can be concurrently active.
 *
 * @par Default Value
 * The default value is @ref CONFIGURE_MAXIMUM_POSIX_KEYS * ( @ref
 * CONFIGURE_MAXIMUM_TASKS + @ref CONFIGURE_MAXIMUM_POSIX_THREADS ).
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.
 *
 * A key value pair is created by pthread_setspecific() if the value is not <a
 * href="https://en.cppreference.com/w/c/types/NULL">NULL</a>, otherwise it is
 * deleted.
 * @endparblock
 */
#define CONFIGURE_MAXIMUM_POSIX_KEY_VALUE_PAIRS

/* Generated from spec:/acfg/if/max-posix-message-queues */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES
 *
 * The value of this configuration option defines the maximum number of POSIX
 * API Message Queues that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option shall be small enough so that the
 *   RTEMS Workspace size calculation carried out by ``<rtems/confdefs.h>``
 *   does not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/integer">uintptr_t</a>.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 * @endparblock
 *
 * @par Notes
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.  You have to account for the memory used to store the messages
 * of each message queue, see @ref CONFIGURE_MESSAGE_BUFFER_MEMORY.
 */
#define CONFIGURE_MAXIMUM_POSIX_MESSAGE_QUEUES

/* Generated from spec:/acfg/if/max-posix-queued-signals */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS
 *
 * The value of this configuration option defines the maximum number of POSIX
 * API Queued Signals that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option shall be small enough so that the
 *   RTEMS Workspace size calculation carried out by ``<rtems/confdefs.h>``
 *   does not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/integer">uintptr_t</a>.
 *
 * - The value of the configuration option shall be zero if the POSIX API is
 *   not enabled (e.g. RTEMS was built without the `RTEMS_POSIX_API = True`
 *   build configuration option).  Otherwise a compile time error in the
 *   configuration file will occur.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * Unlimited objects are not available for queued signals.
 *
 * Queued signals are only available if RTEMS was built with the POSIX API
 * build configuration option enabled.
 * @endparblock
 */
#define CONFIGURE_MAXIMUM_POSIX_QUEUED_SIGNALS

/* Generated from spec:/acfg/if/max-posix-semaphores */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_POSIX_SEMAPHORES
 *
 * The value of this configuration option defines the maximum number of POSIX
 * API Named Semaphores that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option shall be small enough so that the
 *   RTEMS Workspace size calculation carried out by ``<rtems/confdefs.h>``
 *   does not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/integer">uintptr_t</a>.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.
 *
 * Named semaphores are created with sem_open().  Semaphores initialized with
 * sem_init() are not affected by this configuration option since the storage
 * space for these semaphores is user-provided.
 * @endparblock
 */
#define CONFIGURE_MAXIMUM_POSIX_SEMAPHORES

/* Generated from spec:/acfg/if/max-posix-shms */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_POSIX_SHMS
 *
 * The value of this configuration option defines the maximum number of POSIX
 * API Shared Memory objects that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option shall be small enough so that the
 *   RTEMS Workspace size calculation carried out by ``<rtems/confdefs.h>``
 *   does not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/integer">uintptr_t</a>.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 * @endparblock
 *
 * @par Notes
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.
 */
#define CONFIGURE_MAXIMUM_POSIX_SHMS

/* Generated from spec:/acfg/if/max-posix-threads */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_POSIX_THREADS
 *
 * The value of this configuration option defines the maximum number of POSIX
 * API Threads that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option shall be small enough so that the
 *   task stack space calculation carried out by ``<rtems/confdefs.h>`` does
 *   not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/integer">uintptr_t</a>.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.
 *
 * This calculations for the required memory in the RTEMS Workspace for threads
 * assume that each thread has a minimum stack size and has floating point
 * support enabled.  The configuration option @ref CONFIGURE_EXTRA_TASK_STACKS
 * is used to specify thread stack requirements **above** the minimum size
 * required.
 *
 * The maximum number of Classic API Tasks is specified by @ref
 * CONFIGURE_MAXIMUM_TASKS.
 *
 * All POSIX threads have floating point enabled.
 * @endparblock
 */
#define CONFIGURE_MAXIMUM_POSIX_THREADS

/* Generated from spec:/acfg/if/max-posix-timers */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_POSIX_TIMERS
 *
 * The value of this configuration option defines the maximum number of POSIX
 * API Timers that can be concurrently active.
 *
 * @par Default Value
 * The default value is 0.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to
 *   65535.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 *
 * - The value of the configuration option may be defined through
 *   rtems_resource_unlimited() the enable unlimited objects for the object
 *   class, if the value passed to rtems_resource_unlimited() satisfies all
 *   other constraints of the configuration option.
 *
 * - The value of the configuration option shall be zero if the POSIX API is
 *   not enabled (e.g. RTEMS was built without the `RTEMS_POSIX_API = True`
 *   build configuration option).  Otherwise a compile time error in the
 *   configuration file will occur.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * This object class can be configured in unlimited allocation mode, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/intro.html#unlimited-objects">Unlimited
 * Objects</a>.
 *
 * Timers are only available if RTEMS was built with the POSIX API build
 * configuration option enabled.
 * @endparblock
 */
#define CONFIGURE_MAXIMUM_POSIX_TIMERS

/* Generated from spec:/acfg/if/min-posix-thread-stack-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE
 *
 * The value of this configuration option defines the minimum stack size in
 * bytes for every POSIX thread in the system.
 *
 * @par Default Value
 * The default value is two times the value of @ref
 * CONFIGURE_MINIMUM_TASK_STACK_SIZE.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be small enough so that the
 *   task stack space calculation carried out by ``<rtems/confdefs.h>`` does
 *   not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/integer">uintptr_t</a>.
 *
 * - The value of the configuration option shall be greater than or equal to a
 *   BSP-specific and application-specific minimum value.
 * @endparblock
 */
#define CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE

/** @} */

/* Generated from spec:/acfg/if/group-posixinit */

/**
 * @defgroup RTEMSApplConfigPOSIXInitializationThreadConfiguration \
 *   POSIX Initialization Thread Configuration
 *
 * @ingroup RTEMSApplConfig
 *
 * This section describes configuration options related to the POSIX
 * initialization thread.
 *
 * @{
 */

/* Generated from spec:/acfg/if/posix-init-thread-entry-point */

/**
 * @brief This configuration option is an initializer define.
 *
 * @anchor CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT
 *
 * The value of this configuration option initializes the entry point of the
 * POSIX API initialization thread.
 *
 * @par Default Value
 * The default value is `POSIX_Init`.
 *
 * @par Constraints
 * The value of the configuration option shall be defined to a valid function
 * pointer of the type `void *( *entry_point )( void * )`.
 *
 * @par Notes
 * The application shall provide the function referenced by this configuration
 * option.
 */
#define CONFIGURE_POSIX_INIT_THREAD_ENTRY_POINT

/* Generated from spec:/acfg/if/posix-init-thread-stack-size */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE
 *
 * The value of this configuration option defines the thread stack size of the
 * POSIX API initialization thread.
 *
 * @par Default Value
 * The default value is @ref CONFIGURE_MINIMUM_POSIX_THREAD_STACK_SIZE.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   @ref CONFIGURE_MINIMUM_TASK_STACK_SIZE.
 *
 * - The value of the configuration option shall be small enough so that the
 *   task stack space calculation carried out by ``<rtems/confdefs.h>`` does
 *   not overflow an integer of type <a
 *   href="https://en.cppreference.com/w/c/types/integer">uintptr_t</a>.
 * @endparblock
 */
#define CONFIGURE_POSIX_INIT_THREAD_STACK_SIZE

/* Generated from spec:/acfg/if/posix-init-thread-table */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_POSIX_INIT_THREAD_TABLE
 *
 * In case this configuration option is defined, then exactly one POSIX
 * initialization thread is configured.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * The application shall define at least one of the following configuration
 * options
 *
 * * @ref CONFIGURE_RTEMS_INIT_TASKS_TABLE,
 *
 * * `CONFIGURE_POSIX_INIT_THREAD_TABLE`, or
 *
 * * @ref CONFIGURE_IDLE_TASK_INITIALIZES_APPLICATION
 *
 * otherwise a compile time error in the configuration file will occur.
 *
 * If no Classic API initialization task is configured, then the POSIX API
 * initialization thread performs the <a
 * href="https://docs.rtems.org/branches/master/c-user/initialization/operations.html">Global
 * Construction</a>.
 * @endparblock
 */
#define CONFIGURE_POSIX_INIT_THREAD_TABLE

/** @} */

/* Generated from spec:/acfg/if/group-schedgeneral */

/**
 * @defgroup RTEMSApplConfigGeneralSchedulerConfiguration \
 *   General Scheduler Configuration
 *
 * @ingroup RTEMSApplConfig
 *
 * This section describes configuration options related to selecting a
 * scheduling algorithm for an application.  A scheduler configuration is
 * optional and only necessary in very specific circumstances.  A normal
 * application configuration does not need any of the configuration options
 * described in this section.
 *
 * By default, the <a
 * href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/uniprocessor-schedulers.html#deterministic-priority-scheduler">Deterministic
 * Priority Scheduler</a> algorithm is used in uniprocessor configurations.  In
 * case SMP is enabled and the configured maximum processors (@ref
 * CONFIGURE_MAXIMUM_PROCESSORS) is greater than one, then the <a
 * href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/smp-schedulers.html#earliest-deadline-first-smp-scheduler">Earliest
 * Deadline First SMP Scheduler</a> is selected as the default scheduler
 * algorithm.
 *
 * For the schedulers provided by RTEMS (see <a
 * href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/index.html">Scheduling
 * Concepts</a>), the configuration is straightforward.  All that is required
 * is to define the configuration option which specifies which scheduler you
 * want for in your application.
 *
 * The pluggable scheduler interface also enables the user to provide their own
 * scheduling algorithm.  If you choose to do this, you must define multiple
 * configuration option.
 *
 * @{
 */

/* Generated from spec:/acfg/if/cbs-max-servers */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_CBS_MAXIMUM_SERVERS
 *
 * The value of this configuration option defines the maximum number Constant
 * Bandwidth Servers that can be concurrently active.
 *
 * @par Default Value
 * The default value is @ref CONFIGURE_MAXIMUM_TASKS.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be greater than or equal to
 *   zero.
 *
 * - The value of the configuration option shall be less than or equal to <a
 *   href="https://en.cppreference.com/w/c/types/limits">SIZE_MAX</a>.
 *
 * - The value of the configuration option shall be less than or equal to a
 *   BSP-specific and application-specific value which depends on the size of
 *   the memory available to the application.
 * @endparblock
 *
 * @par Notes
 * This configuration option is only evaluated if the configuration option @ref
 * CONFIGURE_SCHEDULER_CBS is defined.
 */
#define CONFIGURE_CBS_MAXIMUM_SERVERS

/* Generated from spec:/acfg/if/max-priority */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_MAXIMUM_PRIORITY
 *
 * For the following schedulers
 *
 * * <a
 *   href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/uniprocessor-schedulers.html#deterministic-priority-scheduler">Deterministic
 *   Priority Scheduler</a>, which is the default in uniprocessor
 *   configurations and can be configured through the @ref
 *   CONFIGURE_SCHEDULER_PRIORITY configuration option,
 *
 * * <a
 *   href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/smp-schedulers.html#deterministic-priority-smp-scheduler">Deterministic
 *   Priority SMP Scheduler</a> which can be configured through the @ref
 *   CONFIGURE_SCHEDULER_PRIORITY_SMP configuration option, and
 *
 * * <a
 *   href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/smp-schedulers.html#arbitrary-processor-affinity-priority-smp-scheduler">Arbitrary
 *   Processor Affinity Priority SMP Scheduler</a> which can be configured
 *   through the @ref CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP configuration
 *   option
 *
 * this configuration option specifies the maximum numeric priority of any task
 * for these schedulers and one less that the number of priority levels for
 * these schedulers.  For all other schedulers provided by RTEMS, this
 * configuration option has no effect.
 *
 * @par Default Value
 * The default value is 255.
 *
 * @par Constraints
 * The value of the configuration option shall be equal to 3, 7, 31, 63, 127,
 * or 255.
 *
 * @par Notes
 * @parblock
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
 * is 12 bytes * (`CONFIGURE_MAXIMUM_PRIORITY` + 1), e.g. 3072 bytes for 256
 * priority levels (default), 48 bytes for 4 priority levels
 * (`CONFIGURE_MAXIMUM_PRIORITY == 3`).
 *
 * The default value is 255, because RTEMS shall support 256 priority levels to
 * be compliant with various standards.  These priorities range from 0 to 255.
 * @endparblock
 */
#define CONFIGURE_MAXIMUM_PRIORITY

/* Generated from spec:/acfg/if/scheduler-assignments */

/**
 * @brief This configuration option is an initializer define.
 *
 * @anchor CONFIGURE_SCHEDULER_ASSIGNMENTS
 *
 * The value of this configuration option is used to initialize the initial
 * scheduler to processor assignments.
 *
 * @par Default Value
 * The default value of this configuration option is computed so that the
 * default scheduler is assigned to each configured processor (up to 32).
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be a list of the following
 *   macros:
 *
 *   * `RTEMS_SCHEDULER_ASSIGN( scheduler_index, attributes )`
 *
 *   * `RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER`
 *
 *   The `scheduler_index` macro parameter shall be a valid index of the
 *   scheduler table defined by the @ref CONFIGURE_SCHEDULER_TABLE_ENTRIES
 *   configuration option.
 *
 *   The `attributes` macro parameter shall be set to exactly one of the
 *   following constants:
 *
 *   * `RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY`
 *
 *   * `RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL`
 *
 * - The value of the configuration option shall be a list of exactly @ref
 *   CONFIGURE_MAXIMUM_PROCESSORS elements.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * Where the system was built with SMP support enabled, this configuration
 * option is evaluated, otherwise it is ignored.
 *
 * This is an advanced configuration option, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/scheduler-clustered.html">Clustered
 * Scheduler Configuration</a>.
 * @endparblock
 */
#define CONFIGURE_SCHEDULER_ASSIGNMENTS

/* Generated from spec:/acfg/if/scheduler-cbs */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_SCHEDULER_CBS
 *
 * In case this configuration option is defined, then the <a
 * href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/uniprocessor-schedulers.html#constant-bandwidth-server-scheduling-cbs">Constant
 * Bandwidth Server Scheduling (CBS)</a> algorithm is made available to the
 * application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * In case no explicit <a
 * href="https://docs.rtems.org/branches/master/c-user/config/scheduler-clustered.html">Clustered
 * Scheduler Configuration</a> is present, then it is used as the scheduler for
 * exactly one processor.
 * @endparblock
 */
#define CONFIGURE_SCHEDULER_CBS

/* Generated from spec:/acfg/if/scheduler-edf */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_SCHEDULER_EDF
 *
 * In case this configuration option is defined, then the <a
 * href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/uniprocessor-schedulers.html#earliest-deadline-first-scheduler">Earliest
 * Deadline First Scheduler</a> algorithm is made available to the application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * In case no explicit <a
 * href="https://docs.rtems.org/branches/master/c-user/config/scheduler-clustered.html">Clustered
 * Scheduler Configuration</a> is present, then it is used as the scheduler for
 * exactly one processor.
 * @endparblock
 */
#define CONFIGURE_SCHEDULER_EDF

/* Generated from spec:/acfg/if/scheduler-edf-smp */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_SCHEDULER_EDF_SMP
 *
 * In case this configuration option is defined, then the <a
 * href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/smp-schedulers.html#earliest-deadline-first-smp-scheduler">Earliest
 * Deadline First SMP Scheduler</a> algorithm is made available to the
 * application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * This scheduler algorithm is only available when RTEMS is built with SMP
 * support enabled.
 *
 * In case no explicit <a
 * href="https://docs.rtems.org/branches/master/c-user/config/scheduler-clustered.html">Clustered
 * Scheduler Configuration</a> is present, then it is used as the scheduler for
 * up to 32 processors.
 *
 * This scheduler algorithm is the default in SMP configurations if @ref
 * CONFIGURE_MAXIMUM_PROCESSORS is greater than one.
 * @endparblock
 */
#define CONFIGURE_SCHEDULER_EDF_SMP

/* Generated from spec:/acfg/if/scheduler-name */

/**
 * @brief This configuration option is an integer define.
 *
 * @anchor CONFIGURE_SCHEDULER_NAME
 *
 * The value of this configuration option defines the name of the default
 * scheduler.
 *
 * @par Default Value
 * @parblock
 * The default value is
 *
 * * `"MEDF"` for the <a
 *   href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/smp-schedulers.html#earliest-deadline-first-smp-scheduler">Earliest
 *   Deadline First SMP Scheduler</a>,
 *
 * * `"MPA "` for the <a
 *   href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/smp-schedulers.html#arbitrary-processor-affinity-priority-smp-scheduler">Arbitrary
 *   Processor Affinity Priority SMP Scheduler</a>,
 *
 * * `"MPD "` for the <a
 *   href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/smp-schedulers.html#deterministic-priority-smp-scheduler">Deterministic
 *   Priority SMP Scheduler</a>,
 *
 * * `"MPS "` for the <a
 *   href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/smp-schedulers.html#simple-priority-smp-scheduler">Simple
 *   Priority SMP Scheduler</a>,
 *
 * * `"UCBS"` for the <a
 *   href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/uniprocessor-schedulers.html#constant-bandwidth-server-scheduling-cbs">Constant
 *   Bandwidth Server Scheduling (CBS)</a>,
 *
 * * `"UEDF"` for the <a
 *   href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/uniprocessor-schedulers.html#earliest-deadline-first-scheduler">Earliest
 *   Deadline First Scheduler</a>,
 *
 * * `"UPD "` for the <a
 *   href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/uniprocessor-schedulers.html#deterministic-priority-scheduler">Deterministic
 *   Priority Scheduler</a>, and
 *
 * * `"UPS "` for the <a
 *   href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/uniprocessor-schedulers.html#simple-priority-scheduler">Simple
 *   Priority Scheduler</a>.
 * @endparblock
 *
 * @par Constraints
 * The value of the configuration option shall be convertible to an integer of
 * type ::rtems_name.
 *
 * @par Notes
 * @parblock
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * Schedulers can be identified via rtems_scheduler_ident().
 *
 * Use rtems_build_name() to define the scheduler name.
 * @endparblock
 */
#define CONFIGURE_SCHEDULER_NAME

/* Generated from spec:/acfg/if/scheduler-priority */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_SCHEDULER_PRIORITY
 *
 * In case this configuration option is defined, then the <a
 * href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/uniprocessor-schedulers.html#deterministic-priority-scheduler">Deterministic
 * Priority Scheduler</a> algorithm is made available to the application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * In case no explicit <a
 * href="https://docs.rtems.org/branches/master/c-user/config/scheduler-clustered.html">Clustered
 * Scheduler Configuration</a> is present, then it is used as the scheduler for
 * exactly one processor.
 *
 * This scheduler algorithm is the default when @ref
 * CONFIGURE_MAXIMUM_PROCESSORS is exactly one.
 *
 * The memory allocated for this scheduler depends on the @ref
 * CONFIGURE_MAXIMUM_PRIORITY configuration option.
 * @endparblock
 */
#define CONFIGURE_SCHEDULER_PRIORITY

/* Generated from spec:/acfg/if/scheduler-priority-affinity-smp */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP
 *
 * In case this configuration option is defined, then the <a
 * href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/smp-schedulers.html#arbitrary-processor-affinity-priority-smp-scheduler">Arbitrary
 * Processor Affinity Priority SMP Scheduler</a> algorithm is made available to
 * the application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * This scheduler algorithm is only available when RTEMS is built with SMP
 * support enabled.
 *
 * In case no explicit <a
 * href="https://docs.rtems.org/branches/master/c-user/config/scheduler-clustered.html">Clustered
 * Scheduler Configuration</a> is present, then it is used as the scheduler for
 * up to 32 processors.
 *
 * The memory allocated for this scheduler depends on the @ref
 * CONFIGURE_MAXIMUM_PRIORITY configuration option.
 * @endparblock
 */
#define CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP

/* Generated from spec:/acfg/if/scheduler-priority-smp */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_SCHEDULER_PRIORITY_SMP
 *
 * In case this configuration option is defined, then the <a
 * href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/smp-schedulers.html#deterministic-priority-smp-scheduler">Deterministic
 * Priority SMP Scheduler</a> algorithm is made available to the application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * This scheduler algorithm is only available when RTEMS is built with SMP
 * support enabled.
 *
 * In case no explicit <a
 * href="https://docs.rtems.org/branches/master/c-user/config/scheduler-clustered.html">Clustered
 * Scheduler Configuration</a> is present, then it is used as the scheduler for
 * up to 32 processors.
 *
 * The memory allocated for this scheduler depends on the @ref
 * CONFIGURE_MAXIMUM_PRIORITY configuration option.
 * @endparblock
 */
#define CONFIGURE_SCHEDULER_PRIORITY_SMP

/* Generated from spec:/acfg/if/scheduler-simple */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_SCHEDULER_SIMPLE
 *
 * In case this configuration option is defined, then the <a
 * href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/uniprocessor-schedulers.html#simple-priority-scheduler">Simple
 * Priority Scheduler</a> algorithm is made available to the application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * In case no explicit <a
 * href="https://docs.rtems.org/branches/master/c-user/config/scheduler-clustered.html">Clustered
 * Scheduler Configuration</a> is present, then it is used as the scheduler for
 * exactly one processor.
 * @endparblock
 */
#define CONFIGURE_SCHEDULER_SIMPLE

/* Generated from spec:/acfg/if/scheduler-simple-smp */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_SCHEDULER_SIMPLE_SMP
 *
 * In case this configuration option is defined, then the <a
 * href="https://docs.rtems.org/branches/master/c-user/scheduling-concepts/smp-schedulers.html#simple-priority-smp-scheduler">Simple
 * Priority SMP Scheduler</a> algorithm is made available to the application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * This scheduler algorithm is only available when RTEMS is built with SMP
 * support enabled.
 *
 * In case no explicit <a
 * href="https://docs.rtems.org/branches/master/c-user/config/scheduler-clustered.html">Clustered
 * Scheduler Configuration</a> is present, then it is used as the scheduler for
 * up to 32 processors.
 * @endparblock
 */
#define CONFIGURE_SCHEDULER_SIMPLE_SMP

/* Generated from spec:/acfg/if/scheduler-strong-apa */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_SCHEDULER_STRONG_APA
 *
 * In case this configuration option is defined, then the Strong APA algorithm
 * is made available to the application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * This scheduler algorithm is only available when RTEMS is built with SMP
 * support enabled.
 *
 * This scheduler algorithm is not correctly implemented.  Do not use it.
 * @endparblock
 */
#define CONFIGURE_SCHEDULER_STRONG_APA

/* Generated from spec:/acfg/if/scheduler-table-entries */

/**
 * @brief This configuration option is an initializer define.
 *
 * @anchor CONFIGURE_SCHEDULER_TABLE_ENTRIES
 *
 * The value of this configuration option is used to initialize the table of
 * configured schedulers.
 *
 * @par Default Value
 * The default value of this configuration option is the definition of exactly
 * one table entry for the configured scheduler.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be a list of the following
 *   macros:
 *
 *   * `RTEMS_SCHEDULER_TABLE_CBS( name, obj_name )`
 *
 *   * `RTEMS_SCHEDULER_TABLE_EDF( name, obj_name )`
 *
 *   * `RTEMS_SCHEDULER_TABLE_EDF_SMP( name, obj_name )`
 *
 *   * `RTEMS_SCHEDULER_TABLE_PRIORITY_AFFINITY_SMP( name, obj_name )`
 *
 *   * `RTEMS_SCHEDULER_TABLE_PRIORITY( name, obj_name )`
 *
 *   * `RTEMS_SCHEDULER_TABLE_PRIORITY_SMP( name, obj_name )`
 *
 *   * `RTEMS_SCHEDULER_TABLE_SIMPLE( name, obj_name )`
 *
 *   * `RTEMS_SCHEDULER_TABLE_SIMPLE_SMP( name, obj_name )`
 *
 *   * `RTEMS_SCHEDULER_TABLE_STRONG_APA( name, obj_name )`
 *
 *   The `name` macro parameter shall be the name associated with the scheduler
 *   data structures, see <a
 *   href="https://docs.rtems.org/branches/master/c-user/config/scheduler-clustered.html">Clustered
 *   Scheduler Configuration</a>.
 *
 *   The `obj_name` macro parameter shall be the scheduler object name.  It is
 *   recommended to define the scheduler object name through
 *   rtems_build_name().
 *
 * - Where the system was build with SMP support enabled, the table shall have
 *   one or more entries, otherwise it shall have exactly one entry.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * Schedulers registered in the scheduler table by this configuration option
 * are available to the application.  The scheduler table entry index defines
 * the index of the scheduler.
 *
 * This is an advanced configuration option, see <a
 * href="https://docs.rtems.org/branches/master/c-user/config/scheduler-clustered.html">Clustered
 * Scheduler Configuration</a>.
 * @endparblock
 */
#define CONFIGURE_SCHEDULER_TABLE_ENTRIES

/* Generated from spec:/acfg/if/scheduler-user */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_SCHEDULER_USER
 *
 * In case this configuration option is defined, then the user shall provide a
 * scheduler algorithm to the application.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * @parblock
 * This scheduler configuration option is an advanced configuration option.
 * Think twice before you use it.
 *
 * RTEMS allows the application to provide its own task/thread scheduling
 * algorithm. In order to do this, one shall define `CONFIGURE_SCHEDULER_USER`
 * to indicate the application provides its own scheduling algorithm. If
 * `CONFIGURE_SCHEDULER_USER` is defined then the following additional macros
 * shall be defined:
 *
 * * `CONFIGURE_SCHEDULER` shall be defined to a static definition of the
 *   scheduler data structures of the user scheduler.
 *
 * * `CONFIGURE_SCHEDULER_TABLE_ENTRIES` shall be defined to a scheduler table
 *   entry initializer for the user scheduler.
 *
 * * `CONFIGURE_SCHEDULER_USER_PER_THREAD` shall be defined to the type of the
 *   per-thread information of the user scheduler.
 *
 * At this time, the mechanics and requirements for writing a new scheduler are
 * evolving and not fully documented.  It is recommended that you look at the
 * existing Deterministic Priority Scheduler in
 * `cpukit/score/src/schedulerpriority*.c` for guidance.  For guidance on the
 * configuration macros, please examine
 * `cpukit/include/rtems/confdefs/scheduler.h` for how these are defined for
 * the Deterministic Priority Scheduler.
 * @endparblock
 */
#define CONFIGURE_SCHEDULER_USER

/** @} */

/* Generated from spec:/acfg/if/group-stackalloc */

/**
 * @defgroup RTEMSApplConfigTaskStackAllocatorConfiguration \
 *   Task Stack Allocator Configuration
 *
 * @ingroup RTEMSApplConfig
 *
 * This section describes configuration options related to the task stack
 * allocator.  RTEMS allows the application or BSP to define its own allocation
 * and deallocation methods for task stacks. This can be used to place task
 * stacks in special areas of memory or to utilize a Memory Management Unit so
 * that stack overflows are detected in hardware.
 *
 * @{
 */

/* Generated from spec:/acfg/if/task-stack-allocator */

/**
 * @brief This configuration option is an initializer define.
 *
 * @anchor CONFIGURE_TASK_STACK_ALLOCATOR
 *
 * The value of this configuration option initializes the stack allocator
 * allocate handler.
 *
 * @par Default Value
 * The default value is `_Workspace_Allocate`, which indicates that task stacks
 * will be allocated from the RTEMS Workspace.
 *
 * @par Constraints
 * The value of the configuration option shall be defined to a valid function
 * pointer of the type `void *( *allocate )( size_t )`.
 *
 * @par Notes
 * @parblock
 * A correctly configured system shall configure the following to be
 * consistent:
 *
 * * @ref CONFIGURE_TASK_STACK_ALLOCATOR_INIT
 *
 * * `CONFIGURE_TASK_STACK_ALLOCATOR`
 *
 * * @ref CONFIGURE_TASK_STACK_DEALLOCATOR
 * @endparblock
 */
#define CONFIGURE_TASK_STACK_ALLOCATOR

/* Generated from spec:/acfg/if/task-stack-no-workspace */

/**
 * @brief This configuration option is a boolean feature define.
 *
 * @anchor CONFIGURE_TASK_STACK_ALLOCATOR_AVOIDS_WORK_SPACE
 *
 * In case this configuration option is defined, then the system is informed
 * that the task stack allocator does not use the RTEMS Workspace.
 *
 * @par Default Configuration
 * If this configuration option is undefined, then the described feature is not
 * enabled.
 *
 * @par Notes
 * This configuration option may be used if a custom task stack allocator is
 * configured, see @ref CONFIGURE_TASK_STACK_ALLOCATOR.
 */
#define CONFIGURE_TASK_STACK_ALLOCATOR_AVOIDS_WORK_SPACE

/* Generated from spec:/acfg/if/task-stack-allocator-for-idle */

/**
 * @brief This configuration option is an initializer define.
 *
 * @anchor CONFIGURE_TASK_STACK_ALLOCATOR_FOR_IDLE
 *
 * The value of this configuration option is the address for the stack
 * allocator allocate handler used to allocate the task storage area of each
 * IDLE task.
 *
 * @par Default Value
 * By default, the IDLE task storage area will be allocated from the RTEMS
 * Workspace.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this configuration option:
 *
 * - The value of the configuration option shall be defined to a valid function
 *   pointer of the type `void *( *allocate )( uint32_t, size_t * )`.
 *
 * - The IDLE task stack allocator shall return a pointer to the allocated
 *   memory area or terminate the system with a fatal error if the allocation
 *   request cannot be satisfied.
 *
 * - The IDLE task stack allocator may increase the size of the allocated
 *   memory area.
 * @endparblock
 *
 * @par Notes
 * @parblock
 * This configuration option is independent of the other thread stack allocator
 * configuration options.  It is assumed that any memory allocated for the task
 * storage area of an IDLE task will not be from the RTEMS Workspace.
 *
 * The IDLE task stack allocator may increase the size of the allocated memory
 * area to account for the actually allocated memory area.
 *
 * The
 *
 * * @ref CONFIGURE_IDLE_TASK_STORAGE_SIZE, and
 *
 * * `CONFIGURE_TASK_STACK_ALLOCATOR_FOR_IDLE`
 *
 * configuration options are mutually exclusive.
 * @endparblock
 */
#define CONFIGURE_TASK_STACK_ALLOCATOR_FOR_IDLE

/* Generated from spec:/acfg/if/task-stack-allocator-init */

/**
 * @brief This configuration option is an initializer define.
 *
 * @anchor CONFIGURE_TASK_STACK_ALLOCATOR_INIT
 *
 * The value of this configuration option initializes the stack allocator
 * initialization handler.
 *
 * @par Default Value
 * The default value is <a
 * href="https://en.cppreference.com/w/c/types/NULL">NULL</a>.
 *
 * @par Constraints
 * The value of the configuration option shall be defined to a valid function
 * pointer of the type `void ( *initialize )( size_t )` or to <a
 * href="https://en.cppreference.com/w/c/types/NULL">NULL</a>.
 *
 * @par Notes
 * @parblock
 * A correctly configured system shall configure the following to be
 * consistent:
 *
 * * `CONFIGURE_TASK_STACK_ALLOCATOR_INIT`
 *
 * * @ref CONFIGURE_TASK_STACK_ALLOCATOR
 *
 * * @ref CONFIGURE_TASK_STACK_DEALLOCATOR
 * @endparblock
 */
#define CONFIGURE_TASK_STACK_ALLOCATOR_INIT

/* Generated from spec:/acfg/if/task-stack-deallocator */

/**
 * @brief This configuration option is an initializer define.
 *
 * @anchor CONFIGURE_TASK_STACK_DEALLOCATOR
 *
 * The value of this configuration option initializes the stack allocator
 * deallocate handler.
 *
 * @par Default Value
 * The default value is `_Workspace_Free`, which indicates that task stacks
 * will be allocated from the RTEMS Workspace.
 *
 * @par Constraints
 * The value of the configuration option shall be defined to a valid function
 * pointer of the type `void ( *deallocate )( void * )`.
 *
 * @par Notes
 * @parblock
 * A correctly configured system shall configure the following to be
 * consistent:
 *
 * * @ref CONFIGURE_TASK_STACK_ALLOCATOR_INIT
 *
 * * @ref CONFIGURE_TASK_STACK_ALLOCATOR
 *
 * * `CONFIGURE_TASK_STACK_DEALLOCATOR`
 * @endparblock
 */
#define CONFIGURE_TASK_STACK_DEALLOCATOR

/* Generated from spec:/acfg/if/task-stack-from-alloc */

/**
 * @brief This configuration option is an initializer define.
 *
 * @anchor CONFIGURE_TASK_STACK_FROM_ALLOCATOR
 *
 * The value of this configuration option is used to calculate the task stack
 * space size.
 *
 * @par Default Value
 * The default value is a macro which supports the system heap allocator.
 *
 * @par Constraints
 * The value of the configuration option shall be defined to a macro which
 * accepts exactly one parameter and returns an unsigned integer.  The
 * parameter will be an allocation size and the macro shall return this size
 * plus the overhead of the allocator to manage an allocation request for this
 * size.
 *
 * @par Notes
 * This configuration option may be used if a custom task stack allocator is
 * configured, see @ref CONFIGURE_TASK_STACK_ALLOCATOR.
 */
#define CONFIGURE_TASK_STACK_FROM_ALLOCATOR

/** @} */
