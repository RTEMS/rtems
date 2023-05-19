/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPICPUUsageReporting
 *
 * @brief This header file provides the CPU usage reporting API.
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

/* Generated from spec:/rtems/cpuuse/if/header */

#ifndef _RTEMS_CPUUSE_H
#define _RTEMS_CPUUSE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Generated from spec:/rtems/cpuuse/if/group */

/**
 * @defgroup RTEMSAPICPUUsageReporting CPU Usage Reporting
 *
 * @ingroup RTEMSAPI
 *
 * @brief The CPU usage reporting directives can be used to report and reset
 *   the CPU usage of threads.
 */

/* Generated from spec:/rtems/cpuuse/if/printer */

/* Forward declaration */
struct rtems_printer;

/* Generated from spec:/rtems/cpuuse/if/cpu-info-report */

/**
 * @ingroup RTEMSAPICPUUsageReporting
 *
 * @brief Reports the CPU information using the printer plugin.
 *
 * @param printer is the printer plugin to output the report.
 *
 * @return Returns the number of characters printed.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within any runtime context.
 *
 * * The directive will not cause the calling task to be preempted.
 * @endparblock
 */
int rtems_cpu_info_report( const struct rtems_printer *printer );

/* Generated from spec:/rtems/cpuuse/if/report */

/**
 * @ingroup RTEMSAPICPUUsageReporting
 *
 * @brief Reports the CPU usage of each thread using the printk() printer.
 *
 * @par Notes
 * See also rtems_cpu_usage_report_with_plugin().
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cpu_usage_report( void );

/* Generated from spec:/rtems/cpuuse/if/report-with-plugin */

/**
 * @ingroup RTEMSAPICPUUsageReporting
 *
 * @brief Reports the CPU usage of each thread using the printer plugin.
 *
 * @param printer is the printer plugin to output the report.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cpu_usage_report_with_plugin( const struct rtems_printer *printer );

/* Generated from spec:/rtems/cpuuse/if/reset */

/**
 * @ingroup RTEMSAPICPUUsageReporting
 *
 * @brief Resets the CPU usage of each thread.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within device driver initialization
 *   context.
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 * @endparblock
 */
void rtems_cpu_usage_reset( void );

/* Generated from spec:/rtems/cpuuse/if/top */

/**
 * @ingroup RTEMSAPICPUUsageReporting
 *
 * @brief Starts an interactive CPU usage reporting using the printk() printer.
 *
 * @par Notes
 * See also rtems_cpu_usage_top_with_plugin().
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 *
 * * The directive sends a request to another task and waits for a response.
 *   This may cause the calling task to be blocked and unblocked.
 * @endparblock
 */
void rtems_cpu_usage_top( void );

/* Generated from spec:/rtems/cpuuse/if/top-with-plugin */

/**
 * @ingroup RTEMSAPICPUUsageReporting
 *
 * @brief Starts an interactive CPU usage reporting using the printer plugin.
 *
 * @param printer is the printer plugin to output the report.
 *
 * @par Notes
 * The directive starts a task to do the reporting.  The getchar() function is
 * used to get commands from the user.
 *
 * @par Constraints
 * @parblock
 * The following constraints apply to this directive:
 *
 * * The directive may be called from within task context.
 *
 * * The directive may obtain and release the object allocator mutex.  This may
 *   cause the calling task to be preempted.
 *
 * * The directive sends a request to another task and waits for a response.
 *   This may cause the calling task to be blocked and unblocked.
 * @endparblock
 */
void rtems_cpu_usage_top_with_plugin( const struct rtems_printer *printer );

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_CPUUSE_H */
