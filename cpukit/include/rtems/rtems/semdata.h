/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicSemaphore
 *
 * @brief This header file provides data structures used by the implementation
 *   and the @ref RTEMSImplApplConfig to define ::_Semaphore_Information.
 */

/*
 * COPYRIGHT (c) 1989-2008, 2016.
 * On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_RTEMS_SEMDATA_H
#define _RTEMS_RTEMS_SEMDATA_H

#include <rtems/rtems/sem.h>
#include <rtems/score/coremutex.h>
#include <rtems/score/coresem.h>
#include <rtems/score/mrsp.h>
#include <rtems/score/object.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSImplClassicSemaphore
 *
 * @{
 */

/**
 *  The following defines the control block used to manage each semaphore.
 */
typedef struct {
  /**
   * @brief The object management portion of a semaphore instance.
   *
   * A pointer of the node of active semaphores contains the semaphore flags,
   * see _Semaphore_Get_flags().  The rational for this optimization is a
   * reduction of the semaphore control size in general and the ability to
   * allow a configuration dependent size of the semaphore control block, e.g.
   * for the MrsP semaphores.
   */
  Objects_Control          Object;

  /**
   *  This contains the memory associated with the SuperCore Semaphore or
   *  Mutex instance that provides the primary functionality of each
   *  Classic API Semaphore instance.  The structure used is dependent
   *  on the attributes specified by the user on the create directive.
   *
   *  @note Only one of these has meaning in a particular Classic API
   *        Semaphore instance.
   */
  union {
    /**
     * @brief The thread queue present in all other variants.
     */
    Thread_queue_Control Wait_queue;

    /**
     *  This is the SuperCore Mutex instance associated with this Classic
     *  API Semaphore instance.
     */
    CORE_ceiling_mutex_Control Mutex;

    /**
     *  This is the SuperCore Semaphore instance associated with this Classic
     *  API Semaphore instance.
     */
    CORE_semaphore_Control Semaphore;

#if defined(RTEMS_SMP)
    MRSP_Control MRSP;
#endif
  } Core_control;
}   Semaphore_Control;

/**
 * @brief The Classic Semaphore objects information.
 */
extern Objects_Information _Semaphore_Information;

#if defined(RTEMS_MULTIPROCESSING)
/**
 *  @brief Semaphore MP Send Extract Proxy
 *
 *  This routine is invoked when a task is deleted and it
 *  has a proxy which must be removed from a thread queue and
 *  the remote node must be informed of this.
 */
void _Semaphore_MP_Send_extract_proxy (
  Thread_Control *the_thread,
  Objects_Id      id
);
#endif

/**
 * @brief Macro to define the objects information for the Classic Semaphore
 * objects.
 *
 * This macro should only be used by <rtems/confdefs.h>.
 *
 * @param max The configured object maximum (the OBJECTS_UNLIMITED_OBJECTS flag
 *   may be set).
 * @param scheduler_count The configured scheduler count (only used in SMP
 *   configurations).
 */
#if defined(RTEMS_SMP)
#define SEMAPHORE_INFORMATION_DEFINE( max, scheduler_count ) \
  typedef union { \
    Objects_Control Object; \
    Semaphore_Control Semaphore; \
    struct { \
      Objects_Control Object; \
      MRSP_Control Control; \
      Priority_Control ceiling_priorities[ scheduler_count ]; \
    } MRSP; \
  } Semaphore_Configured_control; \
  OBJECTS_INFORMATION_DEFINE( \
    _Semaphore, \
    OBJECTS_CLASSIC_API, \
    OBJECTS_RTEMS_SEMAPHORES, \
    Semaphore_Configured_control, \
    max, \
    OBJECTS_NO_STRING_NAME, \
    _Semaphore_MP_Send_extract_proxy \
  )
#else
#define SEMAPHORE_INFORMATION_DEFINE( max, scheduler_count ) \
  OBJECTS_INFORMATION_DEFINE( \
    _Semaphore, \
    OBJECTS_CLASSIC_API, \
    OBJECTS_RTEMS_SEMAPHORES, \
    Semaphore_Control, \
    max, \
    OBJECTS_NO_STRING_NAME, \
    _Semaphore_MP_Send_extract_proxy \
  )
#endif

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
