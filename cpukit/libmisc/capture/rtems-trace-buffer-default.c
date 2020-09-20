/*
 *  Copyright (c) 2015 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/trace/rtems-trace-buffer-vars.h>

/**
 * External Trace Linker and TBG data. We provide weak versions to allow us to
 * link and be present in an application that has not been trace linked.
 */

/*
 * Trace linker data.
 */
uint32_t __rtld_trace_names_size __attribute__ ((weak));
const char *const __rtld_trace_names[1] __attribute__ ((weak));
uint32_t __rtld_trace_enables_size __attribute__ ((weak));
const uint32_t __rtld_trace_enables[1] __attribute__ ((weak));
uint32_t __rtld_trace_triggers_size __attribute__ ((weak));
const uint32_t __rtld_trace_triggers[1] __attribute__ ((weak));
const __rtld_trace_sig __rtld_trace_signatures[1] __attribute__ ((weak));

/*
 * Trace buffer generator data.
 */
const bool __rtld_tbg_present __attribute__ ((weak));
const uint32_t __rtld_tbg_mode __attribute__ ((weak));
const uint32_t __rtld_tbg_buffer_size __attribute__ ((weak));
uint32_t __rtld_tbg_buffer[1] __attribute__ ((weak));
volatile uint32_t __rtld_tbg_buffer_in __attribute__ ((weak));
volatile bool __rtld_tbg_finished __attribute__ ((weak));
volatile bool __rtld_tbg_triggered __attribute__ ((weak));
