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

rtems_shell_cmd_t   * rtems_shell_first_cmd;
rtems_shell_topic_t * rtems_shell_first_topic;

/*
 *  Find the topic from the set of topics registered.
 */
rtems_shell_topic_t * rtems_shell_lookup_topic(const char * topic) {
  rtems_shell_topic_t * shell_topic;
  shell_topic=rtems_shell_first_topic;

  while (shell_topic) {
    if (!strcmp(shell_topic->topic,topic))
      return shell_topic;
    shell_topic=shell_topic->next;
  }
  return NULL;
}

/*
 *  Add a new topic to the list of topics
 */
static rtems_shell_topic_t * rtems_shell_add_topic(const char * topic) {
  rtems_shell_topic_t * current,*aux;

  if (!rtems_shell_first_topic) {
    aux = malloc(sizeof(rtems_shell_topic_t));
    aux->topic = topic;
    aux->next  = NULL;
    return rtems_shell_first_topic = aux;
  }
  current=rtems_shell_first_topic;
  if (!strcmp(topic,current->topic))
    return current;

  while (current->next) {
    if (!strcmp(topic,current->next->topic))
      return current->next;
    current=current->next;
  }
  aux = malloc(sizeof(rtems_shell_topic_t));
  aux->topic = topic;
  aux->next = NULL;
  current->next = aux;
  return aux;
}

/*
 *  Find the command in the set
 */
rtems_shell_cmd_t * rtems_shell_lookup_cmd(const char * cmd) {
  rtems_shell_cmd_t * shell_cmd;
  shell_cmd=rtems_shell_first_cmd;
  while (shell_cmd) {
   if (!strcmp(shell_cmd->name,cmd)) return shell_cmd;
   shell_cmd=shell_cmd->next;
  };
  return NULL;
}

/*
 *  Add a command structure to the set of known commands
 */
rtems_shell_cmd_t *rtems_shell_add_cmd_struct(
  rtems_shell_cmd_t *shell_cmd
)
{
  rtems_shell_cmd_t *shell_pvt;

  shell_pvt = rtems_shell_first_cmd;
  while (shell_pvt) {
    if (strcmp(shell_pvt->name, shell_cmd->name) == 0)
      return NULL;
    shell_pvt = shell_pvt->next;
  }

  if ( !rtems_shell_first_cmd ) {
    rtems_shell_first_cmd = shell_cmd;
  } else {
    shell_pvt = rtems_shell_first_cmd;
    while (shell_pvt->next)
      shell_pvt = shell_pvt->next;
    shell_pvt->next = shell_cmd;
  }
  rtems_shell_add_topic( shell_cmd->topic );
  return shell_cmd;
}

/*
 *  Add a command as a set of arguments to the set and
 *  allocate the command structure on the fly.
 */
rtems_shell_cmd_t * rtems_shell_add_cmd(
  const char            *name,
  const char            *topic,
  const char            *usage,
  rtems_shell_command_t  command
)
{
  rtems_shell_cmd_t *shell_cmd = NULL;
  char *my_name = NULL;
  char *my_topic = NULL;
  char *my_usage = NULL;

  /* Reject empty commands */
  if (name == NULL || command == NULL) {
    return NULL;
  }

  /* Allocate command stucture */
  shell_cmd = (rtems_shell_cmd_t *) malloc(sizeof(rtems_shell_cmd_t));
  if (shell_cmd == NULL) {
    return NULL;
  }

  /* Allocate strings */
  my_name  = strdup(name);
  my_topic = strdup(topic);
  my_usage = strdup(usage);

  /* Assign values */
  shell_cmd->name    = my_name;
  shell_cmd->topic   = my_topic;
  shell_cmd->usage   = my_usage;
  shell_cmd->command = command;
  shell_cmd->alias   = NULL;
  shell_cmd->next    = NULL;

  if (rtems_shell_add_cmd_struct(shell_cmd) == NULL) {
    /* Something is wrong, free allocated resources */
    free(my_usage);
    free(my_topic);
    free(my_name);
    free(shell_cmd);

    return NULL;
  }

  return shell_cmd;
}


void rtems_shell_initialize_command_set(void)
{
  rtems_shell_cmd_t **c;
  rtems_shell_alias_t **a;

  for ( c = rtems_shell_Initial_commands ; *c  ; c++ ) {
    rtems_shell_add_cmd_struct( *c );
  }

  for ( a = rtems_shell_Initial_aliases ; *a  ; a++ ) {
    rtems_shell_alias_cmd( (*a)->name, (*a)->alias );
  }

  rtems_shell_register_monitor_commands();
}

/* ----------------------------------------------- *
 * you can make an alias for every command.
 * ----------------------------------------------- */
rtems_shell_cmd_t *rtems_shell_alias_cmd(
  const char *cmd,
  const char *alias
)
{
  rtems_shell_cmd_t *shell_cmd, *shell_aux;

  shell_aux = (rtems_shell_cmd_t *) NULL;

  if (alias) {
    shell_aux = rtems_shell_lookup_cmd(alias);
    if (shell_aux != NULL) {
      return NULL;
    }
    shell_cmd = rtems_shell_lookup_cmd(cmd);
    if (shell_cmd != NULL) {
      shell_aux = rtems_shell_add_cmd(
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
