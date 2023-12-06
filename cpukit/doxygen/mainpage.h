/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplDoxygen
 *
 * @brief This file exists to provide a top level description of RTEMS for
 *   Doxygen.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
 * Copyright (C) 1989, 2014 On-Line Applications Research Corporation (OAR)
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
 * @mainpage
 *
 * The Real-Time Executive for Multiprocessor Systems (RTEMS) is a
 * multi-threaded, single address-space, real-time operating system with no
 * kernel-space/user-space separation.  It is capable to operate in an SMP
 * configuration providing a state of the art feature set.
 *
 * RTEMS and all third-party software distributed with RTEMS which may be
 * linked to the application is licensed under permissive open source licenses.
 * This means that the licenses do not propagate to the application software.
 * Most of the original RTEMS code is now under the [BSD 2-Clause
 * license](https://git.rtems.org/rtems/tree/LICENSE.BSD-2-Clause).  Some
 * code of RTEMS is under a legacy license, the [modified GPL 2.0 or later
 * license with an exception for static
 * linking](https://git.rtems.org/rtems/tree/LICENSE).  It exposes no license
 * requirements on application code.  Everything necessary to build RTEMS
 * applications is available as open source software.  This makes you
 * completely vendor independent.
 *
 * RTEMS provides the following basic feature set:
 *
 * - @ref RTEMSAPI
 *
 *     - POSIX with
 *       [pthreads](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/pthread.h.html)
 *       (enables a broad range of standard software to run on RTEMS)
 *
 *     - @ref RTEMSAPIClassic
 *
 *     - C11 (including
 *       [thread](https://en.cppreference.com/w/c/thread) support)
 *
 *     - C++11 (including
 *       [thread](https://en.cppreference.com/w/cpp/thread>) support)
 *
 *     - Newlib and GCC internal
 *
 * - Programming languages
 *
 *     - C/C++/OpenMP (RTEMS Source Builder, RSB)
 *
 *     - Ada (RSB, ``--with-ada``)
 *
 *     - Erlang
 *
 *     - Fortran (RSB, ``--with-fortran``)
 *
 *     - Python and MicroPython
 *
 * - Parallel languages
 *
 *     - [Embedded Multicore Building Blocks](https://embb.io/)
 *
 *     - [OpenMP](https://www.openmp.org/)
 *
 * - Thread synchronization and communication
 *
 *     - Mutexes with and without locking protocols
 *
 *     - Counting semaphores
 *
 *     - Binary semaphores
 *
 *     - Condition variables
 *
 *     - Events
 *
 *     - Message queues
 *
 *     - Barriers
 *
 *     - [Futex](@ref RTEMSScoreFutex) (used by OpenMP barriers)
 *
 *     - Epoch Based Reclamation (libbsd)
 *
 * - Locking protocols
 *
 *     - Transitive Priority Inheritance
 *
 *     - OMIP (SMP feature)
 *
 *     - Priority Ceiling
 *
 *     - MrsP (SMP feature)
 *
 * - Scalable timer and timeout support
 *
 * - Lock-free timestamps (FreeBSD timecounters)
 *
 * - Responsive interrupt management
 *
 * - Thread-Local Storage (TLS) as specified by C11/C++11 and later standard
 *   revisions
 *
 * - Link-time configurable schedulers
 *
 *     - Fixed-priority
 *
 *     - Job-level fixed-priority (EDF)
 *
 *     - Constant Bandwidth Server (experimental)
 *
 * - Clustered scheduling (SMP feature)
 *
 *     - Flexible link-time configuration
 *
 *     - Job-level fixed-priority scheduler (EDF) with support for one-to-one
 *        and one-to-all thread to processor affinities (default SMP scheduler)
 *
 *     - Fixed-priority scheduler
 *
 *     - Proof-of-concept strong APA scheduler
 *
 * - Focus on link-time application-specific configuration
 *
 * - Linker-set based initialization (similar to global C++ constructors)
 *
 * - Operating system uses fine-grained locking (SMP feature)
 *
 * - Dynamic memory allocators
 *
 *     - First-fit (default)
 *
 *     - Universal Memory Allocator
 *       ([UMA](https://www.freebsd.org/cgi/man.cgi?query=uma&sektion=9),
 *       libbsd)
 *
 * - File systems
 *
 *     - IMFS
 *
 *     - FAT
 *
 *     - RFS
 *
 *     - NFSv2
 *
 *     - JFFS2 (NOR flashes)
 *
 *     - [YAFFS2](https://git.rtems.org/sebh/rtems-yaffs2.git/)
 *       (NAND flashes, GPL or commercial license required)
 *
 * - Device drivers
 *
 *     - Termios (serial interfaces)
 *
 *     - I2C (Linux user-space API compatible)
 *
 *     - SPI (Linux user-space API compatible)
 *
 *     - Network stacks (legacy, libbsd, lwIP)
 *
 *     - USB stack (libbsd)
 *
 *     - SD/MMC card stack (libbsd)
 *
 *     - Framebuffer (Linux user-space API compatible, Qt)
 *
 *     - Application runs in kernel-space and can access hardware directly
 *
 * - [libbsd](https://git.rtems.org/rtems-libbsd/)
 *
 *     - Port of FreeBSD user-space and kernel-space components to RTEMS
 *
 *     - Easy access to FreeBSD software for RTEMS
 *
 *     - Support to stay in synchronization with FreeBSD
 */
