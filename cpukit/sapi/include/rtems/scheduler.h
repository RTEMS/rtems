/**
 * @file
 *
 * @brief Scheduler Configuration API
 */

/*
 * Copyright (c) 2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SAPI_SCHEDULER_H
#define _RTEMS_SAPI_SCHEDULER_H

#include <rtems/score/scheduler.h>

#define RTEMS_SCHEDULER_CONTEXT_NAME( name ) \
  _Configuration_Scheduler_ ## name

/*
 * This file should be only included in the context of <rtems/confdefs.h>.
 * Define the scheduler configuration macros only in case the corresponding
 * configure symbol is defined.  This is necessary to prevent invalid workspace
 * size estimates since we have to account for the per-thread scheduler
 * information.
 */

#ifdef CONFIGURE_SCHEDULER_CBS
  #include <rtems/score/schedulercbs.h>

  #define RTEMS_SCHEDULER_CONTEXT_CBS_NAME( name ) \
    RTEMS_SCHEDULER_CONTEXT_NAME( CBS_ ## name )

  #define RTEMS_SCHEDULER_CONTEXT_CBS( name ) \
    static Scheduler_EDF_Context RTEMS_SCHEDULER_CONTEXT_CBS_NAME( name )

  #define RTEMS_SCHEDULER_CONTROL_CBS( name ) \
    { \
      &RTEMS_SCHEDULER_CONTEXT_CBS_NAME( name ).Base, \
      SCHEDULER_CBS_ENTRY_POINTS \
    }
#endif

#ifdef CONFIGURE_SCHEDULER_EDF
  #include <rtems/score/scheduleredf.h>

  #define RTEMS_SCHEDULER_CONTEXT_EDF_NAME( name ) \
    RTEMS_SCHEDULER_CONTEXT_NAME( EDF_ ## name )

  #define RTEMS_SCHEDULER_CONTEXT_EDF( name ) \
    static Scheduler_EDF_Context RTEMS_SCHEDULER_CONTEXT_EDF_NAME( name )

  #define RTEMS_SCHEDULER_CONTROL_EDF( name ) \
    { \
      &RTEMS_SCHEDULER_CONTEXT_EDF_NAME( name ).Base, \
      SCHEDULER_EDF_ENTRY_POINTS \
    }
#endif

#ifdef CONFIGURE_SCHEDULER_PRIORITY
  #include <rtems/score/schedulerpriority.h>

  #define RTEMS_SCHEDULER_CONTEXT_PRIORITY_NAME( name ) \
    RTEMS_SCHEDULER_CONTEXT_NAME( priority_ ## name )

  #define RTEMS_SCHEDULER_CONTEXT_PRIORITY( name, prio_count ) \
    static struct { \
      Scheduler_priority_Context Base; \
      Chain_Control              Ready[ ( prio_count ) ]; \
    } RTEMS_SCHEDULER_CONTEXT_PRIORITY_NAME( name )

  #define RTEMS_SCHEDULER_CONTROL_PRIORITY( name ) \
    { \
      &RTEMS_SCHEDULER_CONTEXT_PRIORITY_NAME( name ).Base.Base, \
      SCHEDULER_PRIORITY_ENTRY_POINTS \
    }
#endif

#ifdef CONFIGURE_SCHEDULER_PRIORITY_AFFINITY_SMP
  #include <rtems/score/schedulerpriorityaffinitysmp.h>

  #define RTEMS_SCHEDULER_CONTEXT_PRIORITY_AFFINITY_SMP_NAME( name ) \
    RTEMS_SCHEDULER_CONTEXT_NAME( priority_affinity_SMP_ ## name )

  #define RTEMS_SCHEDULER_CONTEXT_PRIORITY_AFFINITY_SMP( name, prio_count ) \
    static struct { \
      Scheduler_priority_SMP_Context Base; \
      Chain_Control                  Ready[ ( prio_count ) ]; \
    } RTEMS_SCHEDULER_CONTEXT_PRIORITY_AFFINITY_SMP_NAME( name )

  #define RTEMS_SCHEDULER_CONTROL_PRIORITY_AFFINITY_SMP( name ) \
    { \
      &RTEMS_SCHEDULER_CONTEXT_PRIORITY_AFFINITY_SMP_NAME( name ).Base.Base, \
      SCHEDULER_PRIORITY_AFFINITY_SMP_ENTRY_POINTS \
    }
#endif

#ifdef CONFIGURE_SCHEDULER_PRIORITY_SMP
  #include <rtems/score/schedulerprioritysmp.h>

  #define RTEMS_SCHEDULER_CONTEXT_PRIORITY_SMP_NAME( name ) \
    RTEMS_SCHEDULER_CONTEXT_NAME( priority_SMP_ ## name )

  #define RTEMS_SCHEDULER_CONTEXT_PRIORITY_SMP( name, prio_count ) \
    static struct { \
      Scheduler_priority_SMP_Context Base; \
      Chain_Control                  Ready[ ( prio_count ) ]; \
    } RTEMS_SCHEDULER_CONTEXT_PRIORITY_SMP_NAME( name )

  #define RTEMS_SCHEDULER_CONTROL_PRIORITY_SMP( name ) \
    { \
      &RTEMS_SCHEDULER_CONTEXT_PRIORITY_SMP_NAME( name ).Base.Base, \
      SCHEDULER_PRIORITY_SMP_ENTRY_POINTS \
    }
#endif

#ifdef CONFIGURE_SCHEDULER_SIMPLE
  #include <rtems/score/schedulersimple.h>

  #define RTEMS_SCHEDULER_CONTEXT_SIMPLE_NAME( name ) \
    RTEMS_SCHEDULER_CONTEXT_NAME( simple_ ## name )

  #define RTEMS_SCHEDULER_CONTEXT_SIMPLE( name ) \
    static Scheduler_simple_Context \
      RTEMS_SCHEDULER_CONTEXT_SIMPLE_NAME( name )

  #define RTEMS_SCHEDULER_CONTROL_SIMPLE( name ) \
    { \
      &RTEMS_SCHEDULER_CONTEXT_SIMPLE_NAME( name ).Base, \
      SCHEDULER_SIMPLE_ENTRY_POINTS \
    }
#endif

#ifdef CONFIGURE_SCHEDULER_SIMPLE_SMP
  #include <rtems/score/schedulersimplesmp.h>

  #define RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP_NAME( name ) \
    RTEMS_SCHEDULER_CONTEXT_NAME( simple_SMP_ ## name )

  #define RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP( name ) \
    static Scheduler_simple_SMP_Context \
      RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP_NAME( name )

  #define RTEMS_SCHEDULER_CONTROL_SIMPLE_SMP( name ) \
    { \
      &RTEMS_SCHEDULER_CONTEXT_SIMPLE_SMP_NAME( name ).Base, \
      SCHEDULER_SIMPLE_SMP_ENTRY_POINTS \
    }
#endif

#endif /* _RTEMS_SAPI_SCHEDULER_H */
