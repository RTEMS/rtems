/*  directives.h
 *
 *  The following definitions are the directive numbers used
 *  in the assembly interface.
 *
 *  COPYRIGHT (c) 1989-1997
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#ifndef __RTEMS_DIRECTIVES_h
#define __RTEMS_DIRECTIVES_h

#ifdef __cplusplus
extern "C" {
#endif

#define RTEMS_INITIALIZE_EXECUTIVE                       0
#define RTEMS_INITIALIZE_EXECUTIVE_EARLY                 1
#define RTEMS_INITIALIZE_EXECUTIVE_LATE                  2
#define RTEMS_SHUTDOWN_EXECUTIVE                         3
#define RTEMS_TASKS_CREATE                               4
#define RTEMS_TASKS_NAME_TO_ID                           5
#define RTEMS_TASKS_START                                6
#define RTEMS_TASKS_RESTART                              7
#define RTEMS_TASKS_DELETE                               8
#define RTEMS_TASKS_SUSPEND                              9
#define RTEMS_TASKS_RESUME                              10
#define RTEMS_TASKS_SET_PRIORITY                        11
#define RTEMS_TASKS_MODE                                12
#define RTEMS_TASKS_GET_NOTE                            13
#define RTEMS_TASKS_SET_NOTE                            14
#define RTEMS_TASKS_WAKE_AFTER                          15
#define RTEMS_TASKS_WAKE_WHEN                           16
#define RTEMS_INTERRUPT_CATCH                           17
#define RTEMS_CLOCK_SET                                 18
#define RTEMS_CLOCK_GET                                 19
#define RTEMS_CLOCK_TICK                                20
#define RTEMS_EXTENSION_CREATE                          21
#define RTEMS_EXTENSION_NAME_TO_ID                      22
#define RTEMS_EXTENSION_DELETE                          23
#define RTEMS_TIMER_CREATE                              24
#define RTEMS_TIMER_NAME_TO_ID                          25
#define RTEMS_TIMER_CANCEL                              26
#define RTEMS_TIMER_DELETE                              27
#define RTEMS_TIMER_FIRE_AFTER                          28
#define RTEMS_TIMER_FIRE_WHEN                           29
#define RTEMS_TIMER_RESET                               30
#define RTEMS_SEMAPHORE_CREATE                          31
#define RTEMS_SEMAPHORE_NAME_TO_ID                      32
#define RTEMS_SEMAPHORE_DELETE                          33
#define RTEMS_SEMAPHORE_OBTAIN                          34
#define RTEMS_SEMAPHORE_RELEASE                         35
#define RTEMS_MESSAGE_QUEUE_CREATE                      36
#define RTEMS_MESSAGE_QUEUE_NAME_TO_ID                  37
#define RTEMS_MESSAGE_QUEUE_DELETE                      38
#define RTEMS_MESSAGE_QUEUE_SEND                        39
#define RTEMS_MESSAGE_QUEUE_URGENT                      40
#define RTEMS_MESSAGE_QUEUE_BROADCAST                   41
#define RTEMS_MESSAGE_QUEUE_RECEIVE                     42
#define RTEMS_MESSAGE_QUEUE_FLUSH                       43
#define RTEMS_EVENT_SEND                                44
#define RTEMS_EVENT_RECEIVE                             45
#define RTEMS_SIGNAL_CATCH                              46
#define RTEMS_SIGNAL_SEND                               47
#define RTEMS_PARTITION_CREATE                          48
#define RTEMS_PARTITION_NAME_TO_ID                      49
#define RTEMS_PARTITION_DELETE                          50
#define RTEMS_PARTITION_GET_BUFFER                      51
#define RTEMS_PARTITION_RETURN_BUFFER                   52
#define RTEMS_REGION_CREATE                             53
#define RTEMS_REGION_EXTEND                             54
#define RTEMS_REGION_NAME_TO_ID                         55
#define RTEMS_REGION_DELETE                             56
#define RTEMS_REGION_GET_SEGMENT_SIZE                   57
#define RTEMS_REGION_GET_SEGMENT                        58
#define RTEMS_REGION_RETURN_SEGMENT                     59
#define RTEMS_DUAL_PORTED_MEMORY_CREATE                 60
#define RTEMS_DUAL_PORTED_MEMORY_NAME_TO_ID             61
#define RTEMS_DUAL_PORTED_MEMORY_DELETE                 62
#define RTEMS_DUAL_PORTED_MEMORY_EXTERNAL_TO_INTERNAL   63
#define RTEMS_DUAL_PORTED_MEMORY_INTERNAL_TO_EXTERNAL   64
#define RTEMS_IO_INITIALIZE                             65
#define RTEMS_IO_REGISTER_NAME                          66
#define RTEMS_IO_LOOKUP_NAME                            67
#define RTEMS_IO_OPEN                                   68
#define RTEMS_IO_CLOSE                                  69
#define RTEMS_IO_READ                                   70
#define RTEMS_IO_WRITE                                  71
#define RTEMS_IO_CONTROL                                72
#define RTEMS_FATAL_ERROR_OCCURRED                      73
#define RTEMS_RATE_MONOTONIC_CREATE                     74
#define RTEMS_RATE_MONOTONIC_NAME_TO_ID                 75
#define RTEMS_RATE_MONOTONIC_DELETE                     76
#define RTEMS_RATE_MONOTONIC_CANCEL                     77
#define RTEMS_RATE_MONOTONIC_PERIOD                     78
#define RTEMS_MULTIPROCESSING_ANNOUNCE                  79
#define RTEMS_DEBUG_ENABLE                              80
#define RTEMS_DEBUG_DISABLE                             81

#define RTEMS_NUMBER_OF_ENTRY_POINTS                    82

/*
 *  The jump table of entry points into RTEMS directives.
 */

extern const void *_Entry_points[ RTEMS_NUMBER_OF_ENTRY_POINTS ];

#ifdef __cplusplus
}
#endif

#endif
/* end of directives.h */
