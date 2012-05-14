/*
 *  Shell Script Invocation
 *
 *  Pseudo-code from Chris Johns, implemented and debugged
 *  by Joel Sherrill.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include <sys/types.h>
#include <unistd.h>
#define __need_getopt_newlib
#include <getopt.h>

#include <rtems.h>
#include <rtems/shell.h>
#include <rtems/stringto.h>
#include "internal.h"

static void rtems_shell_joel_usage(void)
{
  printf(
    "joel [args] where args may be:\n"
    "  -o FILE     output file (default=stdout)\n"
    "  -p PRIORITY task priority\n"
    "  -s SIZE     task stack size\n"
    "  -t NAME     task name\n"
  );
}

static int findOnPATH(
  const char *userScriptName,
  char       *scriptFile,
  size_t      scriptFileLength
)
{
  int sc;

  /*
   *  If the user script name starts with a / assume it is a fully
   *  qualified path name and just use it.
   */
  if ( userScriptName[0] == '/' ) {
    strncpy( scriptFile, userScriptName, PATH_MAX );
  } else {
    /*
     *  For now, the provided name is just turned into a fully
     *  qualified path name and used.  There is no attempt to
     *  search along a path for it.
     */

    /* XXX should use strncat but what is the limit? */
    getcwd( scriptFile, PATH_MAX );
    strncat( scriptFile, "/", PATH_MAX );
    strncat(
      scriptFile,
      ( (userScriptName[0] == '.' && userScriptName[1] == '/') ?
         &userScriptName[2] : userScriptName),
      PATH_MAX
    );
  }

  sc = access( scriptFile, R_OK );
  if ( sc ) {
    return -1;
  }

  return 0;

#if 0
   /*
    * Does the command (argv[0]) contain a path ?, i.e. starts with
    * '.' or contains a '/'?
    */
   /* TODO: Add concept of PATH */
   if (!contains_path) {
     /* check PATH environment variable */
     for (path_part = PATH; path_part; skip to ':')
     {
     }
     if (not found)
       return -1;
   }
#endif
}

int rtems_shell_main_joel(
  int    argc,
  char **argv
);

int rtems_shell_main_joel(
  int    argc,
  char **argv
)
{
  unsigned long        tmp;
  int                  option;
  int                  sc;
  int                  verbose = 0;
  char                *taskName = "JOEL";
  uint32_t             stackSize = RTEMS_MINIMUM_STACK_SIZE * 10;
  rtems_task_priority  taskPriority = 20;
  char                *outputFile = "stdout";
  rtems_status_code    result;
  char                 scriptFile[PATH_MAX];
  struct getopt_data   getopt_reent;

  memset(&getopt_reent, 0, sizeof(getopt_data));
  while ( (option = getopt_r( argc, argv, "o:p:s:t:v", &getopt_reent)) != -1 ) {
    switch ((char)option) {
      case 'o':
        outputFile = getopt_reent.optarg;
        break;
      case 'p': {
        const char *s = getopt_reent.optarg;

	if ( rtems_string_to_unsigned_long( s, &tmp, NULL, 0) ) {
	  printf( "Task Priority argument (%s) is not a number\n", s );
	  return -1;
	}
	taskPriority = (rtems_task_priority) tmp;
        break;
      }
      case 's': {
        const char *s = getopt_reent.optarg;

	if ( rtems_string_to_unsigned_long( s, &tmp, NULL, 0) ) {
	  printf( "Stack size argument (%s) is not a number\n", s );
	  return -1;
	}
        stackSize = (uint32_t) tmp;
        break;
      }
      case 't':
        taskName = getopt_reent.optarg;
        break;
      case 'v':
        verbose = 1;
        break;
      case '?':
      default:
        rtems_shell_joel_usage();
        return -1;
    }
  }

  if ( verbose ) {
    fprintf( stderr,
      "outputFile: %s\n"
      "taskPriority: %" PRId32 "\n"
      "stackSize: %" PRId32 "\n"
      "taskName: %s\n",
      outputFile,
      taskPriority,
      stackSize,
      taskName
   );
  }

  /*
   *  Verify there is a script name past the end of the arguments.
   *  Preincrement to skip program name.
   */
  if ( getopt_reent.optind >= argc ) {
    fprintf( stderr, "Shell: No script to execute\n" );
    return -1;
  }

  /*
   *  Find script on the path.
   *
   *  NOTE: It is terrible that this is done twice but it
   *        seems to be the most expedient thing.
   */
  sc = findOnPATH( argv[getopt_reent.optind], scriptFile, PATH_MAX );
  if ( sc ) {
    fprintf( stderr, "%s: command not found\n", argv[0] );
    return -1;
  }

  /* fprintf( stderr, "SCRIPT: -%s-\n", scriptFile ); */

  /*
   *  I assume that argv[optind...] will have the arguments to
   *  the shell script.  But that remains to be implemented.
   */

  /*
   * Run the script
   */
  result = rtems_shell_script(
    taskName,        /* the name of the task */
    stackSize,       /* stack size */
    taskPriority,    /* task priority */
    scriptFile,      /* the script file */
    outputFile,      /* where to redirect the script */
    0,               /* run once and exit */
    1,               /* we will wait */
    verbose          /* do we echo */
  );
  if (result)
    return -1;
  return 0;
}

rtems_shell_cmd_t rtems_shell_JOEL_Command = {
  "joel",                        /* name */
  "joel [args] SCRIPT",          /* usage */
  "misc",                        /* topic */
  rtems_shell_main_joel,         /* command */
  NULL,                          /* alias */
  NULL                           /* next */
};

/*
 *  This is a helper function which takes a command as arguments
 *  which has not been located as a built-in command and attempts
 *  to find something in the filesystem with the same name that
 *  appears to be a shell script.
 */
int rtems_shell_script_file(
  int   argc __attribute__((unused)),
  char *argv[]
)
{
  #define FIRST_LINE_LENGTH 128
  #define SCRIPT_ARGV_LIMIT 32
  char    scriptFile[PATH_MAX];
  char   *scriptHead;
  char    scriptHeadBuffer[FIRST_LINE_LENGTH];
  int     sc;
  FILE   *script;
  size_t  length;
  int     scriptArgc;
  char   *scriptArgv[SCRIPT_ARGV_LIMIT];

  /*
   *  Clear argv pointer array
   */
  for ( scriptArgc=0 ; scriptArgc<SCRIPT_ARGV_LIMIT ; scriptArgc++ )
    scriptArgv[scriptArgc] = NULL;

  /*
   *  Find argv[0] on the path
   */
  sc = findOnPATH( argv[0], scriptFile, PATH_MAX );
  if ( sc ) {
    fprintf( stderr, "%s: command not found\n", argv[0] );
    return -1;
  }

  /*
   *  Open the file so we can see if it looks like a script.
   */
  script = fopen( scriptFile, "r" );
  if ( !script ) {
    fprintf( stderr, "%s: Unable to open %s\n", argv[0], scriptFile );
    return -1;
  }

  /*
   *  Is the script OK to run?
   *  Verify the current user has permission to execute it.
   *
   *  NOTE: May not work on all file systems
   */
  sc = access( scriptFile, X_OK );
  if ( sc ) {
    fprintf( stderr, "Unable to execute %s\n", scriptFile );
    fclose( script );
    return -1;
  }

  /*
   *  Try to read the first line from the potential script file
   */
  scriptHead = fgets(scriptHeadBuffer, FIRST_LINE_LENGTH, script);
  if ( !scriptHead ) {
    fprintf(
      stderr, "%s: Unable to read first line of %s\n", argv[0], scriptFile );
    fclose( script );
    return -1;
  }

  fclose(script);

  length = strnlen(scriptHead, FIRST_LINE_LENGTH);
  scriptHead[length - 1] = '\0';

  /* fprintf( stderr, "FIRST LINE: -%s-\n", scriptHead ); */

  /*
   *  Verify the name of the "shell" is joel.  This means
   *  the line starts with "#! joel".
   */
  if (strncmp("#! joel", scriptHead, 7) != 0) {
    fprintf( stderr, "%s: Not a joel script %s\n", argv[0], scriptFile );
    return -1;
  }

  /*
   * Do not worry about search path further.  We have found the
   * script, it is executable, and we have successfully read the
   * first line and found out it is a script.
   */

  /*
   * Check for arguments in the first line of the script.  This changes
   * how the shell task is run.
   */

  sc = rtems_shell_make_args(
    &scriptHead[3],
    &scriptArgc,
    scriptArgv,
    SCRIPT_ARGV_LIMIT - 1
  );
  if ( sc ) {
    fprintf(
      stderr, "%s: Error parsing joel arguments %s\n", argv[0], scriptFile );
    return -1;
  }

  scriptArgv[ scriptArgc++ ] = scriptFile;

  /*
   *  TODO: How do we pass arguments from here to the script?
   *        At this point, it doesn't matter because we don't
   *        have any way for a shell script to access them.
   */
  return rtems_shell_main_joel( scriptArgc, scriptArgv );

  return 0;
}
