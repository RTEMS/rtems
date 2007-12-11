/*
 *  Shell Internal Information
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SHELL_INTERNAL_h
#define _RTEMS_SHELL_INTERNAL_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

struct shell_topic_tt;
typedef struct shell_topic_tt shell_topic_t;

struct shell_topic_tt {
  char          *topic;
  shell_topic_t *next;
};


extern shell_cmd_t   * shell_first_cmd;
extern shell_topic_t * shell_first_topic;

shell_topic_t * shell_lookup_topic(char * topic);


void shell_register_monitor_commands(void);
void shell_initialize_command_set(void);

#endif
