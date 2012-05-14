/*
 *  eolstrip - strip white space from end of lines
 *
 *  This program strips the white space from the end of every line in the
 *  specified program.
 *
 *  usage:  eolstrip  [ -v ] [ arg ... ] files...
 *           -v          -- verbose
 */

#define GETOPTARGS "vt"

char *USAGE = "\
usage:  cklength  [ -v ] [ arg ... ] files... \n\
            -v          -- verbose\n\
            -t          -- test only .. DO NOT OVERWRITE FILE!!!\n\
\n\
Strip the white space from the end of every line on the list of files.\n\
";

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>
#include <errno.h>

#include "config.h"

#ifndef VMS
#ifndef HAVE_STRERROR
extern int sys_nerr;
extern char *sys_errlist[];

#define strerror( _err ) \
  ((_err) < sys_nerr) ? sys_errlist [(_err)] : "unknown error"

#else   /* HAVE_STRERROR */
char *strerror ();
#endif
#else   /* VMS */
char *strerror (int,...);
#endif


#define BUFFER_SIZE     2048
#define MAX_PATH        2048

#define SUCCESS         0
#define FAILURE         -1
#define Failed(x)       (((int) (x)) == FAILURE)
#define TRUE    1
#define FALSE   0
#define STREQ(a,b)      (strcmp(a,b) == 0)
#define NUMELEMS(arr)   (sizeof(arr) / sizeof(arr[0]))

/*
 * Definitions for unsigned "ints"; especially for use in data structures
 *  that will be shared among (potentially) different cpu's (we punt on
 *  byte ordering problems tho)
 */

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned long   u32;

/*
 * vars controlled by command line options
 */

int verbose = FALSE;                    /* be verbose */
int test_only = FALSE;                  /* test only */

extern char *optarg;                    /* getopt(3) control vars */
extern int optind, opterr;

char *progname;                         /* for error() */

int process(char *arg);
void error(int errn, ...);
long getparm(char *s, long min, long max, char *msg);

#define ERR_ERRNO  (1<<((sizeof(int) * 8) - 2)) /* hi bit; use 'errno' */
#define ERR_FATAL  (ERR_ERRNO / 2)              /* fatal error ; no return */
#define ERR_ABORT  (ERR_ERRNO / 4)              /* fatal error ; abort */
#define ERR_MASK   (ERR_ERRNO | ERR_FATAL | ERR_ABORT) /* all */

#define stol(p) strtol(p, (char **) NULL, 0)
int  Open(), Read(), Write();

int main(
  int argc,
  char **argv
)
{
    register int c;
    int showusage = FALSE;                      /* usage error? */
    int rc = 0;

    /*
     * figure out invocation leaf-name
     */

    if ((progname = strrchr(argv[0], '/')) == (char *) NULL)
        progname = argv[0];
    else
        progname++;

    argv[0] = progname;                         /* for getopt err reporting */

    /*
     *  Check options and arguments.
     */

    opterr = 0;                                 /* we'll report all errors */
    while ((c = getopt(argc, argv, GETOPTARGS)) != EOF)
        switch (c)
        {
            case 't':                           /* toggle test only mode */
                test_only = ! test_only;
                break;

            case 'v':                           /* toggle verbose */
                verbose = ! verbose;
                break;

            case '?':
                showusage = TRUE;
        }

    if (showusage)
    {
        (void) fprintf(stderr, "%s", USAGE);
        exit(1);
    }

    /*
     *  traverse and process the arguments
     */

    for ( ; argv[optind]; optind++)
        if (Failed(process(argv[optind])))
            rc = FAILURE;

    return rc;
}


/*
 * process(arg)
 */

int
process(char *arg)
{
  FILE   *in;
  FILE   *out = (FILE *) 0;
  char    outname[ MAX_PATH ];
  char   *bptr;
  char    buffer[ BUFFER_SIZE ];
  int     length;
  int     line_number;
  int     rc = SUCCESS;  /* succeed by default */

  in = fopen( arg, "r" );
  if (!in)
    error( ERR_ERRNO | ERR_FATAL, "Unable to open file (%s)\n", arg );

  if ( !test_only ) {
    sprintf( outname, "%s.eoltmp", arg );

    out = fopen( outname, "w" );
    if (!out)
      error( ERR_ERRNO | ERR_FATAL, "Unable to open file (%s)\n", arg );
  }

  if ( verbose )
    fprintf( stderr, "Processing %s\n", arg );

  for ( line_number=1 ; ; line_number++ ) {
    bptr = fgets( buffer, BUFFER_SIZE, in );
    if (!bptr)
      break;

    /*
     *  Don't count the carriage return.
     */

    length = strlen( buffer ) - 1;

    if ( buffer[ length ] != '\n' )
      error(ERR_ERRNO|ERR_FATAL, "Line %d too long in %s\n", line_number, arg);

    while ( isspace( (unsigned char) buffer[ length ] ) )
      buffer[ length-- ] = '\0';

    if ( test_only ) {
      fprintf( stderr, "%s\n", arg );
      break;
    }

    fprintf( out, "%s\n", buffer );
  }

  fclose( in );
  if ( !test_only ) {
    fclose( out );
    rename( outname, arg );
  }
  return rc;
}

/*
 * error(errn, arglist)
 *      report an error to stderr using printf(3) conventions.
 *      Any output is preceded by '<progname>: '
 *
 * Uses ERR_FATAL bit to request exit(errn)
 *      ERR_ABORT to request abort()
 *      ERR_ERRNO to indicate use of errno instead of argument.
 *
 * If resulting 'errn' is non-zero, it is assumed to be an 'errno' and its
 *      associated error message is appended to the output.
 */

/*VARARGS*/

void
error(int error_flag, ...)
{
    va_list arglist;
    register char *format;
    int local_errno;

    extern int errno;

    (void) fflush(stdout);          /* in case stdout/stderr same */

    local_errno = error_flag & ~ERR_MASK;
    if (error_flag & ERR_ERRNO)     /* use errno? */
        local_errno = errno;

    va_start(arglist, error_flag);
    format = va_arg(arglist, char *);
    (void) fprintf(stderr, "%s: ", progname);
    (void) vfprintf(stderr, format, arglist);
    va_end(arglist);

    if (local_errno)
      (void) fprintf(stderr, " (%s)\n", strerror(local_errno));
    else
      (void) fprintf(stderr, "\n");

    (void) fflush(stderr);

    if (error_flag & (ERR_FATAL | ERR_ABORT))
    {
        if (error_flag & ERR_FATAL)
        {
            error(0, "fatal error, exiting");
            exit(local_errno ? local_errno : 1);
        }
        else
        {
            error(0, "fatal error, aborting");
            abort();
        }
    }
}

long
getparm(char *s,
        long min,
        long max,
        char *msg)
{
    long val;

    if ( ! strchr("0123456789-", *s))
    {
        error(ERR_FATAL, "'%s' is not a number", s);
        return min;
    }

    val = strtol(s, (char **) NULL, 0);
    if ((val < min) || (val > max))
    {
        if (min == max)
            error(ERR_FATAL, "%s can only be %ld", s, min);
        else
            error(ERR_FATAL, "%s must be between %ld and %ld", msg, min, max);
    }

    return val;
}


/*
 * Open()
 *      Perform open(2), returning the file descriptor.  Prints
 *      error message if open fails.
 */

int
Open(char *file,
     int oflag,
     int mode)
{
    int O_fd;

    if (Failed(O_fd = open(file, oflag, mode)))
        error(
          ERR_ERRNO | ERR_FATAL,
          "open('%s', 0x%x, 0%o) failed", file, oflag, mode
        );

    return O_fd;
}

/*
 * Read()
 *      Perform read(2); prints error message if fails.
 */

int
Read(int file,
     char *buffer,
     unsigned int count)
{
    int nbytes;

    if (Failed(nbytes = read(file, buffer, count)))
        error(
          ERR_ERRNO | ERR_FATAL,
          "read(%d, 0x%x, %d) failed", file, buffer, count
        );

    return nbytes;
}

/*
 * Write()
 *      Perform write(2); prints error message if fails.
 */

int
Write(int file,
      char *buffer,
      unsigned int count)
{
    int nbytes;

    if (Failed(nbytes = write(file, buffer, count)))
        error(
          ERR_ERRNO | ERR_FATAL,
          "write(%d, 0x%x, %d) failed", file, buffer, count
        );

    return nbytes;
}
