/*
 * Copyright (c) 2016-2019 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
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

#ifndef _RTEMS_DEBUGGER_SERVER_h
#define _RTEMS_DEBUGGER_SERVER_h

#include <rtems.h>
#include <rtems/printer.h>

#include <rtems/rtems-debugger.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Debugger NUMOF macro.
 */
#define RTEMS_DEBUGGER_NUMOF(_d) (sizeof(_d) / sizeof(_d[0]))

/**
 * Machine specific size. Here for 64bit support.
 */
#define DB_UINT uint32_t

/*
 * Debugger signals.
 */
#define RTEMS_DEBUGGER_SIGNAL_HUP   1     /* Hangup */
#define RTEMS_DEBUGGER_SIGNAL_INT   2     /* Interrupt */
#define RTEMS_DEBUGGER_SIGNAL_QUIT  3     /* Quit */
#define RTEMS_DEBUGGER_SIGNAL_ILL   4     /* Illegal instruction */
#define RTEMS_DEBUGGER_SIGNAL_TRAP  5     /* Trace/breakpoint trap */
#define RTEMS_DEBUGGER_SIGNAL_ABRT  6     /* Aborted */
#define RTEMS_DEBUGGER_SIGNAL_EMT   7     /* Emulation trap */
#define RTEMS_DEBUGGER_SIGNAL_FPE   8     /* Arithmetic exception */
#define RTEMS_DEBUGGER_SIGNAL_KILL  9     /* Killed */
#define RTEMS_DEBUGGER_SIGNAL_BUS   10    /* Bus error */
#define RTEMS_DEBUGGER_SIGNAL_SEGV  11    /* Segmentation fault */
#define RTEMS_DEBUGGER_SIGNAL_SYS   12    /* Bad system call */
#define RTEMS_DEBUGGER_SIGNAL_PIPE  13    /* Broken pipe */
#define RTEMS_DEBUGGER_SIGNAL_ALRM  14    /* Alarm clock */
#define RTEMS_DEBUGGER_SIGNAL_TERM  15    /* Terminated */
#define RTEMS_DEBUGGER_SIGNAL_URG   16    /* Urgent I/O condition */
#define RTEMS_DEBUGGER_SIGNAL_STOP  17    /* Stopped (signal) */
#define RTEMS_DEBUGGER_SIGNAL_TSTP  18    /* Stopped (user) */
#define RTEMS_DEBUGGER_SIGNAL_CONT  19    /* Continued */

/**
 * Timeout period for the Debugger task to stop in usecs.
 */
#define RTEMS_DEBUGGER_TIMEOUT_STOP (5 * 1000 * 1000)

/**
 * Debugger poll wait timeout in usecs.
 */
#define RTEMS_DEBUGGER_POLL_WAIT (10000)

/**
 * Debugger task stack size.
 */
#define RTEMS_DEBUGGER_STACKSIZE (16 * 1024)

/**
 * Debugger output buffer size.
 */
#define RTEMS_DEBUGGER_BUFFER_SIZE (4 * 1024)

/**
 * Debugger flags.
 */
#define RTEMS_DEBUGGER_FLAG_VERBOSE      (1 << 0)
#define RTEMS_DEBUGGER_FLAG_RESET        (1 << 1)
#define RTEMS_DEBUGGER_FLAG_NON_STOP     (1 << 2)
#define RTEMS_DEBUGGER_FLAG_VCONT        (1 << 3)
#define RTEMS_DEBUGGER_FLAG_MULTIPROCESS (1 << 4)
#define RTEMS_DEBUGGER_FLAG_VERBOSE_LOCK (1 << 5)
#define RTEMS_DEBUGGER_FLAG_VERBOSE_CMDS (1 << 6)

/**
 * Forward decl for the threads and targets.
 */
typedef struct rtems_debugger_remote  rtems_debugger_remote;
typedef struct rtems_debugger_threads rtems_debugger_threads;
typedef struct rtems_debugger_target  rtems_debugger_target;

/**
 * Local types for the RTEMS-X interface.
 */
typedef struct _Condition_Control       rtems_rx_cond;
typedef struct _Mutex_recursive_Control rtems_rx_mutex;

/**
 * Debugger data.
 */
typedef struct
{
  int                     port;
  int                     timeout;
  rtems_task_priority     priority;
  rtems_rx_mutex          lock;
  rtems_debugger_remote*  remote;
  rtems_id                server_task;
  rtems_rx_cond           server_cond;
  volatile bool           server_running;
  volatile bool           server_finished;
  rtems_id                events_task;
  volatile bool           events_running;
  volatile bool           events_finished;
  rtems_printer           printer;
  uint32_t                flags;
  pid_t                   pid;
  bool                    remote_debug;
  bool                    ack_pending;
  size_t                  output_level;
  uint8_t                 input[RTEMS_DEBUGGER_BUFFER_SIZE];
  uint8_t                 output[RTEMS_DEBUGGER_BUFFER_SIZE];
  rtems_debugger_threads* threads;
  rtems_chain_control     exception_threads;
  int                     signal;
  rtems_debugger_target*  target;
} rtems_debugger_server;

/**
 * Debugger global variable.
 */
extern rtems_debugger_server* rtems_debugger;

/**
 * Debug server printer.
 */
extern int rtems_debugger_printf(const char* format, ...) RTEMS_PRINTFLIKE(1, 2);
extern int rtems_debugger_clean_printf(const char* format, ...) RTEMS_PRINTFLIKE(1, 2);
extern void rtems_debugger_printk_lock(rtems_interrupt_lock_context* lock_context);
extern void rtems_debugger_printk_unlock(rtems_interrupt_lock_context* lock_context);

/**
 * Lock the Debugger.
 */
extern void rtems_debugger_lock(void);

/**
 * Unlock the Debugger.
 */
extern void rtems_debugger_unlock(void);

/**
 * Is the server still running?
 */
extern bool rtems_debugger_server_running(void);

/**
 * Signel a server crash.
 */
extern void rtems_debugger_server_crash(void);

/**
 * Get the remote handle from the debugger.
 */
extern rtems_debugger_remote* rtems_debugger_remote_handle(void);

/**
 * Is the debugger connected?
 */
extern bool rtems_debugger_connected(void);

/**
 * Is the debugger events thread runnins?
 */
extern bool rtems_debugger_server_events_running(void);

/**
 * Signal events thread in the debug server to run.
 */
extern void rtems_debugger_server_events_signal(void);

/**
 * Check if verbose is on.
 */
extern bool rtems_debugger_verbose(void);

/**
 * Check a flag.
 */
static inline bool rtems_debugger_server_flag(uint32_t mask)
{
  return (rtems_debugger->flags & mask) != 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif
