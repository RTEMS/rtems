/**
 * @file
 *
 * @brief User print interface to the bspIO print plug in.
 *
 * This include file defines the user interface to kernel print methods.
 */

/*
 *  Copyright (c) 2016 Chris Johns <chrisj@rtems.org>
 *  All rights reserved.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_PRINTER_H
#define _RTEMS_PRINTER_H

#include <rtems/print.h>
#include <rtems/chain.h>
#include <rtems/rtems/intr.h>
#include <rtems/rtems/tasks.h>

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSPrintSupport
 *
 * @{
 */

/**
 * Type definition for function which can be plugged in to certain reporting
 * routines to redirect the output.
 *
 * Use the RTEMS Print interface to call these functions. Do not directly use
 * them.
 *
 * If the user provides their own printer, then they may redirect those reports
 * as they see fit.
 */
typedef int (*rtems_print_printer)(void *, const char *format, va_list ap);

/**
 * Type definition for the printer structure used to access the kernel print
 * support.
 */
struct rtems_printer {
  void                *context;
  rtems_print_printer  printer;
};

/**
 * @brief check if the printer is valid.
 *
 * @param[in] printer Pointer to the printer structure.
 *
 * @return true The printer is valid else false is returned.
 */
static inline bool rtems_print_printer_valid(const rtems_printer *printer)
{
  return printer != NULL && printer->printer != NULL;
}

/**
 * @brief Initializes the printer to print nothing.
 *
 * An empty printer prints nothing. You can use this to implement an enable and
 * disable type print implementation.
 *
 * @param[in] printer Pointer to the printer structure.
 */
static inline void rtems_print_printer_empty(rtems_printer *printer)
{
  printer->context = NULL;
  printer->printer = NULL;
}

/**
 * @brief Initializes the printer to print via printk().
 *
 * @param[in] printer Pointer to the printer structure.
 */
void rtems_print_printer_printk(rtems_printer *printer);

/**
 * @brief Initializes the printer to print via printf().
 *
 * @param[in] printer Pointer to the printer structure.
 */
void rtems_print_printer_printf(rtems_printer *printer);

/**
 * @brief Initializes the printer to print via fprintf() using the specified
 * file stream.
 *
 * @param[in] printer Pointer to the printer structure.
 */
void rtems_print_printer_fprintf(rtems_printer *printer, FILE *file);

/**
 * @brief Initializes the printer to print via fprintf() using an unbuffered
 * FILE stream with output through rtems_putc().
 *
 * @param[in] printer Pointer to the printer structure.
 */
void rtems_print_printer_fprintf_putc(rtems_printer *printer);

typedef struct {
  rtems_id                     task;
  RTEMS_INTERRUPT_LOCK_MEMBER( lock )
  rtems_chain_control          free_buffers;
  rtems_chain_control          todo_buffers;
  size_t                       task_stack_size;
  rtems_task_priority          task_priority;
  int                          fd;
  void                        *buffer_table;
  size_t                       buffer_count;
  size_t                       buffer_size;
} rtems_printer_task_context;

static inline void rtems_printer_task_initialize(
  rtems_printer_task_context *context
)
{
  memset( context, 0, sizeof( *context ) );
}

static inline void rtems_printer_task_set_stack_size(
  rtems_printer_task_context *context,
  size_t                    stack_size
)
{
  context->task_stack_size = stack_size;
}

static inline void rtems_printer_task_set_priority(
  rtems_printer_task_context *context,
  rtems_task_priority       priority
)
{
  context->task_priority = priority;
}

static inline void rtems_printer_task_set_file_descriptor(
  rtems_printer_task_context *context,
  int                       fd
)
{
  context->fd = fd;
}

static inline void rtems_printer_task_set_buffer_table(
  rtems_printer_task_context *context,
  void                     *buffer_table
)
{
  context->buffer_table = buffer_table;
}

static inline void rtems_printer_task_set_buffer_count(
  rtems_printer_task_context *context,
  size_t                    buffer_count
)
{
  context->buffer_count = buffer_count;
}

static inline void rtems_printer_task_set_buffer_size(
  rtems_printer_task_context *context,
  size_t                    buffer_size
)
{
  context->buffer_size = buffer_size;
}

/**
 * @brief Creates a printer task.
 *
 * Print requests via rtems_printf() or rtems_vprintf() using a printer task
 * printer are output to a buffer and then placed on a work queue in FIFO
 * order.  The work queue is emptied by the printer task.  The printer task
 * writes the buffer content to the file descriptor specified by the context.
 * Buffers are allocated from a pool of buffers as specified by the context.
 *
 * @param[in] printer Pointer to the printer structure.
 * @param[in] context The initialized printer task context.
 *
 * @retval 0 Successful operation.
 * @retval EINVAL Invalid context parameters.
 * @retval ENOMEM Not enough resources.
 */
int rtems_print_printer_task(
  rtems_printer              *printer,
  rtems_printer_task_context *context
);

/**
 * @brief Drains the work queue of the printer task.
 *
 * Waits until all output buffers in the work queue at the time of this
 * function call are written to the file descriptor and an fsync() completed.
 *
 * The printer task must be successfully started via rtems_print_printer_task()
 * before this function can be used.  Otherwise, the behaviour is undefined.
 *
 * @param[in] context The printer task context of a successfully started
 *   printer task.
 */
void rtems_printer_task_drain(rtems_printer_task_context *context);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_PRINTER_H */
