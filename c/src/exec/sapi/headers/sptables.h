/*  sptables.h
 *
 *  This include file contains the executive's pre-initialized tables
 *  used when in a single processor configuration.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __RTEMS_SPTABLES_h
#define __RTEMS_SPTABLES_h

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/config.h>

#include <rtems/debug.h>
#include <rtems/fatal.h>
#include <rtems/init.h>
#include <rtems/io.h>
#include <rtems/score/sysstate.h>

#include <rtems/rtems/intr.h>
#include <rtems/rtems/clock.h>
#include <rtems/rtems/tasks.h>
#include <rtems/rtems/dpmem.h>
#include <rtems/rtems/event.h>
#include <rtems/rtems/message.h>
#include <rtems/rtems/mp.h>
#include <rtems/rtems/part.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/rtems/region.h>
#include <rtems/rtems/sem.h>
#include <rtems/rtems/signal.h>
#include <rtems/rtems/timer.h>

/*
 *  This is the default Multiprocessing Configuration Table.
 *  It is used in single processor configurations.
 */

const rtems_multiprocessing_table
       _Initialization_Default_multiprocessing_table = {
  1,                      /* local node number */
  1,                      /* maximum number nodes in system */
  0,                      /* maximum number global objects */
  0,                      /* maximum number proxies */
  NULL,                   /* pointer to MPCI address table */
};

/*
 *  This is the version string.
 */

#define RTEMS_VERSION "rtems-981006"

const char _RTEMS_version[] =
  "RTEMS RELEASE " RTEMS_VERSION
     "(" CPU_NAME "/" CPU_MODEL_NAME "/REPLACE_THIS_WITH_THE_BSP)";


/*
 *  This table is used by the single entry point code.
 */

const void * _Entry_points[ RTEMS_NUMBER_OF_ENTRY_POINTS ] = {
  (void *) rtems_initialize_executive,                      /*  0 */
  (void *) rtems_initialize_executive_early,                /*  1 */
  (void *) rtems_initialize_executive_late,                 /*  2 */
  (void *) rtems_shutdown_executive,                        /*  3 */
  (void *) rtems_task_create,                               /*  4 */
  (void *) rtems_task_ident,                                /*  5 */
  (void *) rtems_task_start,                                /*  6 */
  (void *) rtems_task_restart,                              /*  7 */
  (void *) rtems_task_delete,                               /*  8 */
  (void *) rtems_task_suspend,                              /*  9 */
  (void *) rtems_task_resume,                               /* 10 */
  (void *) rtems_task_set_priority,                         /* 11 */
  (void *) rtems_task_mode,                                 /* 12 */
  (void *) rtems_task_get_note,                             /* 13 */
  (void *) rtems_task_set_note,                             /* 14 */
  (void *) rtems_task_wake_after,                           /* 15 */
  (void *) rtems_task_wake_when,                            /* 16 */
  (void *) rtems_interrupt_catch,                           /* 17 */
  (void *) rtems_clock_set,                                 /* 18 */
  (void *) rtems_clock_get,                                 /* 19 */
  (void *) rtems_clock_tick,                                /* 20 */
  (void *) rtems_extension_create,                          /* 21 */
  (void *) rtems_extension_ident,                           /* 22 */
  (void *) rtems_extension_delete,                          /* 23 */
  (void *) rtems_timer_create,                              /* 24 */
  (void *) rtems_timer_ident,                               /* 25 */
  (void *) rtems_timer_cancel,                              /* 26 */
  (void *) rtems_timer_delete,                              /* 27 */
  (void *) rtems_timer_fire_after,                          /* 28 */
  (void *) rtems_timer_fire_when,                           /* 29 */
  (void *) rtems_timer_reset,                               /* 30 */
  (void *) rtems_semaphore_create,                          /* 31 */
  (void *) rtems_semaphore_ident,                           /* 32 */
  (void *) rtems_semaphore_delete,                          /* 33 */
  (void *) rtems_semaphore_obtain,                          /* 34 */
  (void *) rtems_semaphore_release,                         /* 35 */
  (void *) rtems_message_queue_create,                      /* 36 */
  (void *) rtems_message_queue_ident,                       /* 37 */
  (void *) rtems_message_queue_delete,                      /* 38 */
  (void *) rtems_message_queue_send,                        /* 39 */
  (void *) rtems_message_queue_urgent,                      /* 40 */
  (void *) rtems_message_queue_broadcast,                   /* 41 */
  (void *) rtems_message_queue_receive,                     /* 42 */
  (void *) rtems_message_queue_flush,                       /* 43 */
  (void *) rtems_event_send,                                /* 44 */
  (void *) rtems_event_receive,                             /* 45 */
  (void *) rtems_signal_catch,                              /* 46 */
  (void *) rtems_signal_send,                               /* 47 */
  (void *) rtems_partition_create,                          /* 48 */
  (void *) rtems_partition_ident,                           /* 49 */
  (void *) rtems_partition_delete,                          /* 50 */
  (void *) rtems_partition_get_buffer,                      /* 51 */
  (void *) rtems_partition_return_buffer,                   /* 52 */
  (void *) rtems_region_create,                             /* 53 */
  (void *) rtems_region_extend,                             /* 54 */
  (void *) rtems_region_ident,                              /* 55 */
  (void *) rtems_region_delete,                             /* 56 */
  (void *) rtems_region_get_segment,                        /* 57 */
  (void *) rtems_region_get_segment_size,                   /* 58 */
  (void *) rtems_region_return_segment,                     /* 59 */
  (void *) rtems_port_create,                               /* 60 */
  (void *) rtems_port_ident,                                /* 61 */
  (void *) rtems_port_delete,                               /* 62 */
  (void *) rtems_port_external_to_internal,                 /* 63 */
  (void *) rtems_port_internal_to_external,                 /* 64 */
  (void *) rtems_io_initialize,                             /* 65 */
  (void *) rtems_io_register_name,                          /* 66 */
  (void *) rtems_io_lookup_name,                            /* 67 */
  (void *) rtems_io_open,                                   /* 68 */
  (void *) rtems_io_close,                                  /* 69 */
  (void *) rtems_io_read,                                   /* 70 */
  (void *) rtems_io_write,                                  /* 71 */
  (void *) rtems_io_control,                                /* 72 */
  (void *) rtems_fatal_error_occurred,                      /* 73 */
  (void *) rtems_rate_monotonic_create,                     /* 74 */
  (void *) rtems_rate_monotonic_ident,                      /* 75 */
  (void *) rtems_rate_monotonic_delete,                     /* 76 */
  (void *) rtems_rate_monotonic_cancel,                     /* 77 */
  (void *) rtems_rate_monotonic_period,                     /* 78 */
  (void *) rtems_multiprocessing_announce,                  /* 79 */
  (void *) rtems_debug_enable,                              /* 80 */
  (void *) rtems_debug_disable                              /* 81 */
};

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
