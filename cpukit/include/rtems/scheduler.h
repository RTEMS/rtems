/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Scheduler Configuration API
 */

/*
 * Copyright (C) 2014, 2018 embedded brains GmbH & Co. KG
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

#ifndef _RTEMS_SAPI_SCHEDULER_H
#define _RTEMS_SAPI_SCHEDULER_H

#include <rtems/score/scheduler.h>

#define SCHEDULER_CONTEXT_NAME( name ) \
  _Configuration_Scheduler_ ## name

#if defined(RTEMS_SMP)
  #define SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( value ) \
    , value
#else
  #define SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( value )
#endif

#if defined(RTEMS_SMP)
  /* This object doesn't exist and indicates a configuration error */
  extern const Scheduler_Control RTEMS_SCHEDULER_INVALID_INDEX;

  #define RTEMS_SCHEDULER_ASSIGN_DEFAULT \
    SCHEDULER_ASSIGN_DEFAULT

  #define RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL \
    SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL

  #define RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY \
    SCHEDULER_ASSIGN_PROCESSOR_MANDATORY

  #define RTEMS_SCHEDULER_ASSIGN( index, attr ) \
    { \
      ( index ) < RTEMS_ARRAY_SIZE( _Scheduler_Table ) ? \
        &_Scheduler_Table[ ( index ) ] : &RTEMS_SCHEDULER_INVALID_INDEX, \
      ( attr ) \
    }

  #define RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER { NULL, 0 }
#endif

/*
 * This file should be only included in the context of <rtems/confdefs.h>.
 * Define the scheduler configuration macros only in case the corresponding
 * configure symbol is defined.  This is necessary to prevent invalid workspace
 * size estimates since we have to account for the per-thread scheduler
 * information.
 */

#ifdef CONFIGURE_SCHEDULER_CBS
  #include <rtems/score/schedulercbs.h>

  #define SCHEDULER_CBS_CONTEXT_NAME( name ) \
    SCHEDULER_CONTEXT_NAME( CBS_ ## name )

  #define RTEMS_SCHEDULER_CBS( name ) \
    static Scheduler_EDF_Context SCHEDULER_CBS_CONTEXT_NAME( name )

  #define RTEMS_SCHEDULER_TABLE_CBS( name, obj_name ) \
    { \
      &SCHEDULER_CBS_CONTEXT_NAME( name ).Base, \
      SCHEDULER_CBS_ENTRY_POINTS, \
      SCHEDULER_CBS_MAXIMUM_PRIORITY, \
      ( obj_name ) \
      SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( true ) \
    }

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_CBS( name ) \
    RTEMS_SCHEDULER_CBS( name )

  #define RTEMS_SCHEDULER_CONTROL_CBS( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_CBS( name, obj_name )
#endif

#ifdef CONFIGURE_SCHEDULER_EDF
  #include <rtems/score/scheduleredf.h>

  #define SCHEDULER_EDF_CONTEXT_NAME( name ) \
    SCHEDULER_CONTEXT_NAME( EDF_ ## name )

  #define RTEMS_SCHEDULER_EDF( name ) \
    static Scheduler_EDF_Context SCHEDULER_EDF_CONTEXT_NAME( name )

  #define RTEMS_SCHEDULER_TABLE_EDF( name, obj_name ) \
    { \
      &SCHEDULER_EDF_CONTEXT_NAME( name ).Base, \
      SCHEDULER_EDF_ENTRY_POINTS, \
      SCHEDULER_EDF_MAXIMUM_PRIORITY, \
      ( obj_name ) \
      SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( true ) \
    }

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_EDF( name ) \
    RTEMS_SCHEDULER_EDF( name )

  #define RTEMS_SCHEDULER_CONTROL_EDF( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_EDF( name, obj_name )
#endif

#ifdef CONFIGURE_SCHEDULER_EDF_SMP
  #ifndef RTEMS_SMP
    #error "CONFIGURE_SCHEDULER_EDF_SMP cannot be used if RTEMS_SMP is disabled"
  #endif

  #include <rtems/score/scheduleredfsmp.h>

  #ifndef CONFIGURE_MAXIMUM_PROCESSORS
    #error "CONFIGURE_MAXIMUM_PROCESSORS must be defined to configure the EDF SMP scheduler"
  #endif

  #define SCHEDULER_EDF_SMP_CONTEXT_NAME( name ) \
    SCHEDULER_CONTEXT_NAME( EDF_SMP_ ## name )

  #define RTEMS_SCHEDULER_EDF_SMP( name ) \
    static struct { \
      Scheduler_EDF_SMP_Context Base; \
      Scheduler_EDF_SMP_Ready_queue Ready[ CONFIGURE_MAXIMUM_PROCESSORS + 1 ]; \
    } SCHEDULER_EDF_SMP_CONTEXT_NAME( name )

  #define RTEMS_SCHEDULER_TABLE_EDF_SMP( name, obj_name ) \
    { \
      &SCHEDULER_EDF_SMP_CONTEXT_NAME( name ).Base.Base.Base, \
      SCHEDULER_EDF_SMP_ENTRY_POINTS, \
      SCHEDULER_EDF_MAXIMUM_PRIORITY, \
      ( obj_name ) \
      SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( false ) \
    }

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_EDF_SMP( name, max_cpu_count ) \
    RTEMS_SCHEDULER_EDF_SMP( name )

  #define RTEMS_SCHEDULER_CONTROL_EDF_SMP( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_EDF_SMP( name, obj_name )
#endif

#ifdef CONFIGURE_SCHEDULER_PRIORITY
  #include <rtems/score/schedulerpriority.h>

  #define SCHEDULER_PRIORITY_CONTEXT_NAME( name ) \
    SCHEDULER_CONTEXT_NAME( priority_ ## name )

  #define RTEMS_SCHEDULER_PRIORITY( name, prio_count ) \
    static struct { \
      Scheduler_priority_Context Base; \
      Chain_Control              Ready[ ( prio_count ) ]; \
    } SCHEDULER_PRIORITY_CONTEXT_NAME( name )

  #define RTEMS_SCHEDULER_TABLE_PRIORITY( name, obj_name ) \
    { \
      &SCHEDULER_PRIORITY_CONTEXT_NAME( name ).Base.Base, \
      SCHEDULER_PRIORITY_ENTRY_POINTS, \
      RTEMS_ARRAY_SIZE( \
        SCHEDULER_PRIORITY_CONTEXT_NAME( name ).Ready \
      ) - 1, \
      ( obj_name ) \
      SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( true ) \
    }

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_PRIORITY( name, prio_count ) \
    RTEMS_SCHEDULER_PRIORITY( name, prio_count )

  #define RTEMS_SCHEDULER_CONTROL_PRIORITY( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_PRIORITY( name, obj_name )
#endif

#ifdef CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP
  #ifndef RTEMS_SMP
    #error "CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP cannot be used if RTEMS_SMP is disabled"
  #endif

  #include <rtems/score/schedulerpriorityaffinitysmp.h>

  #define SCHEDULER_PRIORITY_AFFINITY_SMP_CONTEXT_NAME( name ) \
    SCHEDULER_CONTEXT_NAME( priority_affinity_SMP_ ## name )

  #define RTEMS_SCHEDULER_PRIORITY_AFFINITY_SMP( name, prio_count ) \
    static struct { \
      Scheduler_priority_SMP_Context Base; \
      Chain_Control                  Ready[ ( prio_count ) ]; \
    } SCHEDULER_PRIORITY_AFFINITY_SMP_CONTEXT_NAME( name )

  #define RTEMS_SCHEDULER_TABLE_PRIORITY_AFFINITY_SMP( name, obj_name ) \
    { \
      &SCHEDULER_PRIORITY_AFFINITY_SMP_CONTEXT_NAME( name ).Base.Base.Base, \
      SCHEDULER_PRIORITY_AFFINITY_SMP_ENTRY_POINTS, \
      RTEMS_ARRAY_SIZE( \
        SCHEDULER_PRIORITY_AFFINITY_SMP_CONTEXT_NAME( name ).Ready \
      ) - 1, \
      ( obj_name ) \
      SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( false ) \
    }

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_PRIORITY_AFFINITY_SMP( name, prio_count ) \
    RTEMS_SCHEDULER_PRIORITY_AFFINITY_SMP( name, prio_count )

  #define RTEMS_SCHEDULER_CONTROL_PRIORITY_AFFINITY_SMP( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_PRIORITY_AFFINITY_SMP( name, obj_name )
#endif

#ifdef CONFIGURE_SCHEDULER_PRIORITY_SMP
  #ifndef RTEMS_SMP
    #error "CONFIGURE_SCHEDULER_PRIORITY_SMP cannot be used if RTEMS_SMP is disabled"
  #endif

  #include <rtems/score/schedulerprioritysmp.h>

  #define SCHEDULER_PRIORITY_SMP_CONTEXT_NAME( name ) \
    SCHEDULER_CONTEXT_NAME( priority_SMP_ ## name )

  #define RTEMS_SCHEDULER_PRIORITY_SMP( name, prio_count ) \
    static struct { \
      Scheduler_priority_SMP_Context Base; \
      Chain_Control                  Ready[ ( prio_count ) ]; \
    } SCHEDULER_PRIORITY_SMP_CONTEXT_NAME( name )

  #define RTEMS_SCHEDULER_TABLE_PRIORITY_SMP( name, obj_name ) \
    { \
      &SCHEDULER_PRIORITY_SMP_CONTEXT_NAME( name ).Base.Base.Base, \
      SCHEDULER_PRIORITY_SMP_ENTRY_POINTS, \
      RTEMS_ARRAY_SIZE( \
        SCHEDULER_PRIORITY_SMP_CONTEXT_NAME( name ).Ready \
      ) - 1, \
      ( obj_name ) \
      SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( false ) \
    }

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_PRIORITY_SMP( name, prio_count ) \
    RTEMS_SCHEDULER_PRIORITY_SMP( name, prio_count )

  #define RTEMS_SCHEDULER_CONTROL_PRIORITY_SMP( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_PRIORITY_SMP( name, obj_name )
#endif

#ifdef CONFIGURE_SCHEDULER_STRONG_APA
  #ifndef RTEMS_SMP
    #error "CONFIGURE_SCHEDULER_STRONG_APA cannot be used if RTEMS_SMP is disabled"
  #endif

  #include <rtems/score/schedulerstrongapa.h>

  #ifndef CONFIGURE_MAXIMUM_PROCESSORS
    #error "CONFIGURE_MAXIMUM_PROCESSORS must be defined to configure the Strong APA scheduler"
  #endif

  #define SCHEDULER_STRONG_APA_CONTEXT_NAME( name ) \
    SCHEDULER_CONTEXT_NAME( strong_APA_ ## name )

  #define RTEMS_SCHEDULER_STRONG_APA( name, prio_count ) \
    static struct { \
      Scheduler_strong_APA_Context Base; \
      Scheduler_strong_APA_CPU CPU[ CONFIGURE_MAXIMUM_PROCESSORS ]; \
    } SCHEDULER_STRONG_APA_CONTEXT_NAME( name )

  #define RTEMS_SCHEDULER_TABLE_STRONG_APA( name, obj_name ) \
    { \
      &SCHEDULER_STRONG_APA_CONTEXT_NAME( name ).Base.Base.Base, \
      SCHEDULER_STRONG_APA_ENTRY_POINTS, \
      SCHEDULER_STRONG_APA_MAXIMUM_PRIORITY, \
      ( obj_name ) \
      SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( false ) \
    }

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_STRONG_APA( name, prio_count ) \
    RTEMS_SCHEDULER_STRONG_APA( name, prio_count )

  #define RTEMS_SCHEDULER_CONTROL_STRONG_APA( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_STRONG_APA( name, obj_name )
#endif

#ifdef CONFIGURE_SCHEDULER_SIMPLE
  #include <rtems/score/schedulersimple.h>

  #define SCHEDULER_SIMPLE_CONTEXT_NAME( name ) \
    SCHEDULER_CONTEXT_NAME( simple_ ## name )

  #define RTEMS_SCHEDULER_SIMPLE( name ) \
    static Scheduler_simple_Context \
      SCHEDULER_SIMPLE_CONTEXT_NAME( name )

  #define RTEMS_SCHEDULER_TABLE_SIMPLE( name, obj_name ) \
    { \
      &SCHEDULER_SIMPLE_CONTEXT_NAME( name ).Base, \
      SCHEDULER_SIMPLE_ENTRY_POINTS, \
      SCHEDULER_SIMPLE_MAXIMUM_PRIORITY, \
      ( obj_name ) \
      SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( true ) \
    }

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_SIMPLE( name ) \
    RTEMS_SCHEDULER_SIMPLE( name )

  #define RTEMS_SCHEDULER_CONTROL_SIMPLE( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_SIMPLE( name, obj_name )
#endif

#ifdef CONFIGURE_SCHEDULER_SIMPLE_SMP
  #ifndef RTEMS_SMP
    #error "CONFIGURE_SCHEDULER_SIMPLE_SMP cannot be used if RTEMS_SMP is disabled"
  #endif

  #include <rtems/score/schedulersimplesmp.h>

  #define SCHEDULER_SIMPLE_SMP_CONTEXT_NAME( name ) \
    SCHEDULER_CONTEXT_NAME( simple_SMP_ ## name )

  #define RTEMS_SCHEDULER_SIMPLE_SMP( name ) \
    static Scheduler_simple_SMP_Context \
      SCHEDULER_SIMPLE_SMP_CONTEXT_NAME( name )

  #define RTEMS_SCHEDULER_TABLE_SIMPLE_SMP( name, obj_name ) \
    { \
      &SCHEDULER_SIMPLE_SMP_CONTEXT_NAME( name ).Base.Base, \
      SCHEDULER_SIMPLE_SMP_ENTRY_POINTS, \
      SCHEDULER_SIMPLE_SMP_MAXIMUM_PRIORITY, \
      ( obj_name ) \
      SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( false ) \
    }

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP( name ) \
    RTEMS_SCHEDULER_SIMPLE_SMP( name )

  #define RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_SIMPLE_SMP( name, obj_name )
#endif

#endif /* _RTEMS_SAPI_SCHEDULER_H */
