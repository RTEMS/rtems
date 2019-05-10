/*
 *
 *  Instantatiate a new terminal shell.
 *
 *  Author:
 *
 *   WORK: fernando.ruiz@ctv.es
 *   HOME: correo@fernando-ruiz.com
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <time.h>

#include <rtems.h>
#include <rtems/libio.h>
#include <rtems/libio_.h>
#include <rtems/shell.h>
#include <rtems/shellconfig.h>
#include <rtems/console.h>
#include "internal.h"

#include <termios.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <pthread.h>
#include <assert.h>

const rtems_shell_env_t rtems_global_shell_env = {
  .magic         = rtems_build_name('S', 'E', 'N', 'V'),
  .devname       = CONSOLE_DEVICE_NAME,
  .taskname      = "SHGL",
  .exit_shell    = false,
  .forever       = true,
  .errorlevel    = -1,
  .echo          = false,
  .cwd           = "/",
  .input         = NULL,
  .output        = NULL,
  .output_append = false,
  .wake_on_end   = RTEMS_ID_NONE,
  .login_check   = NULL
};

static pthread_once_t rtems_shell_once = PTHREAD_ONCE_INIT;

static pthread_key_t rtems_shell_current_env_key;

/*
 *  Initialize the shell user/process environment information
 */
static rtems_shell_env_t *rtems_shell_init_env(
  rtems_shell_env_t *shell_env_p
)
{
  rtems_shell_env_t *shell_env;

  shell_env = malloc(sizeof(rtems_shell_env_t));
  if ( !shell_env )
    return NULL;
  if ( !shell_env_p ) {
    *shell_env = rtems_global_shell_env;
    shell_env->taskname = NULL;
  } else {
    *shell_env = *shell_env_p;
  }

  return shell_env;
}

/*
 *  Completely free a shell_env_t and all associated memory
 */
static void rtems_shell_env_free(
  void *ptr
)
{
  rtems_shell_env_t *shell_env;
  shell_env = (rtems_shell_env_t *) ptr;

  if ( !ptr )
    return;

  if ( shell_env->input )
    free((void *)shell_env->input);
  if ( shell_env->output )
    free((void *)shell_env->output);
  free( ptr );
}

static void rtems_shell_create_file(const char *name, const char *content)
{
  FILE *fp = fopen(name, "wx");

  if (fp != NULL) {
    fputs(content, fp);
    fclose(fp);
  }
}

static void rtems_shell_init_commands(void)
{
  rtems_shell_cmd_t * const *c;
  rtems_shell_alias_t * const *a;

  for ( c = rtems_shell_Initial_commands ; *c  ; c++ ) {
    rtems_shell_add_cmd_struct( *c );
  }

  for ( a = rtems_shell_Initial_aliases ; *a  ; a++ ) {
    rtems_shell_alias_cmd( (*a)->name, (*a)->alias );
  }
}

static void rtems_shell_init_once(void)
{
  struct passwd pwd;
  struct passwd *pwd_res;

  pthread_key_create(&rtems_shell_current_env_key, rtems_shell_env_free);

  /* dummy call to init /etc dir */
  getpwuid_r(0, &pwd, NULL, 0, &pwd_res);

  rtems_shell_create_file("etc/issue",
                          "\n"
                          "Welcome to @V\\n"
                          "Login into @S\\n");

  rtems_shell_create_file("/etc/issue.net",
                          "\n"
                          "Welcome to %v\n"
                          "running on %m\n");

  rtems_shell_init_commands();
  rtems_shell_register_monitor_commands();
}

void rtems_shell_init_environment(void)
{
  assert(pthread_once(&rtems_shell_once, rtems_shell_init_once) == 0);
}

/*
 *  Return the current shell environment
 */
rtems_shell_env_t *rtems_shell_get_current_env(void)
{
  return (rtems_shell_env_t *) pthread_getspecific(rtems_shell_current_env_key);
}

/*
 *  Duplication the current shell environment and if none is set
 *  clear it.
 */
void rtems_shell_dup_current_env(rtems_shell_env_t *copy)
{
  rtems_shell_env_t *env = rtems_shell_get_current_env();
  if (env) {
    *copy = *env;
  }
  else {
    memset(copy, 0, sizeof(rtems_shell_env_t));
    copy->magic    = rtems_build_name('S', 'E', 'N', 'V');
    copy->devname  = CONSOLE_DEVICE_NAME;
    copy->taskname = "RTSH";
  }
}

/*
 *  Get a line of user input with modest features
 */
static int rtems_shell_line_editor(
  char       *cmds[],
  int         count,
  int         size,
  const char *prompt,
  FILE       *in,
  FILE       *out
)
{
  unsigned int extended_key;
  int          c;
  int          col;
  int          last_col;
  int          output;
  char         line[size];
  char         new_line[size];
  int          up;
  int          cmd = -1;
  int          inserting = 1;

  output = (out && isatty(fileno(in)));

  col = last_col = 0;

  tcdrain(fileno(in));
  if (out)
    tcdrain(fileno(out));

  if (output && prompt)
    fprintf(out, "\r%s", prompt);

  line[0] = 0;
  new_line[0] = 0;

  for (;;) {

    if (output)
      fflush(out);

    extended_key = rtems_shell_getchar(in);

    if (extended_key == EOF)
      return -2;

    c = extended_key & RTEMS_SHELL_KEYS_NORMAL_MASK;

    /*
     * Make the extended_key usable as a boolean.
     */
    extended_key &= ~RTEMS_SHELL_KEYS_NORMAL_MASK;

    up = 0;

    if (extended_key)
    {
      switch (c)
      {
        case RTEMS_SHELL_KEYS_END:
          if (output)
            fprintf(out, "%s", line + col);
          col = (int) strlen (line);
          break;

        case RTEMS_SHELL_KEYS_HOME:
          if (output) {
            if (prompt)
              fprintf(out,"\r%s", prompt);
          }
          col = 0;
          break;

        case RTEMS_SHELL_KEYS_LARROW:
          c = 2;
          extended_key = 0;
          break;

        case RTEMS_SHELL_KEYS_RARROW:
          c = 6;
          extended_key = 0;
          break;

        case RTEMS_SHELL_KEYS_UARROW:
          c = 16;
          extended_key = 0;
          break;

        case RTEMS_SHELL_KEYS_DARROW:
          c = 14;
          extended_key = 0;
          break;

        case RTEMS_SHELL_KEYS_DEL:
          if (line[col] != '\0')
          {
            int end;
            int bs;
            strcpy (&line[col], &line[col + 1]);
            if (output) {
              fprintf(out,"\r%s%s ", prompt, line);
              end = (int) strlen (line);
              for (bs = 0; bs < ((end - col) + 1); bs++)
                fputc('\b', out);
            }
          }
          break;

        case RTEMS_SHELL_KEYS_INS:
          inserting = inserting ? 0 : 1;
          break;
      }
    }
    if (!extended_key)
    {
      switch (c)
      {
        case 1:                         /*Control-a*/
          if (output) {
            if (prompt)
              fprintf(out,"\r%s", prompt);
          }
          col = 0;
          break;

        case 2:                         /* Control-B */
          if (col > 0)
          {
            col--;
            if (output)
              fputc('\b', out);
          }
          break;

        case 4:                         /* Control-D */
          if (strlen(line)) {
            if (col < strlen(line)) {
              strcpy (line + col, line + col + 1);
              if (output) {
                int bs;
                fprintf(out,"%s \b", line + col);
                for (bs = 0; bs < ((int) strlen (line) - col); bs++)
                  fputc('\b', out);
              }
            }
            break;
          }
          /* Fall through */

        case EOF:
          if (output)
            fputc('\n', out);
          return -2;

        case 5:                         /*Control-e*/
          if (output)
            fprintf(out, "%s", line + col);
          col = (int) strlen (line);
          break;

        case 6:                         /* Control-F */
          if ((col < size) && (line[col] != '\0')) {
            if (output)
              fputc(line[col], out);
            col++;
          }
          break;

        case 7:                         /* Control-G */
          if (output) {
            /*
             * The (int) cast is needed because the width specifier (%*)
             * must be an int, but strlen() returns a size_t. Without
             * the case, the result is a printf() format warning.
             */
            fprintf(out,"\r%s%*c", prompt, (int) strlen (line), ' ');
            fprintf(out,"\r%s\x7", prompt);
          }
          memset (line, '\0', strlen(line));
          col = 0;
          break;

        case 11:                        /*Control-k*/
          if (line[col]) {
            if (output) {
              int end = strlen(line);
              int bs;
              fprintf(out,"%*c", end - col, ' ');
              for (bs = 0; bs < (end - col); bs++)
                fputc('\b', out);
            }
            line[col] = '\0';
          }
          break;

        case '\f':
          if (output) {
            int end;
            int bs;
            fputc('\f',out);
            fprintf(out,"\r%s%s", prompt, line);
            end = (int) strlen (line);
            for (bs = 0; bs < (end - col); bs++)
              fputc('\b', out);
          }
          break;

        case '\b':
        case '\x7f':
          if (col > 0)
          {
            int bs;
            col--;
            strcpy (line + col, line + col + 1);
            if (output) {
              fprintf(out,"\b%s \b", line + col);
              for (bs = 0; bs < ((int) strlen (line) - col); bs++)
                fputc('\b', out);
            }
          }
          break;

        case '\n':
        case '\r':
        {
          /*
           * Process the command.
           */
          if (output)
            fprintf(out,"\n");

          /*
           * Only process the command if we have a command and it is not
           * repeated in the history.
           */
          if (strlen(line) == 0) {
            cmd = -1;
          } else {
            if ((cmd < 0) || (strcmp(line, cmds[cmd]) != 0)) {
              if (count > 1)
                memmove(cmds[1], cmds[0], (count - 1) * size);
              memmove (cmds[0], line, size);
              cmd = 0;
            } else {
              if ((cmd > 1) && (strcmp(line, cmds[cmd]) == 0)) {
                memmove(cmds[1], cmds[0], cmd * size);
                memmove (cmds[0], line, size);
                cmd = 0;
              }
            }
          }
        }
        return cmd;

        case 16:                         /* Control-P */
          if ((cmd >= (count - 1)) || (strlen(cmds[cmd + 1]) == 0)) {
            if (output)
              fputc('\x7', out);
            break;
          }

          up = 1;
          /* drop through */

        case 14:                        /* Control-N */
        {
          int last_cmd = cmd;
          int clen = strlen (line);

          if (prompt)
            clen += strlen(prompt);

          if (up) {
            cmd++;
          } else {
            if (cmd < 0) {
              if (output)
                fprintf(out, "\x7");
              break;
            }
            else
              cmd--;
          }

          if ((last_cmd < 0) || (strcmp(cmds[last_cmd], line) != 0))
            memcpy (new_line, line, size);

          if (cmd < 0)
            memcpy (line, new_line, size);
          else
            memcpy (line, cmds[cmd], size);

          col = strlen (line);

          if (output) {
            fprintf(out,"\r%s%*c", prompt, clen, ' ');
            fprintf(out,"\r%s%s", prompt, line);
          }
        }
        break;

        case 20:                        /* Control-T */
          if (col > 0)
          {
            char tmp;
            if (col == strlen(line)) {
              col--;
              if (output)
                fprintf(out,"\b");
            }
            tmp           = line[col];
            line[col]     = line[col - 1];
            line[col - 1] = tmp;
            if (output)
              fprintf(out,"\b%c%c", line[col - 1], line[col]);
            col++;
          } else {
            if (output)
              fputc('\x7', out);
          }
          break;

        case 21:                        /* Control-U */
          if (col > 0)
          {
            int clen = strlen (line);

            strcpy (line, line + col);
            if (output) {
              fprintf(out,"\r%s%*c", prompt, clen, ' ');
              fprintf(out,"\r%s%s", prompt, line);
            }
            col = 0;
          }
          break;

        default:
          if ((col < (size - 1)) && (c >= ' ') && (c <= '~')) {
            int end = strlen (line);
            if (inserting && (col < end) && (end < size)) {
              int ch, bs;
              for (ch = end + 1; ch > col; ch--)
                line[ch] = line[ch - 1];
              if (output) {
                fprintf(out, "%s", line + col);
                for (bs = 0; bs < (end - col + 1); bs++)
                  fputc('\b', out);
              }
            }
            line[col++] = c;
            if (col > end)
              line[col] = '\0';
            if (output)
              fputc(c, out);
          }
          break;
      }
    }
  }
  return -2;
}

static bool rtems_shell_login(rtems_shell_env_t *env, FILE * in,FILE * out)
{
  FILE              *fd;
  int               c;
  time_t            t;

  if (out) {
    if ((env->devname[5]!='p')||
        (env->devname[6]!='t')||
        (env->devname[7]!='y')) {
      fd = fopen("/etc/issue","r");
      if (fd) {
        while ((c=fgetc(fd))!=EOF) {
          if (c=='@')  {
            switch(c=fgetc(fd)) {
              case 'L':
                fprintf(out,"%s", env->devname);
                break;
              case 'B':
                fprintf(out,"0");
                break;
              case 'T':
              case 'D':
                time(&t);
                fprintf(out,"%s",ctime(&t));
                break;
              case 'S':
                fprintf(out,"RTEMS");
                break;
              case 'V':
                fprintf(out,"%s\n%s",_RTEMS_version, _Copyright_Notice);
                break;
              case '@':
                fprintf(out,"@");
                break;
              default :
                fprintf(out,"@%c",c);
                break;
            }
          } else if (c=='\\')  {
            switch(c=fgetc(fd)) {
              case '\\': fprintf(out,"\\"); break;
              case 'b':  fprintf(out,"\b"); break;
              case 'f':  fprintf(out,"\f"); break;
              case 'n':  fprintf(out,"\n"); break;
              case 'r':  fprintf(out,"\r"); break;
              case 's':  fprintf(out," ");  break;
              case 't':  fprintf(out,"\t"); break;
              case '@':  fprintf(out,"@");  break;
            }
          } else {
            fputc(c,out);
          }
        }
        fclose(fd);
      }
    } else {
      fd = fopen("/etc/issue.net","r");
      if (fd) {
        while ((c=fgetc(fd))!=EOF) {
          if (c=='%')  {
            switch(c=fgetc(fd)) {
              case 't':
                fprintf(out,"%s", env->devname);
                break;
              case 'h':
                fprintf(out,"0");
                break;
              case 'D':
                fprintf(out," ");
                break;
              case 'd':
                time(&t);
                fprintf(out,"%s",ctime(&t));
                break;
              case 's':
                fprintf(out,"RTEMS");
                break;
              case 'm':
                fprintf(out,"(" CPU_NAME "/" CPU_MODEL_NAME ")");
                break;
              case 'r':
                fprintf(out,_RTEMS_version);
                break;
              case 'v':
                fprintf(out,"%s\n%s",_RTEMS_version,_Copyright_Notice);
	        break;
	      case '%':fprintf(out,"%%");
	        break;
	      default:
                fprintf(out,"%%%c",c);
                break;
            }
          } else {
            fputc(c,out);
          }
        }
        fclose(fd);
      }
    }
  }

  return rtems_shell_login_prompt(in, out, env->devname, env->login_check);
}

#if defined(SHELL_DEBUG)
void rtems_shell_print_env(
  rtems_shell_env_t * shell_env
)
{
  if ( !shell_env ) {
    printk( "shell_env is NULL\n" );
    return;
  }
  printk( "shell_env=%p\n"
    "shell_env->magic=0x%08x\t"
    "shell_env->devname=%s\n"
    "shell_env->taskname=%s\t"
    "shell_env->exit_shell=%d\t"
    "shell_env->forever=%d\n",
    shell_env->magic,
    shell_env->devname,
    ((shell_env->taskname) ? shell_env->taskname : "NOT SET"),
    shell_env->exit_shell,
    shell_env->forever
  );
}
#endif

static rtems_task rtems_shell_task(rtems_task_argument task_argument)
{
  rtems_shell_env_t *shell_env = (rtems_shell_env_t*) task_argument;
  rtems_id           wake_on_end = shell_env->wake_on_end;
  rtems_shell_main_loop( shell_env );
  if (wake_on_end != RTEMS_INVALID_ID)
    rtems_event_send (wake_on_end, RTEMS_EVENT_1);
  rtems_task_exit();
}

static bool rtems_shell_init_user_env(void)
{
  rtems_status_code sc;

  /* Make sure we have a private user environment */
  sc = rtems_libio_set_private_env();
  if (sc != RTEMS_SUCCESSFUL) {
    return false;
  }

  /* Make an effective root user */
  seteuid(0);
  setegid(0);

  return chroot("/") == 0;
}

#define RTEMS_SHELL_MAXIMUM_ARGUMENTS (128)
#define RTEMS_SHELL_CMD_SIZE          (128)
#define RTEMS_SHELL_CMD_COUNT         (32)
#define RTEMS_SHELL_PROMPT_SIZE       (128)

bool rtems_shell_main_loop(
  rtems_shell_env_t *shell_env_arg
)
{
  rtems_shell_env_t *shell_env;
  int                eno;
  struct termios     term;
  struct termios     previous_term;
  char              *prompt = NULL;
  int                cmd;
  int                cmd_count = 1; /* assume a script and so only 1 command line */
  char              *cmds[RTEMS_SHELL_CMD_COUNT];
  char              *cmd_argv;
  int                argc;
  char              *argv[RTEMS_SHELL_MAXIMUM_ARGUMENTS];
  bool               result = true;
  bool               input_file = false;
  int                line = 0;
  FILE              *stdinToClose = NULL;
  FILE              *stdoutToClose = NULL;

  rtems_shell_init_environment();

  shell_env = rtems_shell_init_env(shell_env_arg);
  if (shell_env == NULL) {
    return false;
  }

  eno = pthread_setspecific(rtems_shell_current_env_key, shell_env);
  if (eno != 0) {
    return false;
  }

  if (!rtems_shell_init_user_env()) {
    return false;
  }

  fileno(stdout);

  /* fprintf( stderr,
     "-%s-%s-\n", shell_env->input, shell_env->output );
  */

  if (shell_env->output && strcmp(shell_env->output, "stdout") != 0) {
    if (strcmp(shell_env->output, "stderr") == 0) {
      stdout = stderr;
    } else if (strcmp(shell_env->output, "/dev/null") == 0) {
      fclose (stdout);
    } else {
      FILE *output = fopen(shell_env_arg->output,
                           shell_env_arg->output_append ? "a" : "w");
      if (!output) {
        fprintf(stderr, "shell: open output %s failed: %s\n",
                shell_env_arg->output, strerror(errno));
        return false;
      }
      stdout = output;
      stdoutToClose = output;
    }
  }

  if (shell_env->input && strcmp(shell_env_arg->input, "stdin") != 0) {
    FILE *input = fopen(shell_env_arg->input, "r");
    if (!input) {
      fprintf(stderr, "shell: open input %s failed: %s\n",
              shell_env_arg->input, strerror(errno));
      return false;
    }
    stdin = input;
    stdinToClose = input;
    shell_env->forever = false;
    input_file =true;
  }
  else {
    /* make a raw terminal,Linux Manuals */
    if (tcgetattr(fileno(stdin), &previous_term) >= 0) {
      term = previous_term;
      term.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);
      term.c_oflag &= ~OPOST;
      term.c_oflag |= (OPOST|ONLCR); /* But with cr+nl on output */
      term.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
      term.c_cflag  |= CLOCAL | CREAD;
      term.c_cc[VMIN]  = 1;
      term.c_cc[VTIME] = 0;
      if (tcsetattr (fileno(stdin), TCSADRAIN, &term) < 0) {
        fprintf(stderr,
                "shell:cannot set terminal attributes(%s)\n",shell_env->devname);
      }
    }
    cmd_count = RTEMS_SHELL_CMD_COUNT;
    prompt = malloc(RTEMS_SHELL_PROMPT_SIZE);
    if (!prompt)
        fprintf(stderr,
                "shell:cannot allocate prompt memory\n");
  }

  setvbuf(stdin,NULL,_IONBF,0); /* Not buffered*/
  setvbuf(stdout,NULL,_IONBF,0); /* Not buffered*/

  /*
   * Allocate the command line buffers.
   */
  cmd_argv = malloc (RTEMS_SHELL_CMD_SIZE);
  if (!cmd_argv) {
    fprintf(stderr, "no memory for command line buffers\n" );
  }

  cmds[0] = calloc (cmd_count, RTEMS_SHELL_CMD_SIZE);
  if (!cmds[0]) {
    fprintf(stderr, "no memory for command line buffers\n" );
  }

  if (cmd_argv && cmds[0]) {

    memset (cmds[0], 0, cmd_count * RTEMS_SHELL_CMD_SIZE);

    for (cmd = 1; cmd < cmd_count; cmd++) {
      cmds[cmd] = cmds[cmd - 1] + RTEMS_SHELL_CMD_SIZE;
    }

    do {
      result = rtems_shell_init_user_env();

      if (result) {
        /*
         *  By using result here, we can fall to the bottom of the
         *  loop when the connection is dropped during login and
         *  keep on trucking.
         */
        if (shell_env->login_check != NULL) {
          result = rtems_shell_login(shell_env, stdin,stdout);
        } else {
          setuid(shell_env->uid);
          setgid(shell_env->gid);
          seteuid(shell_env->uid);
          setegid(shell_env->gid);
          rtems_current_user_env_getgroups();

          result = true;
        }
      }

      if (result)  {
        const char *c;
        memset (cmds[0], 0, cmd_count * RTEMS_SHELL_CMD_SIZE);
        if (!input_file) {
          rtems_shell_cat_file(stdout,"/etc/motd");
          fprintf(stdout, "\n"
                  "RTEMS Shell on %s. Use 'help' to list commands.\n",
                  shell_env->devname);
        }

        if (input_file)
          chdir(shell_env->cwd);
        else
          chdir("/"); /* XXX: chdir to getpwent homedir */

        shell_env->exit_shell = false;

        for (;;) {
          int cmd;

          /* Prompt section */
          if (prompt) {
            rtems_shell_get_prompt(shell_env, prompt,
                                   RTEMS_SHELL_PROMPT_SIZE);
          }

          /* getcmd section */
          cmd = rtems_shell_line_editor(cmds, cmd_count,
                                        RTEMS_SHELL_CMD_SIZE, prompt,
                                        stdin, stdout);

          if (cmd == -1)
            continue; /* empty line */

          if (cmd == -2)
            break; /*EOF*/

          line++;

          if (shell_env->echo)
            fprintf(stdout, "%d: %s\n", line, cmds[cmd]);

          /* evaluate cmd section */
          c = cmds[cmd];
          while (*c) {
            if (!isblank((unsigned char)*c))
              break;
            c++;
          }

          if (*c == '\0')   /* empty line */
            continue;

          if (*c == '#') {  /* comment character */
            cmds[cmd][0] = 0;
            continue;
          }

          if (!strcmp(cmds[cmd],"bye") || !strcmp(cmds[cmd],"exit")) {
            fprintf(stdout, "Shell exiting\n" );
            break;
          }

          /* exec cmd section */
          /* TODO:
           *  To avoid user crash catch the signals.
           *  Open a new stdio files with posibility of redirection *
           *  Run in a new shell task background. (unix &)
           *  Resuming. A little bash.
           */
          memcpy (cmd_argv, cmds[cmd], RTEMS_SHELL_CMD_SIZE);
          if (!rtems_shell_make_args(cmd_argv, &argc, argv,
                                     RTEMS_SHELL_MAXIMUM_ARGUMENTS)) {
            shell_env->errorlevel = rtems_shell_execute_cmd(argv[0], argc, argv);
          }

          /* end exec cmd section */
          if (shell_env->exit_shell)
            break;
        }

        fflush( stdout );
        fflush( stderr );
      }
    } while (result && shell_env->forever);

  }

  if (cmds[0])
    free (cmds[0]);
  if (cmd_argv)
    free (cmd_argv);
  if (prompt)
    free (prompt);

  if (stdinToClose) {
    fclose( stdinToClose );
  } else {
    if (tcsetattr(fileno(stdin), TCSADRAIN, &previous_term) < 0) {
      fprintf(
        stderr,
        "shell: cannot reset terminal attributes (%s)\n",
        shell_env->devname
      );
    }
  }
  if ( stdoutToClose )
    fclose( stdoutToClose );
  return result;
}

/* ----------------------------------------------- */
static rtems_status_code rtems_shell_run (
  const char *task_name,
  size_t task_stacksize,
  rtems_task_priority task_priority,
  const char *devname,
  bool forever,
  bool wait,
  const char *input,
  const char *output,
  bool output_append,
  rtems_id wake_on_end,
  bool echo,
  rtems_shell_login_check_t login_check
)
{
  rtems_id           task_id;
  rtems_status_code  sc;
  rtems_shell_env_t *shell_env;
  rtems_name         name;

  if ( task_name && strlen(task_name) >= 4)
    name = rtems_build_name(
      task_name[0], task_name[1], task_name[2], task_name[3]);
  else
    name = rtems_build_name( 'S', 'E', 'N', 'V' );

  sc = rtems_task_create(
    name,
    task_priority,
    task_stacksize,
    RTEMS_PREEMPT | RTEMS_TIMESLICE | RTEMS_NO_ASR,
    RTEMS_LOCAL | RTEMS_FLOATING_POINT,
    &task_id
  );
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  shell_env = rtems_shell_init_env( NULL );
  if ( !shell_env )  {
   return RTEMS_NO_MEMORY;
  }
  shell_env->devname       = devname;
  shell_env->taskname      = task_name;
  shell_env->exit_shell    = false;
  shell_env->forever       = forever;
  shell_env->echo          = echo;
  shell_env->input         = strdup (input);
  shell_env->output        = strdup (output);
  shell_env->output_append = output_append;
  shell_env->wake_on_end   = wake_on_end;
  shell_env->login_check   = login_check;
  shell_env->uid           = getuid();
  shell_env->gid           = getgid();

  getcwd(shell_env->cwd, sizeof(shell_env->cwd));

  sc = rtems_task_start(task_id, rtems_shell_task,
                          (rtems_task_argument) shell_env);
  if (sc != RTEMS_SUCCESSFUL) {
    return sc;
  }

  if (wait) {
    rtems_event_set out;
    sc = rtems_event_receive (RTEMS_EVENT_1, RTEMS_WAIT, 0, &out);
  }

  return 0;
}

rtems_status_code rtems_shell_init(
  const char *task_name,
  size_t task_stacksize,
  rtems_task_priority task_priority,
  const char *devname,
  bool forever,
  bool wait,
  rtems_shell_login_check_t login_check
)
{
  rtems_id to_wake = RTEMS_ID_NONE;

  if ( wait )
    to_wake = rtems_task_self();

  return rtems_shell_run(
    task_name,               /* task_name */
    task_stacksize,          /* task_stacksize */
    task_priority,           /* task_priority */
    devname,                 /* devname */
    forever,                 /* forever */
    wait,                    /* wait */
    "stdin",                 /* input */
    "stdout",                /* output */
    false,                   /* output_append */
    to_wake,                 /* wake_on_end */
    false,                   /* echo */
    login_check              /* login check */
  );
}

rtems_status_code   rtems_shell_script (
  const char          *task_name,
  size_t               task_stacksize,
  rtems_task_priority  task_priority,
  const char*          input,
  const char*          output,
  bool                 output_append,
  bool                 wait,
  bool                 echo
)
{
  rtems_id          current_task = RTEMS_INVALID_ID;
  rtems_status_code sc;

  if (wait) {
    sc = rtems_task_ident (RTEMS_SELF, RTEMS_LOCAL, &current_task);
    if (sc != RTEMS_SUCCESSFUL)
      return sc;
  }

  sc = rtems_shell_run(
    task_name,       /* task_name */
    task_stacksize,  /* task_stacksize */
    task_priority,   /* task_priority */
    NULL,            /* devname */
    0,               /* forever */
    wait,            /* wait */
    input,           /* input */
    output,          /* output */
    output_append,   /* output_append */
    current_task,    /* wake_on_end */
    echo,            /* echo */
    NULL             /* login check */
  );
  if (sc != RTEMS_SUCCESSFUL)
    return sc;

  return sc;
}
