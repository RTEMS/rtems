/*
 * Command parsing routines for RTEMS monitor
 *
 * TODO:
 *
 *  $Id$
 */

#include <rtems.h>

#include "monitor.h"

#include <stdio.h>
#include <string.h>

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


/*
 * Read and break up a monitor command
 *
 * We have to loop on the gets call, since it will return NULL under UNIX
 *  RTEMS when we get a signal (eg: SIGALRM).
 */

int
rtems_monitor_command_read(char *command,
                           int  *argc,
                           char **argv)
{
    extern rtems_configuration_table  BSP_Configuration;
    static char monitor_prompt[32];
    
    /*
     * put node number in the prompt if we are multiprocessing
     */

    if (BSP_Configuration.User_multiprocessing_table == 0)
        sprintf(monitor_prompt, "%s", MONITOR_PROMPT);
    else if (rtems_monitor_default_node != rtems_monitor_node)
        sprintf(monitor_prompt, "%d-%s-%d", rtems_monitor_node, MONITOR_PROMPT, rtems_monitor_default_node);
    else
        sprintf(monitor_prompt, "%d-%s", rtems_monitor_node, MONITOR_PROMPT);

#ifdef RTEMS_UNIX
    /* RTEMS on unix gets so many interrupt system calls this is hosed */
    printf("%s> ", monitor_prompt);
    fflush(stdout);
    while (gets(command) == (char *) 0)
        ;
#else
    do
    {
        printf("%s> ", monitor_prompt);
        fflush(stdout);
    } while (gets(command) == (char *) 0);
#endif

    return rtems_monitor_make_argv(command, argc, argv);
}

/*
 * Look up a command in a command table
 *
 */

rtems_monitor_command_entry_t *
rtems_monitor_command_lookup(
    rtems_monitor_command_entry_t *table,
    int                            argc,
    char                          **argv
)
{
    rtems_monitor_command_entry_t *p;
    rtems_monitor_command_entry_t *abbreviated_match = 0;
    int abbreviated_matches = 0;
    char *command;
    int command_length;

    command = argv[0];

    if ((table == 0) || (command == 0))
        goto failed;
    
    command_length = strlen(command);

    for (p = table; p->command; p++)
        if (STREQ(command, p->command))    /* exact match */
            goto done;
        else if (STRNEQ(command, p->command, command_length))
        {
            abbreviated_matches++;
            abbreviated_match = p;
        }

    /* no perfect match; is there a non-ambigous abbreviated match? */
    if ( ! abbreviated_match)
    {
        printf("Unrecognized command '%s'; try 'help'\n", command);
        goto failed;
    }    

    if (abbreviated_matches > 1)
    {
        printf("Command '%s' is ambiguous; try 'help'\n", command);
        goto failed;
    }
    
    p = abbreviated_match;

done:
    if (p->command_function == 0)
        goto failed;
    return p;

failed:
    return 0;
}

void
rtems_monitor_command_usage(rtems_monitor_command_entry_t *table,
                            char *command_string)
{
    rtems_monitor_command_entry_t *help = 0;
    char *help_command_argv[2];
    
    /* if first entry in table is a usage, then print it out */
    if (command_string == 0)
    {        
        if (STREQ(table->command, "--usage--") && table->usage)
            help = table;
    }
    else
    {
        help_command_argv[0] = command_string;
        help_command_argv[1] = 0;
        help = rtems_monitor_command_lookup(table, 1, help_command_argv);
    }

    if (help)
        printf("%s\n", help->usage);
}


void
rtems_monitor_help_cmd(
    int          argc,
    char       **argv,
    unsigned32   command_arg,
    boolean verbose
)
{
    int arg;
    rtems_monitor_command_entry_t *command;

    command = (rtems_monitor_command_entry_t *) command_arg;
    
    if (argc == 1)
        rtems_monitor_command_usage(command, 0);
    else
    {
        for (arg=1; argv[arg]; arg++)
            rtems_monitor_command_usage(command, argv[arg]);
    }
}
