/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief  Shell Help Command
 */

/*
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/shell.h>

#include "internal.h"
#include <string.h>

static int rtems_shell_help_pause(int line, int lines) {
  if (lines && line >= lines - 1) {
    printf("\rPress any key to continue...");
    (void) getchar();
    printf("\r%*c\r", 29, ' ');
    line = 0;
  }
  return line;
}

/*
 * show the help for one command.
 */
static int rtems_shell_help_cmd(
  const rtems_shell_cmd_t *shell_cmd, int indent, int line,
  int cols, int lines
)
{
  const char * pc;
  int    col;

  if (!rtems_shell_can_see_cmd(shell_cmd)) {
    return 0;
  }

  printf("%-*s - ", indent, shell_cmd->name);
  indent += 3;
  col = indent;
  if (shell_cmd->alias) {
    printf("is an <alias> for command '%s'",shell_cmd->alias->name);
  } else if (shell_cmd->usage) {
    pc = shell_cmd->usage;
    while (*pc) {
      switch(*pc) {
        case '\r':
          break;
        case '\n':
          if (*(pc + 1) != '\0') {
            putchar('\n');
            col = 0;
          }
          break;
        default:
          putchar(*pc);
          col++;
          break;
      }
      pc++;
      if (col > (cols - 3)) {
        if (*pc) {
          putchar('\n');
          col = 0;
        }
      }
      if (col == 0 && *pc) {
        line = rtems_shell_help_pause(line + 1, lines);
        printf("%*c", indent, ' ');
        col = indent;
      }
    }
  }
  puts("");
  line = rtems_shell_help_pause(line + 1, lines);
  return line;
}

/*
 * show the help. The first command implemented.
 * Can you see the header of routine? Known?
 * The same with all the commands....
 */
static int rtems_shell_help(
  int argc,
  char * argv[]
)
{
  int col,line,cols,lines,arg,indent;
  char *lines_env, *cols_env;
  rtems_shell_topic_t *topic;
  rtems_shell_cmd_t *shell_cmd;

  lines = 16;
  cols = 80;
  lines_env = getenv("SHELL_LINES");
  if (lines_env) {
    lines = strtol(lines_env, 0, 0);
  } else {
    lines_env = getenv("LINES");
    if (lines_env) {
      lines = strtol(lines_env, 0, 0);
    }
  }

  cols_env = getenv("COLUMNS");
  if (cols_env) {
    cols = strtol(cols_env, 0, 0);
  }

  if (argc<2) {
    printf("help: The topics are\n");
    topic = rtems_shell_first_topic;
    col = printf("  all");
    while (topic) {
      if (!col){
        col = printf("  %s",topic->topic);
      } else {
        if ((col+(int)strlen(topic->topic)+2)>(cols - 2)){
          printf("\n");
          col = printf("  %s",topic->topic);
        } else {
          col+= printf(", %s",topic->topic);
        }
      }
      topic = topic->next;
    }
    printf("\n");
    return 1;
  }
  indent = 0;
  shell_cmd = rtems_shell_first_cmd;
  while (shell_cmd) {
    size_t len = strlen(shell_cmd->name);
    if ((int)len > indent) {
      indent = len;
    }
    shell_cmd = shell_cmd->next;
  }
  line = 0;
  for (arg = 1;arg<argc;arg++) {
    const char *cur = argv[arg];
    topic = rtems_shell_lookup_topic(cur);
    if (topic == NULL) {
      if ((shell_cmd = rtems_shell_lookup_cmd(cur)) == NULL) {
        if (strcmp(cur, "all") != 0) {
          printf(
            "help: topic or cmd '%s' not found. Try <help> alone for a list\n",
            cur
          );
          line = rtems_shell_help_pause(line + 1, lines);
          continue;
        }
      } else {
        line = rtems_shell_help_cmd(shell_cmd, indent, line, cols, lines);
        continue;
      }
    }
    printf("help: list for the topic '%s'\n",cur);
    line++;
    shell_cmd = rtems_shell_first_cmd;
    while (shell_cmd) {
      if (topic == NULL || !strcmp(topic->topic,shell_cmd->topic)) {
        line = rtems_shell_help_cmd(shell_cmd, indent, line, cols, lines);
      }
      shell_cmd = shell_cmd->next;
    }
  }
  return 0;
}

rtems_shell_cmd_t rtems_shell_HELP_Command  =  {
  .name = "help",
  .usage = "help [topic] # list of usage of commands",
  .topic = "help",
  .command = rtems_shell_help,
  .mode = S_IRUSR | S_IXUSR | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH
};
