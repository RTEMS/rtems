
#ifndef RTEMSPPPD_H
#define RTEMSPPPD_H

/* check to see if pppd task values are set */
#ifndef RTEMS_PPPD_TASK_PRIORITY
#define RTEMS_PPPD_TASK_PRIORITY        120
#endif
#ifndef RTEMS_PPPD_TASK_STACK_SIZE
#define RTEMS_PPPD_TASK_STACK_SIZE      (10*1024)
#endif
#ifndef RTEMS_PPPD_TASK_INITIAL_MODES
#define RTEMS_PPPD_TASK_INITIAL_MODES   (RTEMS_PREEMPT | \
                                         RTEMS_NO_TIMESLICE | \
                                         RTEMS_NO_ASR | \
                                         RTEMS_INTERRUPT_LEVEL(0))
#endif

/* define hook function identifiers */
#define RTEMS_PPPD_LINKUP_HOOK      1
#define RTEMS_PPPD_LINKDOWN_HOOK    2
#define RTEMS_PPPD_IPUP_HOOK        3
#define RTEMS_PPPD_IPDOWN_HOOK      4

/* define hook function pointer prototype */
typedef void (*rtems_pppd_hookfunction)(void);


/* define pppd function prototyes */
int rtems_pppd_initialize(void);
int rtems_pppd_terminate(void);
int rtems_pppd_reset_options(void);
int rtems_pppd_set_hook(int id, rtems_pppd_hookfunction hookfp);
int rtems_pppd_set_option(const char *pOption, const char *pValue);
int rtems_pppd_connect(void);
int rtems_pppd_disconnect(void);

#endif
