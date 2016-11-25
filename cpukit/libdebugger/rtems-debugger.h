/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Debugger for RTEMS.
 */

#ifndef _RTEMS_DEBUGGER_h
#define _RTEMS_DEBUGGER_h

#include <stdbool.h>

#include <rtems.h>
#include <rtems/printer.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Timeout period for an ack
 */
#define RTEMS_DEBUGGER_TIMEOUT (3)

/**
 * Start the Debugger.
 */
extern int rtems_debugger_start(const char*          remote,
                                const char*          device,
                                int                  timeout,
                                rtems_task_priority  priority,
                                const rtems_printer* printer);

/**
 * Stop the Debugger.
 */
extern int rtems_debugger_stop(void);

/**
 * Is the Debugger running?.
 */
extern bool rtems_debugger_running(void);

/**
 * Verbose control.
 */
extern void rtems_debugger_set_verbose(bool on);

/**
 * Control remote debug printing.
 */
extern int rtems_debugger_remote_debug(bool state);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
