/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup TermiostypesSupport
 *
 * @brief This header file provides the interfaces of the
 *   @ref TermiostypesSupport.
 */

/*
 * Copyright (C) 2014, 2017 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_TERMIOSDEVICE_H
#define _RTEMS_TERMIOSDEVICE_H

#include <rtems/thread.h>
#include <rtems/rtems/intr.h>

#include <sys/ioccom.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct rtems_libio_open_close_args;
struct rtems_termios_tty;
struct termios;

/**
 * @defgroup TermiostypesSupport RTEMS Termios Device Support
 *
 * @ingroup libcsupport
 *
 * @brief This group contains the Termios Device Support provided by RTEMS.
 */

/**
 * @brief Termios device context.
 *
 * @see RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER(),
 * rtems_termios_device_context_initialize() and
 * rtems_termios_device_install().
 */
typedef struct rtems_termios_device_context {
  union {
    /* Used for TERMIOS_POLLED and TERMIOS_IRQ_DRIVEN */
    rtems_interrupt_lock interrupt;

    /* Used for TERMIOS_IRQ_SERVER_DRIVEN and TERMIOS_TASK_DRIVEN */
    rtems_mutex mutex;
  } lock;

  void ( *lock_acquire )(
    struct rtems_termios_device_context *,
    rtems_interrupt_lock_context *
  );

  void ( *lock_release )(
    struct rtems_termios_device_context *,
    rtems_interrupt_lock_context *
  );
} rtems_termios_device_context;

typedef enum {
  TERMIOS_POLLED,
  TERMIOS_IRQ_DRIVEN,
  TERMIOS_TASK_DRIVEN,
  TERMIOS_IRQ_SERVER_DRIVEN
} rtems_termios_device_mode;

/**
 * @brief Termios device handler.
 *
 * @see rtems_termios_device_install().
 */
typedef struct {
  /**
   * @brief First open of this device.
   *
   * @param[in] tty The Termios control.  This parameter may be passed to
   *   interrupt service routines since it must be provided for the
   *   rtems_termios_enqueue_raw_characters() and
   *   rtems_termios_dequeue_characters() functions.
   * @param[in] context The Termios device context.
   * @param[in] term The current Termios attributes.
   * @param[in] args The open/close arguments.  This is parameter provided to
   *   support legacy drivers.  It must not be used by new drivers.
   *
   * @retval true Successful operation.
   * @retval false Cannot open device.
   *
   * @see rtems_termios_get_device_context() and rtems_termios_set_best_baud().
   */
  bool (*first_open)(
    struct rtems_termios_tty           *tty,
    rtems_termios_device_context       *context,
    struct termios                     *term,
    struct rtems_libio_open_close_args *args
  );

  /**
   * @brief Last close of this device.
   *
   * @param[in] tty The Termios control.
   * @param[in] context The Termios device context.
   * @param[in] args The open/close arguments.  This is parameter provided to
   *   support legacy drivers.  It must not be used by new drivers.
   */
  void (*last_close)(
    struct rtems_termios_tty           *tty,
    rtems_termios_device_context       *context,
    struct rtems_libio_open_close_args *args
  );

  /**
   * @brief Polled read.
   *
   * In case mode is TERMIOS_IRQ_DRIVEN, TERMIOS_IRQ_SERVER_DRIVEN or
   * TERMIOS_TASK_DRIVEN, then data is received via
   * rtems_termios_enqueue_raw_characters().
   *
   * @param[in] context The Termios device context.
   *
   * @retval char The received data encoded as unsigned char.
   * @retval -1 No data currently available.
   */
  int (*poll_read)(rtems_termios_device_context *context);

  /**
   * @brief Polled write in case mode is TERMIOS_POLLED or write support
   * otherwise.
   *
   * @param[in] context The Termios device context.
   * @param[in] buf The output buffer.
   * @param[in] len The output buffer length in characters.
   */
  void (*write)(
    rtems_termios_device_context *context,
    const char *buf,
    size_t len
  );

  /**
   * @brief Set attributes after a Termios settings change.
   *
   * @param[in] context The Termios device context.
   * @param[in] term The new Termios attributes.
   *
   * @retval true Successful operation.
   * @retval false Invalid attributes.
   */
  bool (*set_attributes)(
    rtems_termios_device_context *context,
    const struct termios         *term
  );

  /**
   * @brief IO control handler.
   *
   * Invoked in case the Termios layer cannot deal with the IO request.
   *
   * @param[in] context The Termios device context.
   * @param[in] request The IO control request.
   * @param[in] buffer The IO control buffer.
   */
  int (*ioctl)(
    rtems_termios_device_context *context,
    ioctl_command_t               request,
    void                         *buffer
  );

  /**
   * @brief Termios device mode.
   */
  rtems_termios_device_mode mode;
} rtems_termios_device_handler;

/**
 * @brief Termios device flow control handler.
 *
 * @see rtems_termios_device_install().
 */
typedef struct {
  /**
   * @brief Indicate to stop remote transmitter.
   *
   * @param[in] context The Termios device context.
   */
  void (*stop_remote_tx)(rtems_termios_device_context *context);

  /**
   * @brief Indicate to start remote transmitter.
   *
   * @param[in] context The Termios device context.
   */
  void (*start_remote_tx)(rtems_termios_device_context *context);
} rtems_termios_device_flow;

void rtems_termios_device_lock_acquire_default(
  rtems_termios_device_context *ctx,
  rtems_interrupt_lock_context *lock_context
);

void rtems_termios_device_lock_release_default(
  rtems_termios_device_context *ctx,
  rtems_interrupt_lock_context *lock_context
);

/**
 * @brief Initializes a device context.
 *
 * @param[in] context The Termios device context.
 * @param[in] name The name for the interrupt lock.  This name must be a
 *   string persistent throughout the life time of this lock.  The name is only
 *   used if profiling is enabled.
 */
static inline void rtems_termios_device_context_initialize(
  rtems_termios_device_context *context,
  const char                   *name
)
{
  rtems_interrupt_lock_initialize( &context->lock.interrupt, name );
  context->lock_acquire = rtems_termios_device_lock_acquire_default;
  context->lock_release = rtems_termios_device_lock_release_default;
}

/**
 * @brief Initializer for static initialization of Termios device contexts.
 *
 * @param name The name for the interrupt lock.  It must be a string.  The name
 *   is only used if profiling is enabled.
 */
#define RTEMS_TERMIOS_DEVICE_CONTEXT_INITIALIZER( name ) \
  { \
    { RTEMS_INTERRUPT_LOCK_INITIALIZER( name ) }, \
    rtems_termios_device_lock_acquire_default, \
    rtems_termios_device_lock_release_default \
  }

/**
 * @brief Acquires the device lock.
 *
 * @param[in] context The device context.
 * @param[in] lock_context The local interrupt lock context for an acquire and
 *   release pair.
 */
static inline void rtems_termios_device_lock_acquire(
  rtems_termios_device_context *context,
  rtems_interrupt_lock_context *lock_context
)
{
  ( *context->lock_acquire )( context, lock_context );
}

/**
 * @brief Releases the device lock.
 *
 * @param[in] context The device context.
 * @param[in] lock_context The local interrupt lock context for an acquire and
 *   release pair.
 */
static inline void rtems_termios_device_lock_release(
  rtems_termios_device_context *context,
  rtems_interrupt_lock_context *lock_context
)
{
  ( *context->lock_release )( context, lock_context );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _RTEMS_TERMIOSDEVICE_H */
