/*
 *
 *  Shell Help Command
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

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/system.h>
#include <rtems/shell.h>

#include "internal.h"
#include <string.h>

/*
 * show the help for one command.
 */
int shell_help_cmd(shell_cmd_t * shell_cmd) {
  char * pc;
  int    col,line;

  printf("%-10.10s - ",shell_cmd->name);
  col = 12;
  line = 1;
  if (shell_cmd->alias) {
    printf("is an <alias> for command '%s'",shell_cmd->alias->name);
  } else if (shell_cmd->usage) {
    pc = shell_cmd->usage;
    while (*pc) {
      switch(*pc) {
        case '\r':
          break;
        case '\n':
          putchar('\n');
          col = 0;
          break;
        default:
          putchar(*pc);
          col++;
          break;
      }
      pc++;
      if (col>78) { /* What daring... 78?*/
        if (*pc) {
          putchar('\n');
          col = 0;
        }
      }
      if (!col && *pc) {
        printf("            ");
        col = 12;line++;
      }
    }
  }
  puts("");
  return line;
}

/*
 * show the help. The first command implemented.
 * Can you see the header of routine? Known?
 * The same with all the commands....
 */
int shell_help(int argc,char * argv[]) {
  int col,line,arg;
  shell_topic_t *topic;
  shell_cmd_t * shell_cmd = shell_first_cmd;

  if (argc<2) {
    printf("help: ('r' repeat last cmd - 'e' edit last cmd)\n"
           "  TOPIC? The topics are\n");
    topic = shell_first_topic;
    col = 0;
    while (topic) {
      if (!col){
        col = printf("   %s",topic->topic);
      } else {
        if ((col+strlen(topic->topic)+2)>78){
          printf("\n");
          col = printf("   %s",topic->topic);
        } else {
          col+= printf(", %s",topic->topic);
        }
      }
      topic = topic->next;
    }
    printf("\n");
    return 1;
  }
  line = 0;
  for (arg = 1;arg<argc;arg++) {
    if (line>16) {
      printf("Press any key to continue...");getchar();
      printf("\n");
      line = 0;
    }
    topic  =  shell_lookup_topic(argv[arg]);
    if (!topic){
      if ((shell_cmd = shell_lookup_cmd(argv[arg])) == NULL) {
        printf("help: topic or cmd '%s' not found. Try <help> alone for a list\n",
            argv[arg]);
        line++;
      } else {
        line+= shell_help_cmd(shell_cmd);
      }
      continue;
    }
    printf("help: list for the topic '%s'\n",argv[arg]);
    line++;
    while (shell_cmd) {
      if (!strcmp(topic->topic,shell_cmd->topic))
        line+= shell_help_cmd(shell_cmd);
      if (line>16) {
        printf("Press any key to continue...");
        getchar();
        printf("\n");
        line = 0;
      }
      shell_cmd = shell_cmd->next;
    }
  }
  puts("");
  return 0;
}

shell_cmd_t Shell_HELP_Command  =  {
  "help",                                       /* name  */
   "help [topic] # list of usage of commands",  /* usage */
  "help",                                       /* topic */
  shell_help,                                   /* command */
  NULL,                                         /* alias */
  NULL                                          /* next */
};
