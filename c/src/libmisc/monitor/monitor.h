/*
 *      @(#)monitor.h	1.2 - 95/04/24
 *      
 */

/*
 *  File:	monitor.h
 *
 *  Description:
 *    The RTEMS monitor task include file.
 *
 *
 *
 *  TODO:
 *
 */

#ifndef __MONITOR_H
#define __MONITOR_H

#ifdef __cplusplus
extern "C" {
#endif

void    rtems_monitor_init(rtems_boolean monitor_suspend);
void    rtems_monitor_wakeup(void);
void    rtems_monitor_task(rtems_task_argument monitor_suspend);
void    rtems_monitor_symbols_loadup(void);

extern rtems_unsigned32        rtems_monitor_task_id;

extern rtems_symbol_table_t *rtems_monitor_symbols;

#ifdef __cplusplus
}
#endif

#endif  /* ! __MONITOR_H */
