/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup Shell
 *
 * @brief Access to the RTEMS Trace Buffer Generator (TBG).
 */
/*
 *  Copyright (c) 2015 Chris Johns <chrisj@rtems.org>
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

#if !defined (_RTEMS_TRACE_BUFFER_VARS_H_)
#define _RTEMS_TRACE_BUFFER_VARS_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * These functions are provided as a separated interface to the Trace Buffer
 * Generatror (TBG) data are not really designed for any real-time performance
 * type interface.
 *
 * Separating the data from the codes stops the compiler incorrectly loop
 * optimising.
 */

typedef struct
{
  uint32_t          size;
  const char* const type;
} __rtld_trace_sig_arg;

  typedef struct {
    uint32_t                    argc;
    const __rtld_trace_sig_arg* args;
} __rtld_trace_sig;

typedef __rtld_trace_sig_arg rtems_trace_sig_arg;
typedef __rtld_trace_sig rtems_trace_sig;

/*
 * Trace linker data.
 */
extern uint32_t __rtld_trace_names_size;
extern const char *const __rtld_trace_names[];
extern uint32_t __rtld_trace_enables_size;
extern const uint32_t __rtld_trace_enables[];
extern uint32_t __rtld_trace_triggers_size;
extern const uint32_t __rtld_trace_triggers[];
extern const __rtld_trace_sig __rtld_trace_signatures[];

/*
 * Trace buffer generator data.
 */
extern const bool __rtld_tbg_present;
extern const uint32_t __rtld_tbg_mode;
extern const uint32_t __rtld_tbg_buffer_size;
extern uint32_t __rtld_tbg_buffer[];
extern volatile uint32_t __rtld_tbg_buffer_in;
extern volatile bool __rtld_tbg_finished;
extern volatile bool __rtld_tbg_triggered;

/**
 * Returns the number of trace functions.
 */
uint32_t  rtems_trace_names_size (void);

/**
 * Return the name given an index. No range checking.
 */
const char* rtems_trace_names (const uint32_t index);

/**
 * Returns the number of words in the enables array.
 */
uint32_t rtems_trace_enables_size (void);

/**
 * Return the enable 32bit bitmap indexed into the enables array. No range
 * checking.
 */
uint32_t rtems_trace_enables (const uint32_t index);

/**
 * Returns the number of words in the triggers array.
 */
uint32_t rtems_trace_triggers_size (void);

/**
 * Return the trigger 32bit bitmap indexed into the triggers array. No range
 * checking.
 */
uint32_t rtems_trace_triggers (const uint32_t index);

/**
 * Return the trace function signature.
 */
const rtems_trace_sig* rtems_trace_signatures (const uint32_t index);

/**
 * Return true is the enable bit is set for the trace function index.
 */
bool rtems_trace_enable_set(const uint32_t index);

/**
 * Return true is the trigger bit is set for the trace function index.
 */
bool rtems_trace_trigger_set(const uint32_t index);

/**
 * The application has been linked with Trace Buffering generated code.
 */
bool rtems_trace_buffering_present (void);

/**
 * Return the trace buffering mode flags.
 */
uint32_t rtems_trace_buffering_mode (void);

/**
 * Return the size of the trace buffering buffer in words.
 */
uint32_t rtems_trace_buffering_buffer_size (void);

/**
 * Return the base of the trace buffering buffer.
 */
uint32_t* rtems_trace_buffering_buffer (void);

/**
 * Return the buffer level. This is only stable if tracing has finished.
 */
uint32_t rtems_trace_buffering_buffer_in (void);

/**
 * The tracing has finished.
 */
bool rtems_trace_buffering_finished (void);

/**
 * Trace has been triggered and enable trace functions are being recorded.
 */
bool rtems_trace_buffering_triggered (void);

/**
 * Start tracing by clearing the triggered flag, setting to 0 and clearing the
 * finished flag.
 */
void rtems_trace_buffering_start (void);

/**
 * Stop tracing by setting the finished flag.
 */
void rtems_trace_buffering_stop (void);

/**
 * Resume tracing by setting the finished flag.
 */
void rtems_trace_buffering_resume (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
