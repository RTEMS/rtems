/*
 * Command parsing routines for RTEMS monitor
 *
 * TODO:
 *
 *  $Id$
 */

#include <rtems.h>

#include <rtems/monitor.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * 2001-01-30 KJO (vac4050@cae597.rsc.raytheon.com):
 *  Fixed rtems_monitor_command_lookup() to accept partial 
 *  commands to uniqeness.  Added support for setting 
 *  the monitor prompt via an environment variable:
 *  RTEMS_MONITOR_PROMPT
 *
 * CCJ: 26-3-2000, adding command history and command line
 * editing. This code is donated from My Right Boot and not
 * covered by GPL, only the RTEMS license.
 */

/*
 * Some key labels to define special keys.
 */

#define KEYS_EXTENDED    (0x8000)
#define KEYS_NORMAL_MASK (0x00ff)
#define KEYS_INS         (0)
#define KEYS_DEL         (1)
#define KEYS_UARROW      (2)
#define KEYS_DARROW      (3)
#define KEYS_LARROW      (4)
#define KEYS_RARROW      (5)
#define KEYS_HOME        (6)
#define KEYS_END         (7) 
#define KEYS_F1          (8) 
#define KEYS_F2          (9) 
#define KEYS_F3          (10)
#define KEYS_F4          (11)
#define KEYS_F5          (12)
#define KEYS_F6          (13)
#define KEYS_F7          (14)
#define KEYS_F8          (15)
#define KEYS_F9          (16)
#define KEYS_F10         (17)

#define RTEMS_COMMAND_BUFFER_SIZE (75)

static char monitor_prompt[32];
static char buffer[RTEMS_COMMAND_BUFFER_SIZE];
static int  pos;
static int  logged_in;

/*
 * History data.
 */

#define RTEMS_COMMAND_HISTORIES (20)

static char history_buffer[RTEMS_COMMAND_HISTORIES][RTEMS_COMMAND_BUFFER_SIZE];
static int  history_pos[RTEMS_COMMAND_HISTORIES];
static int  history;
static int  history_next;

/*
 * Translation tables. Not sure if this is the best way to
 * handle this, how-ever I wish to avoid the overhead of
 * including a more complete and standard environment such
 * as ncurses.
 */

struct translation_table
{
  char                     expecting;
  struct translation_table *branch;
  unsigned int             key;
};

static struct translation_table trans_two[] =
{
  { '~', 0, KEYS_INS },
  { 0,   0, 0 }
};

static struct translation_table trans_three[] =
{
  { '~', 0, KEYS_DEL },
  { 0,   0, 0 }
};

static struct translation_table trans_tab_csi[] =
{
  { '2', trans_two,   0 },
  { '3', trans_three, 0 },
  { 'A', 0,           KEYS_UARROW },
  { 'B', 0,           KEYS_DARROW },
  { 'D', 0,           KEYS_LARROW },
  { 'C', 0,           KEYS_RARROW },
  { 'F', 0,           KEYS_END },
  { 'H', 0,           KEYS_HOME },
  { 0,   0,           0 }
};

static struct translation_table trans_tab_O[] =
{
  { '1', 0, KEYS_F1 },
  { '2', 0, KEYS_F2 },
  { '3', 0, KEYS_F3 },
  { '4', 0, KEYS_F4 },
  { '5', 0, KEYS_F5 },
  { '6', 0, KEYS_F6 },
  { '7', 0, KEYS_F7 },
  { '8', 0, KEYS_F8 },
  { '9', 0, KEYS_F9 },
  { ':', 0, KEYS_F10 },
  { 'P', 0, KEYS_F1 },
  { 'Q', 0, KEYS_F2 },
  { 'R', 0, KEYS_F3 },
  { 'S', 0, KEYS_F4 },
  { 'T', 0, KEYS_F5 },
  { 'U', 0, KEYS_F6 },
  { 'V', 0, KEYS_F7 },
  { 'W', 0, KEYS_F8 },
  { 'X', 0, KEYS_F9 },
  { 'Y', 0, KEYS_F10 },
  { 0,   0, 0 }
};

static struct translation_table trans_tab[] =
{
  { '[', trans_tab_csi, 0 },    /* CSI command sequences */
  { 'O', trans_tab_O,   0 },    /* O are the fuction keys */
  { 0,   0,             0 }
};

/*
 * Perform a basic tranlation for some ANSI/VT100 key codes.
 * This code could do with a timeout on the ESC as it is
 * now lost from the input stream. It is not* used by the
 * line editor below so considiered not worth the effort.
 */

static unsigned int
rtems_monitor_getchar (
)
{
  struct translation_table *translation = 0;
  for (;;)
  {
    char c = getchar ();
    if (c == 27)
      translation = trans_tab;
    else
    {
      /*
       * If no translation happing just pass through
       * and return the key.
       */
      if (translation)
      {
        /*
         * Scan the current table for the key, and if found
         * see if this key is a fork. If so follow it and
         * wait else return the extended key.
         */
        int index    = 0;
        int branched = 0;
        while ((translation[index].expecting != '\0') ||
               (translation[index].key != '\0'))
        {
          if (translation[index].expecting == c)
          {
            /*
             * A branch is take if more keys are to come.
             */
            if (translation[index].branch == 0)
              return KEYS_EXTENDED | translation[index].key;
            else
            {
              translation = translation[index].branch;
              branched    = 1;
              break;
            }
          }
          index++;
        }
        /*
         * Who knows what these keys are, just drop them.
         */
        if (!branched)
          translation = 0;
      }
      else
        return c;
    }
  }
}

/*
 * The line editor with history.
 */

static int
rtems_monitor_line_editor (
    char *command
)
{
  int repeating = 0;

  memset (buffer, 0, RTEMS_COMMAND_BUFFER_SIZE);
  history = history_next;
  pos     = 0;
          
  if (!logged_in)
    printf ("\nMonitor ready, press enter to login.\n\n");
  else
    printf ("%s $ ", monitor_prompt);

  while (1)
  {
    unsigned int extended_key = rtems_monitor_getchar ();
    char         c = extended_key & KEYS_NORMAL_MASK;

    /*
     * Make the extended_key usable as a boolean.
     */
    extended_key &= ~KEYS_NORMAL_MASK;
    
    if (!extended_key && !logged_in)
    {
      if (c == '\n')
      {
        logged_in = 1;
        /*
         * The prompt has changed from `>' to `$' to help know
         * which version of the monitor code people are using.
         */
        printf("%s $ ", monitor_prompt);
      }
    }
    else
    {
      if (extended_key)
      {
        switch (c)
        {
          case KEYS_END:
            printf (buffer + pos);
            pos = (int) strlen (buffer);
            break;

          case KEYS_HOME:
            printf ("\r%s $ ", monitor_prompt);
            pos = 0;
            break;
        
          case KEYS_LARROW:
            if (pos > 0)
            {
              pos--;
              putchar ('\b');
            }
            break;

          case KEYS_RARROW:
            if ((pos < RTEMS_COMMAND_BUFFER_SIZE) && (buffer[pos] != '\0'))
            {
              putchar (buffer[pos]);
              pos++;
            }
            break;

          case KEYS_UARROW:
            /*
             * If we are moving up the histories then we need to save the working
             * buffer.
             */
            if (history)
            {
              int end;
              int bs;
              if (history == history_next)
              {
                memcpy (history_buffer[history_next], buffer,
                        RTEMS_COMMAND_BUFFER_SIZE);
                history_pos[history_next] = pos;
              }
              history--;
              memcpy (buffer, history_buffer[history],
                      RTEMS_COMMAND_BUFFER_SIZE);
              pos = history_pos[history];
              printf ("\r%*c", RTEMS_COMMAND_BUFFER_SIZE, ' ');
              printf ("\r%s $ %s", monitor_prompt, buffer);
              end = (int) strlen (buffer);
              for (bs = 0; bs < (end - pos); bs++)
                putchar ('\b');
            }
            break;
                  
          case KEYS_DARROW:
            if (history < history_next)
            {
              int end;
              int bs;
              history++;
              memcpy (buffer, history_buffer[history],
                      RTEMS_COMMAND_BUFFER_SIZE);
              pos = history_pos[history];
              printf ("\r%*c", RTEMS_COMMAND_BUFFER_SIZE, ' ');
              printf ("\r%s $ %s", monitor_prompt, buffer);
              end = (int) strlen (buffer);
              for (bs = 0; bs < (end - pos); bs++)
                putchar ('\b');
            }
            break;

          case KEYS_DEL:
            if (buffer[pos] != '\0')
            {
              int end;
              int bs;
              strcpy (&buffer[pos], &buffer[pos + 1]);
              printf ("\r%s $ %s", monitor_prompt, buffer);
              end = (int) strlen (buffer);
              for (bs = 0; bs < (end - pos); bs++)
                putchar ('\b');
            }
            break;
        }
      }
      else
      {
        switch (c)
        {
          case '\b':
          case '\x7e':
          case '\x7f':
            if (pos > 0)
            {
              int bs;
              pos--;
              strcpy (buffer + pos, buffer + pos + 1);
              printf ("\b%s \b", buffer + pos);
              for (bs = 0; bs < ((int) strlen (buffer) - pos); bs++)
                putchar ('\b');
            }
            break;

          case '\n':
            /*
             * Process the command.
             */
            printf ("\n");
            repeating = 1;
            /*
             * Only process the history if we have a command and
             *a history.
             */
            if (strlen (buffer))
            {
              if (history_next && (history == history_next))
              {
                /*
                 * Do not place the last command into the history
                 *if the same.
                 */
                if (strcmp (history_buffer[history_next - 1], buffer))
                  repeating = 0;
              }
              else
                repeating = 0;
            }
            if (!repeating)
            {
              memcpy (history_buffer[history_next], buffer,
                      RTEMS_COMMAND_BUFFER_SIZE);
              history_pos[history_next] = pos;
              if (history_next < (RTEMS_COMMAND_HISTORIES - 1))
                history_next++;
              else
              {
                memmove (history_buffer[0], history_buffer[1],
                         RTEMS_COMMAND_BUFFER_SIZE * (RTEMS_COMMAND_HISTORIES - 1));
                memmove (&history_pos[0], &history_pos[1],
                         sizeof (history_pos[0]) * (RTEMS_COMMAND_HISTORIES - 1));
              }
            }
            else
            {
#ifdef ENABLE_ENTER_REPEATS
              if (history_next)
                memcpy (buffer, history_buffer[history_next - 1],
                        RTEMS_COMMAND_BUFFER_SIZE);
#endif
            }
            memmove (command, buffer, RTEMS_COMMAND_BUFFER_SIZE);
            return repeating;
            break;

          default:
            if ((pos < (RTEMS_COMMAND_BUFFER_SIZE - 1)) &&
                (c >= ' ') && (c <= 'z'))
            {
              int end;
              end = strlen (buffer);
              if ((pos < end) && (end < RTEMS_COMMAND_BUFFER_SIZE))
              {
                int ch, bs;
                for (ch = end + 1; ch > pos; ch--)
                  buffer[ch] = buffer[ch - 1];
                printf (buffer + pos);
                for (bs = 0; bs < (end - pos + 1); bs++)
                  putchar ('\b');
              }
              buffer[pos++] = c;
              if (pos > end)
                buffer[pos] = '\0';
              putchar (c);
            }
            break;
        }
      }
    }
  }
}

/*
 * make_argv(cp): token-count
 *  Break up the command line in 'cp' into global argv[] and argc (return
 *  value).
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
  argv[argc] = (char *) NULL;      /* end of argv */

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
	char *env_prompt;

	env_prompt = getenv("RTEMS_MONITOR_PROMPT");

  /*
   * put node number in the prompt if we are multiprocessing
   */
  if (!rtems_configuration_get_user_multiprocessing_table ())
    sprintf (monitor_prompt, "%s",  
             (env_prompt == NULL) ? MONITOR_PROMPT: env_prompt);
  else if (rtems_monitor_default_node != rtems_monitor_node)
    sprintf (monitor_prompt, "%d-%s-%d", rtems_monitor_node,
             (env_prompt == NULL) ? MONITOR_PROMPT : env_prompt,
             rtems_monitor_default_node);
  else
    sprintf (monitor_prompt, "%d-%s", rtems_monitor_node, 
             (env_prompt == NULL) ? MONITOR_PROMPT : env_prompt);

#if defined(RTEMS_UNIX)
  /* RTEMS on unix gets so many interrupt system calls this is hosed */
  printf ("%s> ", monitor_prompt);
  fflush (stdout);
  while (gets(command) == (char *) 0)
    ;
#else
  rtems_monitor_line_editor (command);
#endif

  return rtems_monitor_make_argv (command, argc, argv);
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
  int command_length;
  rtems_monitor_command_entry_t *found_it = NULL;

  command_length = strlen (argv[0]);

  if ((table == 0) || (argv[0] == 0))
    return 0;
    
  while (table)
  {
    if (table->command)
    {

      /*
       * Check for ambiguity
       */
      if (!strncmp (table->command, argv[0], command_length))
      {
        if (found_it)
        {
          return 0;
        }
       
        else
          found_it = table;
      }
    }
    table = table->next;
  }

  /*
   * No ambiguity (the possible partial command was unique after all)
   */
  if (found_it)
  {
    if (table->command_function == 0)
      return 0;

    return found_it;
  }

  return 0;
}

void
rtems_monitor_show_help (
  rtems_monitor_command_entry_t *help_cmd,
  int                           max_cmd_len
)
{
#define MAX_HELP_LINE_LENGTH (75 - max_cmd_len - 2)

  if (help_cmd && help_cmd->command)
  {
    const char *help = help_cmd->usage;
    int         help_len = strlen (help);
    int         spaces = max_cmd_len - strlen (help_cmd->command);
    int         show_this_line = 0;
    int         line_one = 1;
    int         c;

    printf ("%s", help_cmd->command);

    if (help_len == 0)
    {
      printf (" - No help associated.\n");
      return;
    }
  
    while (help_len)
    {
      printf ("%*c", spaces, ' ');
      
      if (line_one)
        printf (" - ");

      spaces   = max_cmd_len + 2;
      line_one = 0;

      /*
       * See if greater then the line length if so, work back
       * from the end for a space, tab or lf or cr.
       */
     
      if (help_len > MAX_HELP_LINE_LENGTH)
      {
        for (show_this_line = MAX_HELP_LINE_LENGTH - 1; 
             show_this_line; 
             show_this_line--)
          if ((help[show_this_line] == ' ') ||
              (help[show_this_line] == '\n') ||
              (help[show_this_line] == '\r'))
            break;

        /*
         * If show_this_line is 0, it is a very long word !!
         */
      
        if (show_this_line == 0)
          show_this_line = MAX_HELP_LINE_LENGTH - 1;
      }
      else
        show_this_line = help_len;

      for (c = 0; c < show_this_line; c++)
        if ((help[c] == '\r') || (help[c] == '\n'))
          show_this_line = c;
        else
          putchar (help[c]);

      printf ("\n");
                  
      help     += show_this_line;
      help_len -= show_this_line;

      /*
       * Move past the line feeds or what ever else is being skipped.
       */
    
      while (help_len)
      {
        if ((*help != '\r') && (*help != '\n'))
          break;

        if (*help != ' ')
        {
          help++;
          help_len--;
          break;
        }
        help++;
        help_len--;
      }
    }
  }
}

void
rtems_monitor_command_usage(
  rtems_monitor_command_entry_t *table,
  char                          *command_string
)
{
  rtems_monitor_command_entry_t *command = table;
  int                           max_cmd_len = 0;
    
  /* if first entry in table is a usage, then print it out */

  if (command_string && (*command_string != '\0'))
  {
    char *argv[2];
    
    argv[0] = command_string;
    argv[1] = 0;
    
    command = rtems_monitor_command_lookup (table, 1, argv);

    if (command)
      rtems_monitor_show_help (command, strlen (command_string));
    else
      printf ("Unrecognised command; try just 'help'\n");
    return;
  }
  
  /*
   * Find the largest command size.
   */
  
  while (command)
  {
    int len = strlen (command->command);

    if (len > max_cmd_len)
      max_cmd_len = len;

    command = command->next;
  }

  max_cmd_len++;

  command = table;

  /*
   * Now some nice formatting for the help.
   */

  while (command)
  {
    rtems_monitor_show_help (command, max_cmd_len);
    command = command->next;
  }
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
    for (arg = 1; argv[arg]; arg++)
      rtems_monitor_command_usage(command, argv[arg]);
  }
}
