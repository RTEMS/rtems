/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief This header file contains interfaces to define a scheduler
 *   configuration for an application.
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

  /**
   * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
   *
   * @brief This define indicates that default attributes shall be used for a
   *   processor to scheduler assignment.
   *
   * This define may be used as an attribute parameter value in the
   * RTEMS_SCHEDULER_ASSIGN() macro.
   */
  #define RTEMS_SCHEDULER_ASSIGN_DEFAULT \
    SCHEDULER_ASSIGN_DEFAULT

  /**
   * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
   *
   * @brief This define indicates that the processor is optionally assigned to
   *   the scheduler.
   *
   * If the processor is not present during system initialization, then the
   * system initialization continues and the processor is marked as not online.
   *
   * This define may be used as an attribute parameter value in the
   * RTEMS_SCHEDULER_ASSIGN() macro.
   */
  #define RTEMS_SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL \
    SCHEDULER_ASSIGN_PROCESSOR_OPTIONAL

  /**
   * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
   *
   * @brief This define indicates that the processor to scheduler assignment is
   *   mandatory.
   *
   * If the processor is not present during system initialization, then the
   * system terminates with the fatal source of ::RTEMS_FATAL_SOURCE_SMP and
   * fatal code of ::SMP_FATAL_MANDATORY_PROCESSOR_NOT_PRESENT.
   *
   * This define may be used as an attribute parameter value in the
   * RTEMS_SCHEDULER_ASSIGN() macro.
   */
  #define RTEMS_SCHEDULER_ASSIGN_PROCESSOR_MANDATORY \
    SCHEDULER_ASSIGN_PROCESSOR_MANDATORY

  /**
   * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
   *
   * @brief Defines a processor to scheduler assignment.
   *
   * This macro may be used to define entries of the scheduler assignment
   * table, see @ref CONFIGURE_SCHEDULER_ASSIGNMENTS.
   *
   * @param index is the scheduler index.
   *
   * @param attr is the attribute set of the assignment.
   */
  #define RTEMS_SCHEDULER_ASSIGN( index, attr ) \
    { \
      ( index ) < RTEMS_ARRAY_SIZE( _Scheduler_Table ) ? \
        &_Scheduler_Table[ ( index ) ] : &RTEMS_SCHEDULER_INVALID_INDEX, \
      ( attr ) \
    }

  /**
   * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
   *
   * @brief Defines that no scheduler is assigned to the processor.
   *
   * This processor cannot be used by the application.
   *
   * This macro may be used to define entries of the scheduler assignment
   * table, see @ref CONFIGURE_SCHEDULER_ASSIGNMENTS.
   */
  #define RTEMS_SCHEDULER_ASSIGN_NO_SCHEDULER { NULL, 0 }
#endif

/*
 * This file should be only included in the context of <rtems/confdefs.h>.
 * Define the scheduler configuration macros only in case the corresponding
 * configure symbol is defined.  This is necessary to prevent invalid workspace
 * size estimates since we have to account for the per-thread scheduler
 * information.
 */

/**
 * @brief Defines a CBS Scheduler context name based on the instantiation
 *   name.
 *
 * @param name is the scheduler instantiation name.
 */
#define SCHEDULER_CBS_CONTEXT_NAME( name ) \
  SCHEDULER_CONTEXT_NAME( CBS_ ## name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines a CBS Scheduler instantiation.
 *
 * @param name is the scheduler instantiation name.
 */
#define RTEMS_SCHEDULER_CBS( name ) \
  static Scheduler_EDF_Context SCHEDULER_CBS_CONTEXT_NAME( name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines a CBS Scheduler entry for the scheduler table.
 *
 * Use this macro to define an entry for the
 * @ref CONFIGURE_SCHEDULER_TABLE_ENTRIES application configuration option.
 *
 * @param name is the scheduler instantiation name.
 *
 * @param name is the scheduler object name.
 */
#define RTEMS_SCHEDULER_TABLE_CBS( name, obj_name ) \
  { \
    &SCHEDULER_CBS_CONTEXT_NAME( name ).Base, \
    SCHEDULER_CBS_ENTRY_POINTS, \
    SCHEDULER_CBS_MAXIMUM_PRIORITY, \
    ( obj_name ) \
    SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( true ) \
  }

#ifdef CONFIGURE_SCHEDULER_CBS
  #include <rtems/score/schedulercbs.h>

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_CBS( name ) \
    RTEMS_SCHEDULER_CBS( name )

  #define RTEMS_SCHEDULER_CONTROL_CBS( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_CBS( name, obj_name )
#endif

/**
 * @brief Defines an EDF Scheduler context name based on the instantiation
 *   name.
 *
 * @param name is the scheduler instantiation name.
 */
#define SCHEDULER_EDF_CONTEXT_NAME( name ) \
  SCHEDULER_CONTEXT_NAME( EDF_ ## name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines an EDF Scheduler instantiation.
 *
 * @param name is the scheduler instantiation name.
 */
#define RTEMS_SCHEDULER_EDF( name ) \
  static Scheduler_EDF_Context SCHEDULER_EDF_CONTEXT_NAME( name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines an EDF Scheduler entry for the scheduler table.
 *
 * Use this macro to define an entry for the
 * @ref CONFIGURE_SCHEDULER_TABLE_ENTRIES application configuration option.
 *
 * @param name is the scheduler instantiation name.
 *
 * @param name is the scheduler object name.
 */
#define RTEMS_SCHEDULER_TABLE_EDF( name, obj_name ) \
  { \
    &SCHEDULER_EDF_CONTEXT_NAME( name ).Base, \
    SCHEDULER_EDF_ENTRY_POINTS, \
    SCHEDULER_EDF_MAXIMUM_PRIORITY, \
    ( obj_name ) \
    SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( true ) \
  }

#ifdef CONFIGURE_SCHEDULER_EDF
  #include <rtems/score/scheduleredf.h>

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_EDF( name ) \
    RTEMS_SCHEDULER_EDF( name )

  #define RTEMS_SCHEDULER_CONTROL_EDF( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_EDF( name, obj_name )
#endif

/**
 * @brief Defines an EDF SMP Scheduler context name based on the instantiation
 *   name.
 *
 * @param name is the scheduler instantiation name.
 */
#define SCHEDULER_EDF_SMP_CONTEXT_NAME( name ) \
  SCHEDULER_CONTEXT_NAME( EDF_SMP_ ## name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines an EDF SMP Scheduler instantiation.
 *
 * @param name is the scheduler instantiation name.
 */
#define RTEMS_SCHEDULER_EDF_SMP( name ) \
  static struct { \
    Scheduler_EDF_SMP_Context Base; \
    Scheduler_EDF_SMP_Ready_queue Ready[ CONFIGURE_MAXIMUM_PROCESSORS + 1 ]; \
  } SCHEDULER_EDF_SMP_CONTEXT_NAME( name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines an EDF SMP Scheduler entry for the scheduler table.
 *
 * Use this macro to define an entry for the
 * @ref CONFIGURE_SCHEDULER_TABLE_ENTRIES application configuration option.
 *
 * @param name is the scheduler instantiation name.
 *
 * @param name is the scheduler object name.
 */
#define RTEMS_SCHEDULER_TABLE_EDF_SMP( name, obj_name ) \
  { \
    &SCHEDULER_EDF_SMP_CONTEXT_NAME( name ).Base.Base.Base, \
    SCHEDULER_EDF_SMP_ENTRY_POINTS, \
    SCHEDULER_EDF_MAXIMUM_PRIORITY, \
    ( obj_name ) \
    SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( false ) \
  }

#ifdef CONFIGURE_SCHEDULER_EDF_SMP
  #ifndef RTEMS_SMP
    #error "CONFIGURE_SCHEDULER_EDF_SMP cannot be used if RTEMS_SMP is disabled"
  #endif

  #ifndef CONFIGURE_MAXIMUM_PROCESSORS
    #error "CONFIGURE_MAXIMUM_PROCESSORS must be defined to configure the EDF SMP Scheduler"
  #endif

  #include <rtems/score/scheduleredfsmp.h>

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_EDF_SMP( name, max_cpu_count ) \
    RTEMS_SCHEDULER_EDF_SMP( name )

  #define RTEMS_SCHEDULER_CONTROL_EDF_SMP( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_EDF_SMP( name, obj_name )
#endif

/**
 * @brief Defines a Deterministic Priority Scheduler context name based on the
 *   instantiation name.
 *
 * @param name is the scheduler instantiation name.
 */
#define SCHEDULER_PRIORITY_CONTEXT_NAME( name ) \
  SCHEDULER_CONTEXT_NAME( priority_ ## name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines a Deterministic Priority Scheduler instantiation.
 *
 * @param name is the scheduler instantiation name.
 *
 * @param prio_count is the count of supported priority levels.
 */
#define RTEMS_SCHEDULER_PRIORITY( name, prio_count ) \
  static struct { \
    Scheduler_priority_Context Base; \
    Chain_Control              Ready[ ( prio_count ) ]; \
  } SCHEDULER_PRIORITY_CONTEXT_NAME( name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines a Deterministic Priority Scheduler entry for the scheduler
 *   table.
 *
 * Use this macro to define an entry for the
 * @ref CONFIGURE_SCHEDULER_TABLE_ENTRIES application configuration option.
 *
 * @param name is the scheduler instantiation name.
 *
 * @param name is the scheduler object name.
 */
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

#ifdef CONFIGURE_SCHEDULER_PRIORITY
  #include <rtems/score/schedulerpriority.h>

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_PRIORITY( name, prio_count ) \
    RTEMS_SCHEDULER_PRIORITY( name, prio_count )

  #define RTEMS_SCHEDULER_CONTROL_PRIORITY( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_PRIORITY( name, obj_name )
#endif

/**
 * @brief Defines a Arbitrary Processor Affinity Priority SMP Scheduler context
 *   name based on the instantiation name.
 *
 * @param name is the scheduler instantiation name.
 */
#define SCHEDULER_PRIORITY_AFFINITY_SMP_CONTEXT_NAME( name ) \
  SCHEDULER_CONTEXT_NAME( priority_affinity_SMP_ ## name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines a Arbitrary Processor Affinity Priority SMP Scheduler
 *   instantiation.
 *
 * @param name is the scheduler instantiation name.
 *
 * @param prio_count is the count of supported priority levels.
 */
#define RTEMS_SCHEDULER_PRIORITY_AFFINITY_SMP( name, prio_count ) \
  static struct { \
    Scheduler_priority_SMP_Context Base; \
    Chain_Control                  Ready[ ( prio_count ) ]; \
  } SCHEDULER_PRIORITY_AFFINITY_SMP_CONTEXT_NAME( name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines a Arbitrary Processor Affinity Priority SMP Scheduler entry
 *   for the scheduler table.
 *
 * Use this macro to define an entry for the
 * @ref CONFIGURE_SCHEDULER_TABLE_ENTRIES application configuration option.
 *
 * @param name is the scheduler instantiation name.
 *
 * @param name is the scheduler object name.
 */
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

#ifdef CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP
  #ifndef RTEMS_SMP
    #error "CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP cannot be used if RTEMS_SMP is disabled"
  #endif

  #include <rtems/score/schedulerpriorityaffinitysmp.h>

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_PRIORITY_AFFINITY_SMP( name, prio_count ) \
    RTEMS_SCHEDULER_PRIORITY_AFFINITY_SMP( name, prio_count )

  #define RTEMS_SCHEDULER_CONTROL_PRIORITY_AFFINITY_SMP( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_PRIORITY_AFFINITY_SMP( name, obj_name )
#endif

/**
 * @brief Defines a Deterministic Priority SMP Scheduler context name based on
 *   the instantiation name.
 *
 * @param name is the scheduler instantiation name.
 */
#define SCHEDULER_PRIORITY_SMP_CONTEXT_NAME( name ) \
  SCHEDULER_CONTEXT_NAME( priority_SMP_ ## name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines a Deterministic Priority SMP Scheduler instantiation.
 *
 * @param name is the scheduler instantiation name.
 *
 * @param prio_count is the count of supported priority levels.
 */
#define RTEMS_SCHEDULER_PRIORITY_SMP( name, prio_count ) \
  static struct { \
    Scheduler_priority_SMP_Context Base; \
    Chain_Control                  Ready[ ( prio_count ) ]; \
  } SCHEDULER_PRIORITY_SMP_CONTEXT_NAME( name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines a Deterministic Priority SMP Scheduler entry for the
 *   scheduler table.
 *
 * Use this macro to define an entry for the
 * @ref CONFIGURE_SCHEDULER_TABLE_ENTRIES application configuration option.
 *
 * @param name is the scheduler instantiation name.
 *
 * @param name is the scheduler object name.
 */
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

#ifdef CONFIGURE_SCHEDULER_PRIORITY_SMP
  #ifndef RTEMS_SMP
    #error "CONFIGURE_SCHEDULER_PRIORITY_SMP cannot be used if RTEMS_SMP is disabled"
  #endif

  #include <rtems/score/schedulerprioritysmp.h>

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_PRIORITY_SMP( name, prio_count ) \
    RTEMS_SCHEDULER_PRIORITY_SMP( name, prio_count )

  #define RTEMS_SCHEDULER_CONTROL_PRIORITY_SMP( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_PRIORITY_SMP( name, obj_name )
#endif

/**
 * @brief Defines a Strong APA Scheduler context name based on the
 *   instantiation name.
 *
 * @param name is the scheduler instantiation name.
 */
#define SCHEDULER_STRONG_APA_CONTEXT_NAME( name ) \
  SCHEDULER_CONTEXT_NAME( strong_APA_ ## name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines a Strong APA Scheduler instantiation.
 *
 * @param name is the scheduler instantiation name.
 */
#define RTEMS_SCHEDULER_STRONG_APA( name, prio_count ) \
  static struct { \
    Scheduler_strong_APA_Context Base; \
    Scheduler_strong_APA_CPU CPU[ CONFIGURE_MAXIMUM_PROCESSORS ]; \
  } SCHEDULER_STRONG_APA_CONTEXT_NAME( name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines a Strong APA Scheduler entry for the scheduler table.
 *
 * Use this macro to define an entry for the
 * @ref CONFIGURE_SCHEDULER_TABLE_ENTRIES application configuration option.
 *
 * @param name is the scheduler instantiation name.
 *
 * @param name is the scheduler object name.
 */
#define RTEMS_SCHEDULER_TABLE_STRONG_APA( name, obj_name ) \
  { \
    &SCHEDULER_STRONG_APA_CONTEXT_NAME( name ).Base.Base.Base, \
    SCHEDULER_STRONG_APA_ENTRY_POINTS, \
    SCHEDULER_STRONG_APA_MAXIMUM_PRIORITY, \
    ( obj_name ) \
    SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( false ) \
  }

#ifdef CONFIGURE_SCHEDULER_STRONG_APA
  #ifndef RTEMS_SMP
    #error "CONFIGURE_SCHEDULER_STRONG_APA cannot be used if RTEMS_SMP is disabled"
  #endif

  #ifndef CONFIGURE_MAXIMUM_PROCESSORS
    #error "CONFIGURE_MAXIMUM_PROCESSORS must be defined to configure the Strong APA Scheduler"
  #endif

  #include <rtems/score/schedulerstrongapa.h>

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_STRONG_APA( name, prio_count ) \
    RTEMS_SCHEDULER_STRONG_APA( name, prio_count )

  #define RTEMS_SCHEDULER_CONTROL_STRONG_APA( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_STRONG_APA( name, obj_name )
#endif

/**
 * @brief Defines a Simple Scheduler context name based on the instantiation
 *   name.
 *
 * @param name is the scheduler instantiation name.
 */
#define SCHEDULER_SIMPLE_CONTEXT_NAME( name ) \
  SCHEDULER_CONTEXT_NAME( simple_ ## name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines a Simple Scheduler instantiation.
 *
 * @param name is the scheduler instantiation name.
 */
#define RTEMS_SCHEDULER_SIMPLE( name ) \
  static Scheduler_simple_Context \
    SCHEDULER_SIMPLE_CONTEXT_NAME( name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines a Simple Scheduler entry for the scheduler table.
 *
 * Use this macro to define an entry for the
 * @ref CONFIGURE_SCHEDULER_TABLE_ENTRIES application configuration option.
 *
 * @param name is the scheduler instantiation name.
 *
 * @param name is the scheduler object name.
 */
#define RTEMS_SCHEDULER_TABLE_SIMPLE( name, obj_name ) \
  { \
    &SCHEDULER_SIMPLE_CONTEXT_NAME( name ).Base, \
    SCHEDULER_SIMPLE_ENTRY_POINTS, \
    SCHEDULER_SIMPLE_MAXIMUM_PRIORITY, \
    ( obj_name ) \
    SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( true ) \
  }

#ifdef CONFIGURE_SCHEDULER_SIMPLE
  #include <rtems/score/schedulersimple.h>

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_SIMPLE( name ) \
    RTEMS_SCHEDULER_SIMPLE( name )

  #define RTEMS_SCHEDULER_CONTROL_SIMPLE( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_SIMPLE( name, obj_name )
#endif

/**
 * @brief Defines a Simple SMP Scheduler context name based on the
 *   instantiation name.
 *
 * @param name is the scheduler instantiation name.
 */
#define SCHEDULER_SIMPLE_SMP_CONTEXT_NAME( name ) \
  SCHEDULER_CONTEXT_NAME( simple_SMP_ ## name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines a Simple SMP Scheduler instantiation.
 *
 * @param name is the scheduler instantiation name.
 */
#define RTEMS_SCHEDULER_SIMPLE_SMP( name ) \
  static Scheduler_simple_SMP_Context \
    SCHEDULER_SIMPLE_SMP_CONTEXT_NAME( name )

/**
 * @ingroup RTEMSApplConfigGeneralSchedulerConfiguration
 *
 * @brief Defines a Simple SMP Scheduler entry for the scheduler table.
 *
 * Use this macro to define an entry for the
 * @ref CONFIGURE_SCHEDULER_TABLE_ENTRIES application configuration option.
 *
 * @param name is the scheduler instantiation name.
 *
 * @param name is the scheduler object name.
 */
#define RTEMS_SCHEDULER_TABLE_SIMPLE_SMP( name, obj_name ) \
  { \
    &SCHEDULER_SIMPLE_SMP_CONTEXT_NAME( name ).Base.Base, \
    SCHEDULER_SIMPLE_SMP_ENTRY_POINTS, \
    SCHEDULER_SIMPLE_SMP_MAXIMUM_PRIORITY, \
    ( obj_name ) \
    SCHEDULER_CONTROL_IS_NON_PREEMPT_MODE_SUPPORTED( false ) \
  }

#ifdef CONFIGURE_SCHEDULER_SIMPLE_SMP
  #ifndef RTEMS_SMP
    #error "CONFIGURE_SCHEDULER_SIMPLE_SMP cannot be used if RTEMS_SMP is disabled"
  #endif

  #include <rtems/score/schedulersimplesmp.h>

  /* Provided for backward compatibility */

  #define RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP( name ) \
    RTEMS_SCHEDULER_SIMPLE_SMP( name )

  #define RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP( name, obj_name ) \
    RTEMS_SCHEDULER_TABLE_SIMPLE_SMP( name, obj_name )
#endif

#endif /* _RTEMS_SAPI_SCHEDULER_H */
