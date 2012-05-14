/**
 * @file
 *
 * @brief Command line editor for RTEMS monitor.
 */

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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include <rtems/monitor.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <termios.h>
#include <unistd.h>

#ifndef MONITOR_PROMPT
#define MONITOR_PROMPT "rtems"          /* will have '> ' appended */
#endif

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
  const struct translation_table *branch;
  unsigned int             key;
};

static const struct translation_table trans_two[] =
{
  { '~', 0, KEYS_INS },
  { 0,   0, 0 }
};

static const struct translation_table trans_three[] =
{
  { '~', 0, KEYS_DEL },
  { 0,   0, 0 }
};

static const struct translation_table trans_tab_csi[] =
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

static const struct translation_table trans_tab_O[] =
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

static const struct translation_table trans_tab[] =
{
  { '[', trans_tab_csi, 0 },    /* CSI command sequences */
  { 'O', trans_tab_O,   0 },    /* O are the fuction keys */
  { 0,   0,             0 }
};

/*
 * Perform a basic translation for some ANSI/VT100 key codes.
 * This code could do with a timeout on the ESC as it is
 * now lost from the input stream. It is not* used by the
 * line editor below so considiered not worth the effort.
 */

static unsigned int
rtems_monitor_getchar (void)
{
  const struct translation_table *translation = 0;
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
    fprintf(stdout,"\nMonitor ready, press enter to login.\n\n");
  else
    fprintf(stdout,"%s $ ", monitor_prompt);

  while (1)
  {
    unsigned int extended_key;
    char         c;

    fflush (stdout);

    extended_key = rtems_monitor_getchar ();
    c = extended_key & KEYS_NORMAL_MASK;

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
        fprintf(stdout,"%s $ ", monitor_prompt);
      }
    }
    else
    {
      if (extended_key)
      {
        switch (c)
        {
          case KEYS_END:
            fprintf(stdout,buffer + pos);
            pos = (int) strlen (buffer);
            break;

          case KEYS_HOME:
            fprintf(stdout,"\r%s $ ", monitor_prompt);
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
              fprintf(stdout,"\r%*c", RTEMS_COMMAND_BUFFER_SIZE, ' ');
              fprintf(stdout,"\r%s $ %s", monitor_prompt, buffer);
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
              fprintf(stdout,"\r%*c", RTEMS_COMMAND_BUFFER_SIZE, ' ');
              fprintf(stdout,"\r%s $ %s", monitor_prompt, buffer);
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
              fprintf(stdout,"\r%s $ %s", monitor_prompt, buffer);
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
              fprintf(stdout,"\b%s \b", buffer + pos);
              for (bs = 0; bs < ((int) strlen (buffer) - pos); bs++)
                putchar ('\b');
            }
            break;

          case '\n':
            /*
             * Process the command.
             */
            fprintf(stdout,"\n");
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
                for (ch = end; ch > pos; ch--)
                  buffer[ch] = buffer[ch - 1];
                fprintf(stdout,buffer + pos);
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
#if defined(RTEMS_MULTIPROCESSING)
  if (!rtems_configuration_get_user_multiprocessing_table ())
    sprintf (monitor_prompt, "%s",
             (env_prompt == NULL) ? MONITOR_PROMPT: env_prompt);
  else /* .... */
#endif
  if (rtems_monitor_default_node != rtems_monitor_node)
    sprintf (monitor_prompt, "%" PRId32 "-%s-%" PRId32 "", rtems_monitor_node,
             (env_prompt == NULL) ? MONITOR_PROMPT : env_prompt,
             rtems_monitor_default_node);
  else
    sprintf (monitor_prompt, "%" PRId32 "-%s", rtems_monitor_node,
             (env_prompt == NULL) ? MONITOR_PROMPT : env_prompt);

  rtems_monitor_line_editor (command);

  return rtems_monitor_make_argv (command, argc, argv);
}

/*
 * Main monitor command loop
 */

void
rtems_monitor_task(
    rtems_task_argument monitor_flags
)
{
#if UNUSED
    rtems_tcb *debugee = 0;
    rtems_context *rp;
#if (CPU_HARDWARE_FP == TRUE) || (CPU_SOFTWARE_FP == TRUE)
    rtems_context_fp *fp;
#endif
#endif
    char command_buffer[513];
    int argc;
    char *argv[64];
    bool  verbose = false;
    struct termios term;

    /*
     * Make the stdin stream characte not line based.
     */

    if (tcgetattr (STDIN_FILENO, &term) < 0)
    {
      fprintf(stdout,"rtems-monitor: cannot get terminal attributes.\n");
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
        fprintf(stdout,"cannot set terminal attributes\n");
      }
    }

    if (!(monitor_flags & RTEMS_MONITOR_NOSYMLOAD)) {
      rtems_monitor_symbols_loadup();
    }

    if (monitor_flags & RTEMS_MONITOR_SUSPEND)
        (void) rtems_monitor_suspend(RTEMS_NO_TIMEOUT);

    for (;;)
    {
        const rtems_monitor_command_entry_t *command;

#if UNUSED
        debugee = _Thread_Executing;
        rp = &debugee->Registers;
#if (CPU_HARDWARE_FP == TRUE) || (CPU_SOFTWARE_FP == TRUE)
        fp = debugee->fp_context;  /* possibly 0 */
#endif
#endif
        if (0 == rtems_monitor_command_read(command_buffer, &argc, argv))
            continue;
        if (argc < 1
          || (command = rtems_monitor_command_lookup(argv [0])) == 0) {
          /* no command */
          fprintf(stdout,"Unrecognised command; try 'help'\n");
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
    uint32_t   monitor_flags
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

    rtems_monitor_node = rtems_object_id_get_node(rtems_monitor_task_id);
    rtems_monitor_default_node = rtems_monitor_node;

    rtems_monitor_server_init(monitor_flags);

    if (!(monitor_flags & RTEMS_MONITOR_NOTASK)) {
      /*
       * Start the monitor task itself
       */
      status = rtems_task_start(
        rtems_monitor_task_id, rtems_monitor_task, monitor_flags);
      if (status != RTEMS_SUCCESSFUL) {
        rtems_error(status, "could not start monitor");
        return;
      }
   }
}
