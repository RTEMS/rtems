/*
 *
 *  Shell Command Set Management
 *
 *  Author:
 *   WORK: fernando.ruiz@ctv.es
 *   HOME: correo@fernando-ruiz.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <time.h>
#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/shellconfig.h>
#include "internal.h"

/*
 * Common linked list of shell commands.
 *
 * Because the help report is very long, there is a topic for each command.
 *
 * Help list the topics
 *   help [topic] list the commands for the topic
 *   help [command] help for the command
 * 
 */

shell_cmd_t   * shell_first_cmd;
shell_topic_t * shell_first_topic;

/*
 *  Find the topic from the set of topics registered.
 */
shell_topic_t * shell_lookup_topic(char * topic) {
  shell_topic_t * shell_topic;
  shell_topic=shell_first_topic;

  while (shell_topic) {
    if (!strcmp(shell_topic->topic,topic))
      return shell_topic;
    shell_topic=shell_topic->next;
  }
  return (shell_topic_t *) NULL;
}

/*
 *  Add a new topic to the list of topics
 */
shell_topic_t * shell_add_topic(char * topic) {
  shell_topic_t * current,*aux;

  if (!shell_first_topic) {
    aux = malloc(sizeof(shell_topic_t));
    aux->topic = topic;
    aux->next  = (shell_topic_t*)NULL;
    return shell_first_topic = aux;
  }
  current=shell_first_topic;
  if (!strcmp(topic,current->topic))
    return current;

  while (current->next) {
    if (!strcmp(topic,current->next->topic))
      return current->next;
    current=current->next;
  }
  aux = malloc(sizeof(shell_topic_t));
  aux->topic = topic;
  aux->next = (shell_topic_t*)NULL;
  current->next = aux;
  return aux;
}

/*
 *  Find the command in the set
 */
shell_cmd_t * shell_lookup_cmd(char * cmd) {
  shell_cmd_t * shell_cmd;
  shell_cmd=shell_first_cmd;
  while (shell_cmd) {
   if (!strcmp(shell_cmd->name,cmd)) return shell_cmd;
   shell_cmd=shell_cmd->next;
  };
  return (shell_cmd_t *) NULL;
}

/*
 *  Add a command structure to the set of known commands
 */
shell_cmd_t *shell_add_cmd_struct(
  shell_cmd_t *shell_cmd
)
{
  shell_cmd_t *shell_pvt;

  if ( !shell_first_cmd ) {
    shell_first_cmd = shell_cmd;
  } else {
    shell_pvt = shell_first_cmd;
    while (shell_pvt->next)
      shell_pvt = shell_pvt->next;
    shell_pvt->next = shell_cmd;
  }  
  shell_add_topic( shell_cmd->topic );
  return shell_cmd;
}

/*
 *  Add a command as a set of arguments to the set and 
 *  allocate the command structure on the fly.
 */
shell_cmd_t * shell_add_cmd(
  char            *cmd,
  char            *topic,
  char            *usage,
  shell_command_t  command
)
{
  extern void register_cmds(void);

  shell_cmd_t *shell_cmd;


  if (!cmd)
    return (shell_cmd_t *) NULL;
  if (!command)
    return (shell_cmd_t *) NULL;

  shell_cmd          = (shell_cmd_t *) malloc(sizeof(shell_cmd_t));
  shell_cmd->name    = cmd;
  shell_cmd->topic   = topic;
  shell_cmd->usage   = usage;
  shell_cmd->command = command;
  shell_cmd->alias   = (shell_cmd_t *) NULL;
  shell_cmd->next    = (shell_cmd_t *) NULL;

  return shell_add_cmd_struct( shell_cmd );
}


void shell_initialize_command_set(void)
{
  shell_cmd_t **c;
  shell_alias_t **a;

  if ( shell_first_cmd )
    return;

  for ( c = Shell_Initial_commands ; *c  ; c++ ) {
    shell_add_cmd_struct( *c );
  }

  for ( a = Shell_Initial_aliases ; *a  ; a++ ) {
    shell_alias_cmd( (*a)->name, (*a)->alias );
  }

  shell_register_monitor_commands();
}

/* ----------------------------------------------- *
 * you can make an alias for every command.
 * ----------------------------------------------- */
shell_cmd_t *shell_alias_cmd(
  char *cmd,
  char *alias
)
{
  shell_cmd_t *shell_cmd, *shell_aux;

  shell_aux = (shell_cmd_t *) NULL;

  if (alias) {
    shell_aux = shell_lookup_cmd(alias);
    if (shell_aux != NULL) {
      return NULL;
    }
    shell_cmd = shell_lookup_cmd(cmd);
    if (shell_cmd != NULL) {
      shell_aux = shell_add_cmd(
         alias,
         shell_cmd->topic,
         shell_cmd->usage,
         shell_cmd->command
      );
      if (shell_aux)
        shell_aux->alias = shell_cmd;
    }
  }
  return shell_aux;
}
