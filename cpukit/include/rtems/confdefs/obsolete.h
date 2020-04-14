/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplicationConfiguration
 *
 * @brief Evaluate Obsolete Configuration Options
 */

/*
 * Copyright (C) 2017, 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef _RTEMS_CONFDEFS_OBSOLETE_H
#define _RTEMS_CONFDEFS_OBSOLETE_H

#ifndef __CONFIGURATION_TEMPLATE_h
#error "Do not include this file directly, use <rtems/confdefs.h> instead"
#endif

#ifdef CONFIGURE_INIT

/*
 * Please keep the list of obsolete configuration options alphabetically sorted.
 *
 * Use #warning for renamed options and define the new option accordingly.
 *
 * Use #warning for obsolete options which are now superfluous, e.g. because
 * the objects are now self-contained.
 *
 * Use #error for options which require now a different configuration approach,
 * e.g. options for an own configuration table.
 *
 * Mention the RTEMS release which obsoleted the configuration option in the
 * message.
 */

#ifdef CONFIGURE_BDBUF_BUFFER_SIZE
  #error "The CONFIGURE_BDBUF_BUFFER_SIZE configuration option is obsolete since RTEMS 4.10.0"
#endif

#ifdef CONFIGURE_BDBUF_BUFFER_COUNT
  #error "The CONFIGURE_BDBUF_BUFFER_COUNT configuration option is obsolete since RTEMS 4.10.0"
#endif

#ifdef CONFIGURE_MAXIMUM_DEVICES
  #warning "The CONFIGURE_MAXIMUM_DEVICES configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_ENABLE_GO
  #warning "The CONFIGURE_ENABLE_GO configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_GNAT_RTEMS
  #warning "The CONFIGURE_GNAT_RTEMS configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_HAS_OWN_CONFIGURATION_TABLE
  #error "The CONFIGURE_HAS_OWN_CONFIGURATION_TABLE configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_HAS_OWN_BDBUF_TABLE
  #error "The CONFIGURE_HAS_BDBUF_TABLE configuration option is obsolete since RTEMS 4.10.0"
#endif

#ifdef CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE
  #error "The CONFIGURE_HAS_OWN_DEVICE_DRIVER_TABLE configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_HAS_OWN_FILESYSTEM_TABLE
  #error "The CONFIGURE_HAS_OWN_FILESYSTEM_TABLE configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_HAS_OWN_INIT_TABLE
  #error "The CONFIGURE_HAS_OWN_INIT_TABLE configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_HAS_OWN_MOUNT_TABLE
  #error "The CONFIGURE_HAS_OWN_MOUNT_TABLE configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_HAS_OWN_MULTIPROCESSING_TABLE
  #error "The CONFIGURE_HAS_OWN_MULTIPROCESSING_TABLE configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS
  #warning "CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS has been renamed to CONFIGURE_MAXIMUM_FILE_DESCRIPTORS since RTEMS 5.1"
  #define CONFIGURE_MAXIMUM_FILE_DESCRIPTORS CONFIGURE_LIBIO_MAXIMUM_FILE_DESCRIPTORS
#endif

#ifdef CONFIGURE_MAXIMUM_ADA_TASKS
  #warning "The CONFIGURE_MAXIMUM_ADA_TASKS configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_MAXIMUM_FAKE_ADA_TASKS
  #warning "The CONFIGURE_MAXIMUM_FAKE_ADA_TASKS configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_MAXIMUM_FIFOS
  #warning "CONFIGURE_MAXIMUM_FIFOS is obsolete since RTEMS 5.1; use CONFIGURE_IMFS_ENABLE_MKFIFO instead"
  #if CONFIGURE_MAXIMUM_FIFOS > 0
    #define CONFIGURE_IMFS_ENABLE_MKFIFO
  #endif
#endif

#ifdef CONFIGURE_MAXIMUM_GOROUTINES
  #warning "The CONFIGURE_MAXIMUM_GOROUTINES configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_MAXIMUM_GO_CHANNELS
  #warning "The CONFIGURE_MAXIMUM_GO_CHANNELS configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_MAXIMUM_MRSP_SEMAPHORES
  #warning "The CONFIGURE_MAXIMUM_MRSP_SEMAPHORES configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_MAXIMUM_PIPES
  #warning "CONFIGURE_MAXIMUM_PIPES is obsolete since RTEMS 5.1; use CONFIGURE_IMFS_ENABLE_MKFIFO instead"
  #if CONFIGURE_MAXIMUM_PIPES > 0
    #define CONFIGURE_IMFS_ENABLE_MKFIFO
  #endif
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

#ifdef CONFIGURE_NUMBER_OF_TERMIOS_PORTS
  #warning "The CONFIGURE_NUMBER_OF_TERMIOS_PORTS configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_POSIX_HAS_OWN_INIT_THREAD_TABLE
  #error "The CONFIGURE_POSIX_HAS_OWN_INIT_THREAD_TABLE configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_SCHEDULER_CONTEXT
  #warning "CONFIGURE_SCHEDULER_CONTEXT has been renamed to CONFIGURE_SCHEDULER since RTEMS 5.1"
  #define CONFIGURE_SCHEDULER CONFIGURE_SCHEDULER_CONTEXT
#endif

#ifdef CONFIGURE_SCHEDULER_CONTROLS
  #warning "CONFIGURE_SCHEDULER_CONTROLS has been renamed to CONFIGURE_SCHEDULER_TABLE_ENTRIES since RTEMS 5.1"
  #define CONFIGURE_SCHEDULER_TABLE_ENTRIES CONFIGURE_SCHEDULER_CONTROLS
#endif

#ifdef CONFIGURE_SMP_APPLICATION
  #warning "CONFIGURE_SMP_APPLICATION is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_SMP_MAXIMUM_PROCESSORS
  #warning "CONFIGURE_SMP_MAXIMUM_PROCESSORS has been renamed to CONFIGURE_MAXIMUM_PROCESSORS since RTEMS 5.1"
  #define CONFIGURE_MAXIMUM_PROCESSORS CONFIGURE_SMP_MAXIMUM_PROCESSORS
#endif

#ifdef CONFIGURE_SMP_SCHEDULER_ASSIGNMENTS
  #warning "CONFIGURE_SMP_SCHEDULER_ASSIGNMENTS has been renamed to CONFIGURE_SCHEDULER_ASSIGNMENTS since RTEMS 5.1"
  #define CONFIGURE_SCHEDULER_ASSIGNMENTS CONFIGURE_SMP_SCHEDULER_ASSIGNMENTS
#endif

#ifdef CONFIGURE_TERMIOS_DISABLED
  #warning "The CONFIGURE_TERMIOS_DISABLED configuration option is obsolete since RTEMS 5.1"
#endif

#ifdef CONFIGURE_TEST_NEEDS_CLOCK_DRIVER
  #error "The CONFIGURE_TEST_NEEDS_CLOCK_DRIVER configuration option is obsolete since at least RTEMS 4.5.0"
#endif

#ifdef CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER
  #error "The CONFIGURE_TEST_NEEDS_CONSOLE_DRIVER configuration option is obsolete at least RTEMS 4.5.0"
#endif

#ifdef CONFIGURE_TEST_NEEDS_RTC_DRIVER
  #error "The CONFIGURE_TEST_NEEDS_RTC_DRIVER configuration option is obsolete at least RTEMS 4.5.0"
#endif

#ifdef CONFIGURE_TEST_NEEDS_STUB_DRIVER
  #error "The CONFIGURE_TEST_NEEDS_STUB_DRIVER configuration option is obsolete at least RTEMS 4.5.0"
#endif

#ifdef CONFIGURE_TEST_NEEDS_TIMER_DRIVER
  #error "The CONFIGURE_TEST_NEEDS_TIMER_DRIVER configuration option is obsolete at least RTEMS 4.5.0"
#endif

#ifdef STACK_CHECKER_ON
  #warning "STACK_CHECKER_ON has been renamed to CONFIGURE_STACK_CHECKER_ENABLED since RTEMS 4.10.0"
  #define CONFIGURE_STACK_CHECKER_ENABLED
#endif

#endif /* CONFIGURE_INIT */

#endif /* _RTEMS_CONFDEFS_OBSOLETE_H */
