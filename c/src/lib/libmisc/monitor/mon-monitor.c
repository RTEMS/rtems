/*
 * RTEMS monitor main body
 *	
 *  TODO:
 *      add stuff to RTEMS api
 *            rtems_get_name(id)
 *            rtems_get_type(id)
 *            rtems_build_id(node, type, num)
 *      Add a command to dump out info about an arbitrary id when
 *         types are added to id's
 *         rtems> id idnum
 *                idnum: node n, object: whatever, id: whatever
 *      allow id's to be specified as n:t:id, where 'n:t' is optional
 *      should have a separate monitor FILE stream (ala the debugger)
 *      remote request/response stuff should be cleaned up
 *         maybe we can use real rpc??
 *      'info' command to print out:
 *           interrupt stack location, direction and size
 *           floating point config stuff
 *           interrupt config stuff
 *
 *  $Id$
 */

#include <rtems.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <rtems/monitor.h>

/* set by trap handler */
extern rtems_tcb       *debugger_interrupted_task;
extern rtems_context   *debugger_interrupted_task_context;
extern rtems_unsigned32 debugger_trap;

/*
 * Various id's for the monitor
 * They need to be public variables for access by other agencies
 * such as debugger and remote servers'
 */

rtems_id  rtems_monitor_task_id;

unsigned32 rtems_monitor_node;		/* our node number */
unsigned32 rtems_monitor_default_node;  /* current default for commands */

/*
 * The rtems symbol table
 */

rtems_symbol_table_t *rtems_monitor_symbols;

/*
 * The top-level commands
 */

rtems_monitor_command_entry_t rtems_monitor_commands[] = {
    { "--usage--",
      "\n"
      "RTEMS monitor\n"
      "\n"
      "Commands (may be abbreviated)\n"
      "\n"
      "  help      -- get this message or command specific help\n"
      "  pause     -- pause monitor for a specified number of ticks\n"
      "  exit      -- invoke a fatal RTEMS error\n"
      "  symbol    -- show entries from symbol table\n"
      "  continue  -- put monitor to sleep waiting for explicit wakeup\n"
      "  config    -- show system configuration\n"
      "  itask     -- list init tasks\n"
      "  mpci      -- list mpci config\n"
      "  task      -- show task information\n"
      "  queue     -- show message queue information\n"
      "  extension -- user extensions\n"
      "  driver    -- show information about named drivers\n"
      "  dname     -- show information about named drivers\n"
      "  object    -- generic object information\n"
      "  node      -- specify default node for commands that take id's\n"
#ifdef CPU_INVOKE_DEBUGGER
      "  debugger  -- invoke system debugger\n"
#endif
      ,
      0,
      0,
      (unsigned32) rtems_monitor_commands,
    },
    { "config",
      "config\n"
      "  Show the system configuration.\n",
      0,
      rtems_monitor_object_cmd,
      RTEMS_MONITOR_OBJECT_CONFIG,
    },
    { "itask",
      "itask\n"
      "  List init tasks for the system\n",
      0,
      rtems_monitor_object_cmd,
      RTEMS_MONITOR_OBJECT_INIT_TASK,
    }, 
   { "mpci",
      "mpci\n"
      "  Show the MPCI system configuration, if configured.\n",
      0,
      rtems_monitor_object_cmd,
      RTEMS_MONITOR_OBJECT_MPCI,
    },
    { "pause",
      "pause [ticks]\n"
      "  monitor goes to \"sleep\" for specified ticks (default is 1)\n"
      "  monitor will resume at end of period or if explicitly awakened\n",
      0,
      rtems_monitor_pause_cmd,
      0,
    },
    { "continue",
      "continue\n"
      "  put the monitor to sleep waiting for an explicit wakeup from the\n"
      "  program running.\n",
      0,
      rtems_monitor_continue_cmd,
      0,
    },
    { "go",
      "go\n"
      "  Alias for 'continue'\n",
      0,
      rtems_monitor_continue_cmd,
      0,
    },
    { "node",
      "node [ node number ]\n"
      "  Specify default node number for commands that take id's\n",
      0,
      rtems_monitor_node_cmd,
      0,
    },
    { "symbol",
      "symbol [ symbolname [symbolname ... ] ]\n"
      "  display value associated with specified symbol.\n"
      "  Defaults to displaying all known symbols.\n",
      0,
      rtems_monitor_symbol_cmd,
      (unsigned32) &rtems_monitor_symbols,
    },
    { "extension",
      "extension [id [id ...] ]\n"
      "  display information about specified extensions.\n"
      "  Default is to display information about all extensions on this node\n",
      0,
      rtems_monitor_object_cmd,
      RTEMS_MONITOR_OBJECT_EXTENSION,
    },
    { "task",
      "task [id [id ...] ]\n"
      "  display information about the specified tasks.\n"
      "  Default is to display information about all tasks on this node\n",
      0,
      rtems_monitor_object_cmd,
      RTEMS_MONITOR_OBJECT_TASK,
    },
    { "queue",
      "queue [id [id ... ] ]\n"
      "  display information about the specified message queues\n"
      "  Default is to display information about all queues on this node\n",
      0,
      rtems_monitor_object_cmd,
      RTEMS_MONITOR_OBJECT_QUEUE,
    },
    { "object",
      "object [id [id ...] ]\n"
      "  display information about specified RTEMS objects.\n"
      "  Object id's must include 'type' information.\n"
      "  (which may normally be defaulted)\n",
      0,
      rtems_monitor_object_cmd,
      RTEMS_MONITOR_OBJECT_INVALID,
    },
    { "driver",
      "driver [ major [ major ... ] ]\n"
      "  Display the RTEMS device driver table.\n",
      0,
      rtems_monitor_object_cmd,
      RTEMS_MONITOR_OBJECT_DRIVER,
    },
    { "dname",
      "dname\n"
      "  Displays information about named drivers.\n",
      0,
      rtems_monitor_object_cmd,
      RTEMS_MONITOR_OBJECT_DNAME,
    },
    { "exit",
      "exit [status]\n"
      "  Invoke 'rtems_fatal_error_occurred' with 'status'\n"
      "  (default is RTEMS_SUCCESSFUL)\n",
      0,
      rtems_monitor_fatal_cmd,
      RTEMS_SUCCESSFUL,
    },
    { "fatal",
      "fatal [status]\n"
      "  'exit' with fatal error; default error is RTEMS_TASK_EXITTED\n",
      0,
      rtems_monitor_fatal_cmd,
      RTEMS_TASK_EXITTED,			/* exit value */
    },
    { "quit",
      "quit [status]\n"
      "  Alias for 'exit'\n",
      0,
      rtems_monitor_fatal_cmd,
      RTEMS_SUCCESSFUL,				/* exit value */
    },
    { "help",
      "help [ command [ command ] ]\n"
      "  provide information about commands\n"
      "  Default is show basic command summary.\n",
      0,
      rtems_monitor_help_cmd,
      (unsigned32) rtems_monitor_commands,
    },
#ifdef CPU_INVOKE_DEBUGGER
    { "debugger",
      "debugger\n"
      "  Enter the debugger, if possible.\n"
      "  A continue from the debugger will return to the monitor.\n",
      0,
      rtems_monitor_debugger_cmd,
      0,
    },
#endif            
    { 0, 0, 0, 0, 0 },
};


rtems_status_code
rtems_monitor_suspend(rtems_interval timeout)
{
    rtems_event_set event_set;
    rtems_status_code status;
    
    status = rtems_event_receive(MONITOR_WAKEUP_EVENT,
                                 RTEMS_DEFAULT_OPTIONS,
                                 timeout,
                                 &event_set);
    return status;
}

void
rtems_monitor_wakeup(void)
{
    rtems_status_code status;
    
    status = rtems_event_send(rtems_monitor_task_id, MONITOR_WAKEUP_EVENT);
}

void
rtems_monitor_debugger_cmd(
    int        argc,
    char     **argv,
    unsigned32 command_arg,
    boolean    verbose
)
{
#ifdef CPU_INVOKE_DEBUGGER
    CPU_INVOKE_DEBUGGER;
#endif
}

void
rtems_monitor_pause_cmd(
    int        argc,
    char     **argv,
    unsigned32 command_arg,
    boolean    verbose
)
{
    if (argc == 1)
        rtems_monitor_suspend(1);
    else
        rtems_monitor_suspend(strtoul(argv[1], 0, 0));
}

void
rtems_monitor_fatal_cmd(
    int     argc,
    char  **argv,
    unsigned32 command_arg,
    boolean verbose
)
{
    if (argc == 1)
        rtems_fatal_error_occurred(command_arg);
    else
        rtems_fatal_error_occurred(strtoul(argv[1], 0, 0));
}

void
rtems_monitor_continue_cmd(
    int     argc,
    char  **argv,
    unsigned32 command_arg,
    boolean verbose
)
{
    rtems_monitor_suspend(RTEMS_NO_TIMEOUT);
}

void
rtems_monitor_node_cmd(
    int     argc,
    char  **argv,
    unsigned32 command_arg,
    boolean verbose
)
{
    unsigned32 new_node = rtems_monitor_default_node;
    
    switch (argc)
    {
        case 1: 		/* no node, just set back to ours */
            new_node = rtems_monitor_node;
            break;

        case 2:
            new_node = strtoul(argv[1], 0, 0);
            break;

        default:
            printf("invalid syntax, try 'help node'\n");
            break;
    }

    if ((new_node >= 1) &&
        _Configuration_MP_table &&
        (new_node <= _Configuration_MP_table->maximum_nodes))
            rtems_monitor_default_node = new_node;
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
 *      We ignore the type field.
 *
 *  Side Effects:
 *      Creates and fills in 'rtems_monitor_symbols' table
 *
 *  TODO
 *      there should be a BSP #define or something like that
 *         to do this;  Assuming stdio is crazy.
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

    if (rtems_monitor_symbols)
        rtems_symbol_table_destroy(rtems_monitor_symbols);
    
    rtems_monitor_symbols = rtems_symbol_table_create(10);
    if (rtems_monitor_symbols == 0)
        return;

    fp = fopen("symbols", "r");
    
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
                printf("could not define symbol '%s'\n", symbol);
                goto done;
            }
        }
        else
        {
            printf("parsing error on '%s'\n", buffer);
            goto done;
        }
    }

done:
    return;
}


/*
 * Main monitor command loop
 */

void
rtems_monitor_task(
    rtems_task_argument monitor_flags
)
{
    rtems_tcb *debugee = 0;
    rtems_context *rp;
    rtems_context_fp *fp;
    char command_buffer[513];
    int argc;
    char *argv[64];        
    boolean verbose = FALSE;

    if (monitor_flags & RTEMS_MONITOR_SUSPEND)
        (void) rtems_monitor_suspend(RTEMS_NO_TIMEOUT);

    for (;;)
    {
        extern rtems_tcb * _Thread_Executing;
        rtems_monitor_command_entry_t *command;

        debugee = _Thread_Executing;
        rp = &debugee->Registers;
        fp = (rtems_context_fp *) debugee->fp_context;  /* possibly 0 */

        if (0 == rtems_monitor_command_read(command_buffer, &argc, argv))
            continue;
        if ((command = rtems_monitor_command_lookup(rtems_monitor_commands,
                                                    argc,
                                                    argv)) == 0)
            continue;

        command->command_function(argc, argv, command->command_arg, verbose);

        fflush(stdout);
    }
}


void
rtems_monitor_kill(void)
{
    if (rtems_monitor_task_id)
        rtems_task_delete(rtems_monitor_task_id);
    rtems_monitor_task_id = 0;
    
    rtems_monitor_server_kill();
}

void
rtems_monitor_init(
    unsigned32 monitor_flags
)
{
    rtems_status_code status;
    
    rtems_monitor_kill();

    status = rtems_task_create(RTEMS_MONITOR_NAME,
                               1,
                               0 /* default stack */,
                               RTEMS_INTERRUPT_LEVEL(0),
                               RTEMS_DEFAULT_ATTRIBUTES,
                               &rtems_monitor_task_id);
    if (status != RTEMS_SUCCESSFUL)
    {
        rtems_error(status, "could not create monitor task");
        goto done;
    }

    rtems_monitor_node = rtems_get_node(rtems_monitor_task_id);
    rtems_monitor_default_node = rtems_monitor_node;

    rtems_monitor_symbols_loadup();

    if (monitor_flags & RTEMS_MONITOR_GLOBAL)
        rtems_monitor_server_init(monitor_flags);

    /*
     * Start the monitor task itself
     */
    
    status = rtems_task_start(rtems_monitor_task_id,
                              rtems_monitor_task,
                              monitor_flags);
    if (status != RTEMS_SUCCESSFUL)
    {
        rtems_error(status, "could not start monitor");
        goto done;
    }

done:
}
