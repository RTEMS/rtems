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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>

#include <rtems/monitor.h>

#define STREQ(a,b)      (strcmp(a,b) == 0)

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
 * User registered commands.
 */

rtems_monitor_command_entry_t rtems_registered_commands;

/*
 * The top-level commands
 */

rtems_monitor_command_entry_t rtems_monitor_commands[] = {
    { "config",
      "Show the system configuration.",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_CONFIG },
      &rtems_monitor_commands[1],
    },
    { "itask",
      "List init tasks for the system",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_INIT_TASK },
      &rtems_monitor_commands[2],
    }, 
   { "mpci",
      "Show the MPCI system configuration, if configured.",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_MPCI },
      &rtems_monitor_commands[3],
    },
    { "pause",
      "Monitor goes to \"sleep\" for specified ticks (default is 1). "
      "Monitor will resume at end of period or if explicitly awakened\n"
      "  pause [ticks]",
      0,
      rtems_monitor_pause_cmd,
      { 0 },
      &rtems_monitor_commands[4],
    },
    { "continue",
      "Put the monitor to sleep waiting for an explicit wakeup from the "
      "program running.\n",
      0,
      rtems_monitor_continue_cmd,
      { 0 },
      &rtems_monitor_commands[5],
    },
    { "go",
      "Alias for 'continue'",
      0,
      rtems_monitor_continue_cmd,
      { 0 },
      &rtems_monitor_commands[6],
    },
    { "node",
      "Specify default node number for commands that take id's.\n"
      "  node [ node number ]",
      0,
      rtems_monitor_node_cmd,
      { 0 },
      &rtems_monitor_commands[7],
    },
    { "symbol",
      "Display value associated with specified symbol. "
      "Defaults to displaying all known symbols.\n"
      "  symbol [ symbolname [symbolname ... ] ]",
      0,
      rtems_monitor_symbol_cmd,
      { .symbol_table = &rtems_monitor_symbols },
      &rtems_monitor_commands[8],
    },
    { "extension",
      "Display information about specified extensions. "
      "Default is to display information about all extensions on this node.\n"
      "  extension [id [id ...] ]",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_EXTENSION },
      &rtems_monitor_commands[9],
    },
    { "task",
      "Display information about the specified tasks. "
      "Default is to display information about all tasks on this node.\n"
      "  task [id [id ...] ]",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_TASK },
      &rtems_monitor_commands[10],
    },
    { "queue",
      "Display information about the specified message queues. "
      "Default is to display information about all queues on this node.\n"
      "  queue [id [id ... ] ]",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_QUEUE },
      &rtems_monitor_commands[11],
    },
    { "object",
      "Display information about specified RTEMS objects. "
      "Object id's must include 'type' information. "
      "(which may normally be defaulted)\n"
      "  object [id [id ...] ]",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_INVALID },
      &rtems_monitor_commands[12],
    },
    { "driver",
      "Display the RTEMS device driver table.\n"
      "  driver [ major [ major ... ] ]",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_DRIVER },
      &rtems_monitor_commands[13],
    },
    { "dname",
      "Displays information about named drivers.\n",
      0,
      rtems_monitor_object_cmd,
      { RTEMS_MONITOR_OBJECT_DNAME },
      &rtems_monitor_commands[14],
    },
    { "exit",
      "Invoke 'rtems_fatal_error_occurred' with 'status' "
      "(default is RTEMS_SUCCESSFUL)\n"
      "  exit [status]",
      0,
      rtems_monitor_fatal_cmd,
      { .status_code = RTEMS_SUCCESSFUL },
      &rtems_monitor_commands[15],
    },
    { "fatal",
      "'exit' with fatal error; default error is RTEMS_TASK_EXITTED\n"
      "  fatal [status]",
      0,
      rtems_monitor_fatal_cmd,
      { .status_code = RTEMS_TASK_EXITTED },		/* exit value */
      &rtems_monitor_commands[16],
    },
    { "quit",
      "Alias for 'exit'\n",
      0,
      rtems_monitor_fatal_cmd,
      { .status_code = RTEMS_SUCCESSFUL },		/* exit value */
      &rtems_monitor_commands[17],
    },
    { "help",
      "Provide information about commands. "
      "Default is show basic command summary.\n"
      "help [ command [ command ] ]",
      0,
      rtems_monitor_help_cmd,
      { .monitor_command_entry = rtems_monitor_commands },
      &rtems_monitor_commands[18],
    },
#ifdef CPU_INVOKE_DEBUGGER
    { "debugger",
      "Enter the debugger, if possible. "
      "A continue from the debugger will return to the monitor.\n",
      0,
      rtems_monitor_debugger_cmd,
      { 0 },
      &rtems_monitor_commands[19],
    },
#endif            
    { 0, 0, 0, 0, { 0 }, &rtems_registered_commands },
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
    rtems_monitor_command_arg_t* command_arg,
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
    rtems_monitor_command_arg_t* command_arg,
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
    rtems_monitor_command_arg_t* command_arg,
    boolean verbose
)
{
    if (argc == 1)
        rtems_fatal_error_occurred(command_arg->status_code);
    else
        rtems_fatal_error_occurred(strtoul(argv[1], 0, 0));
}

void
rtems_monitor_continue_cmd(
    int     argc,
    char  **argv,
    rtems_monitor_command_arg_t* command_arg,
    boolean verbose
)
{
    rtems_monitor_suspend(RTEMS_NO_TIMEOUT);
}

void
rtems_monitor_node_cmd(
    int     argc,
    char  **argv,
    rtems_monitor_command_arg_t* command_arg,
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
 * User registered commands.
 */

int
rtems_monitor_insert_cmd (
    rtems_monitor_command_entry_t *command
)
{
    rtems_monitor_command_entry_t **p =  &rtems_registered_commands.next;

    command->next = 0;

    while (*p) {
        if ( STREQ(command->command, (*p)->command) )
            return 0;
        p = & (*p)->next;
    }
    *p = command;
    return 1;
}

int
rtems_monitor_erase_cmd (
    rtems_monitor_command_entry_t *command
)
{
    rtems_monitor_command_entry_t **p = & rtems_registered_commands.next;
                                                                                
    while (*p) {
        if ( STREQ(command->command, (*p)->command) ) {
            *p = (*p)->next;
            command->next = 0;
            return 1;
        }
        p = & (*p)->next;
    }
    return 0;

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
    struct termios term;

    /*
     * Make the stdin stream characte not line based.
     */
    
    if (tcgetattr (STDIN_FILENO, &term) < 0)
    {
      printf("rtems-monitor: cannot get terminal attributes.\n");
    }
    else
    {
      /*
       * No echo, no canonical processing.
       */

      term.c_lflag &= ~(ECHO | ICANON | IEXTEN);
  
      /*
       * No sigint on BREAK, CR-to-NL off, input parity off,
       * don't strip 8th bit on input, output flow control off
       */

      term.c_lflag    &= ~(INPCK | ISTRIP | IXON);
      term.c_cc[VMIN]  = 1;
      term.c_cc[VTIME] = 0;

      if (tcsetattr (STDIN_FILENO, TCSANOW, &term) < 0)
      {
        printf("cannot set terminal attributes\n");
      }
    }
    
    if (monitor_flags & RTEMS_MONITOR_SUSPEND)
        (void) rtems_monitor_suspend(RTEMS_NO_TIMEOUT);

    for (;;)
    {
        extern rtems_tcb * _Thread_Executing;
        rtems_monitor_command_entry_t *command;

        debugee = _Thread_Executing;
        rp = &debugee->Registers;
#if (CPU_HARDWARE_FP == TRUE) || (CPU_SOFTWARE_FP == TRUE)
        fp = debugee->fp_context;  /* possibly 0 */
#else
        fp = 0;
#endif

        if (0 == rtems_monitor_command_read(command_buffer, &argc, argv))
            continue;
        if ((command = rtems_monitor_command_lookup(rtems_monitor_commands,
                                                    argc,
                                                    argv)) == 0)
        {
            /* no command */
            printf("Unrecognised command; try 'help'\n");
            continue;
        }

        command->command_function(argc, argv, &command->command_arg, verbose);

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
                               RTEMS_MINIMUM_STACK_SIZE * 2,
                               RTEMS_INTERRUPT_LEVEL(0),
                               RTEMS_DEFAULT_ATTRIBUTES,
                               &rtems_monitor_task_id);
    if (status != RTEMS_SUCCESSFUL)
    {
        rtems_error(status, "could not create monitor task");
        return;
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
        return;
    }
}
