/*
 *	@(#)monitor.c	1.6 - 95/04/24
 *	
 */

/*
 *  mon-task.c
 *
 *  Description:
 *	RTEMS monitor task
 *	
 *	
 *	
 *  TODO:
 *	add pause command (monitor sleeps for 'n' ticks, then wakes up)
 *      
 */

#include <rtems.h>
/* #include <bsp.h> */

#include "symbols.h"
#include "monitor.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define STREQ(a,b)	(strcmp(a,b) == 0)

/* set by trap handler */
extern rtems_tcb       *debugger_interrupted_task;
extern rtems_context   *debugger_interrupted_task_context;
extern rtems_unsigned32 debugger_trap;

/* our task id needs to be public so any debugger can resume us */
rtems_unsigned32        rtems_monitor_task_id;


rtems_symbol_table_t *rtems_monitor_symbols;


#ifndef MONITOR_PROMPT
#define MONITOR_PROMPT "rtems> "
#endif

#define MONITOR_WAKEUP_EVENT   RTEMS_EVENT_0

/*
 *  Function:	rtems_monitor_init
 *
 *  Description:
 *	Create the RTEMS monitor task
 *	
 *  Parameters:
 *	'monitor_suspend' arg is passed as initial arg to monitor task
 *      If TRUE, monitor will suspend itself as it starts up.  Otherwise
 *      it will begin its command loop.
 *	
 *  Returns:
 *	
 *	
 *  Side Effects:
 *	
 *	
 *  Notes:
 *	
 *	
 *  Deficiencies/ToDo:
 *	
 *	
 */

/*
 * make_argv(cp): token-count
 *	Break up the command line in 'cp' into global argv[] and argc (return
 *	value).
 */

int
rtems_monitor_make_argv(
    char *cp,
    int  *argc_p,
    char **argv)
{
    int argc = 0;

    while ((cp = strtok(cp, " \t\n\r")))
    {
	argv[argc++] = cp;
	cp = (char *) NULL;
    }
    argv[argc] = (char *) NULL;			/* end of argv */

    return *argc_p = argc;
}

void
rtems_monitor_init(rtems_boolean monitor_suspend)
{
    rtems_status_code status;
    
    status = rtems_task_create(rtems_build_name('R', 'M', 'O', 'N'),
                               1, 0/*stack*/, RTEMS_NO_PREEMPT | RTEMS_INTERRUPT_LEVEL(0), RTEMS_DEFAULT_ATTRIBUTES, &rtems_monitor_task_id);
    if (status != RTEMS_SUCCESSFUL)
    {
        printf("could not create monitor task\n");
        goto done;
    }

    rtems_monitor_symbols_loadup();

    status = rtems_task_start(rtems_monitor_task_id, rtems_monitor_task, monitor_suspend);
    if (status != RTEMS_SUCCESSFUL)
    {
        printf("could not start monitor!\n");
        goto done;
    }

done:
}

rtems_status_code
rtems_monitor_suspend(rtems_interval timeout)
{
    rtems_event_set event_set;
    rtems_status_code status;
    
    status = rtems_event_receive(MONITOR_WAKEUP_EVENT, RTEMS_DEFAULT_OPTIONS, timeout, &event_set);
    return status;
}

void
rtems_monitor_wakeup(void)
{
    rtems_status_code status;
    
    status = rtems_event_send(rtems_monitor_task_id, MONITOR_WAKEUP_EVENT);
}


/*
 * Read and break up a monitor command
 *
 * We have to loop on the gets call, since it will return NULL under UNIX
 *  RTEMS when we get a signal (eg: SIGALRM).
 */

int
rtems_monitor_read_command(char *command,
                           int  *argc,
                           char **argv)
{
    printf("%s", MONITOR_PROMPT);  fflush(stdout);
    while (gets(command) == (char *) 0)
        ;
    return rtems_monitor_make_argv(command, argc, argv);
}

void
rtems_monitor_task(rtems_task_argument monitor_suspend)
{
    rtems_tcb *debugee = 0;
    char command[513];
    rtems_context *rp;
    rtems_context_fp *fp;
    char *cp;
    int argc;
    char *argv[64];        

    if ((rtems_boolean) monitor_suspend)
        (void) rtems_monitor_suspend(RTEMS_NO_TIMEOUT);

    for (;;)
    {
        extern rtems_tcb * _Thread_Executing;
        debugee = _Thread_Executing;
        rp = &debugee->Registers;
        fp = (rtems_context_fp *) debugee->fp_context;  /* possibly 0 */

        if (0 == rtems_monitor_read_command(command, &argc, argv))
            continue;
        
        if (STREQ(argv[0], "quit"))
            rtems_monitor_suspend(RTEMS_NO_TIMEOUT);
        else if (STREQ(argv[0], "pause"))
            rtems_monitor_suspend(1);

#ifdef CPU_INVOKE_DEBUGGER
        else if (STREQ(argv[0], "debug"))
        {
            CPU_INVOKE_DEBUGGER;
        }
#endif            
        else if (STREQ(argv[0], "symbol"))
        {
            char *symbol;
            char *value;

            if (argc != 3)
            {
                printf("usage: symbol symname symvalue\n");
                continue;
            }

            symbol = argv[1];
            value = argv[2];
            if (symbol && value)
            {
                rtems_symbol_t *sp;
                sp = rtems_symbol_create(rtems_monitor_symbols,
                                         symbol,
                                         (rtems_unsigned32) strtoul(value, 0, 16));
                if (sp)
                    printf("symbol defined is at %p\n", sp);
                else
                    printf("could not define symbol\n");
            }
            else
                printf("parsing error\n");
        }
        else
        {
            printf("Unrecognized command: '%s'\n", argv[0]);
        }
    }
}

/*
 *  Function:   rtems_monitor_symbols_loadup
 *
 *  Description:
 *      Create and load the monitor's symbol table.
 *      We are reading the output format of 'gnm' which looks like this:
 *
 *              400a7068 ? _Rate_monotonic_Information
 *              400a708c ? _Thread_Dispatch_disable_level
 *              400a7090 ? _Configuration_Table
 *
 *
 *      We ignore the type field.
 *
 *  Parameters:
 *
 *
 *  Returns:
 *
 *
 *  Side Effects:
 *      Creates and fills in 'rtems_monitor_symbols' table
 *
 *  Notes:
 *
 *
 *  Deficiencies/ToDo:
 *      Someday this should know BFD
 *              Maybe we could get objcopy to just copy the symbol areas
 *              and copy that down.
 *
 */

void
rtems_monitor_symbols_loadup(void)
{
    FILE *fp;
    char buffer[128];

    rtems_monitor_symbols = rtems_symbol_table_create(10);
    if (rtems_monitor_symbols == 0)
        return;

    fp = fdopen(8, "r");
    if (fp == 0)
        return;

    while (fgets(buffer, sizeof(buffer) - 1, fp))
    {
        char *symbol;
        char *value;
        char *ignored_type;

        value = strtok(buffer, " \t\n");
        ignored_type = strtok(0, " \t\n");
        symbol = strtok(0, " \t\n");

        if (symbol && ignored_type && value)
        {
            rtems_symbol_t *sp;
            sp = rtems_symbol_create(rtems_monitor_symbols,
                                     symbol,
                                     (rtems_unsigned32) strtoul(value, 0, 16));
            if (sp == 0)
            {
                printf("could not define symbol\n");
                goto done;
            }
        }
        else
        {
            printf("parsing error\n");
            goto done;
        }
    }

done:
}
