/*
 *  cklength - check the length of lines in a file
 *
 *  This program check to see if the files passed to it on the command line
 *  contain a line which exceeds the maximum allowable length.  The default
 *  maximum line length is 80.
 *
 *  usage:  cklength  [ -v ] [ arg ... ] files...
 *           -l length   -- maximum line length
 *           -v          -- verbose
 */

#define GETOPTARGS "l:nNv"

char *USAGE = "\
usage:  cklength  [ -v ] [ arg ... ] files... \n\
            -l length   -- maximum line length\n\
            -n          -- report line numbers for offending lines\n\
            -N          -- report line numbers and length for offending lines\n\
            -v          -- verbose\n\
\n\
Print the name of files which have at least 1 line which exceeds the\n\
maximum line length.  The default maximum line length is 80.\n\
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


#define BUFFER_SIZE     512

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
int report_line_numbers = FALSE;        /* report line numbers of offenders */
int report_line_length = FALSE;         /* report line length of offenders */

int line_length = 80;                   /* maximum allowable line length */

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
            case 'l':                            /* line length */
                line_length = atoi( optarg );
                if ( line_length < 0 || line_length > BUFFER_SIZE )
                  error(ERR_FATAL, "(%d) is illegal line length\n",line_length);
                break;

            case 'n':                           /* toggle report_line_numbers */
                report_line_numbers = ! report_line_numbers;
                break;

            case 'N':                           /* toggle both reports */
                report_line_numbers = ! report_line_numbers;
                report_line_length  = ! report_line_length;
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
  char   *bptr;
  char    buffer[ BUFFER_SIZE ];
  int     line_number;
  int     length;
  int     count;
  int     rc = SUCCESS;  /* succeed by default */

  in = fopen( arg, "r" );
  if (!in)
    error( ERR_ERRNO | ERR_FATAL, "Unable to open file (%s)\n", arg );

  count = 0;

  for ( line_number=1 ; ; line_number++ ) {
    bptr = fgets( buffer, BUFFER_SIZE, in );
    if (!bptr)
      break;

    /*
     *  Don't count the carriage return.
     */

    length = strlen( buffer ) - 1;

    if ( length <= line_length )
      continue;

    if ( count == 0 ) {
      fprintf( stderr, "%s\n", arg );
      if ( !report_line_numbers )
        break;
    }

    if ( verbose )
      fprintf( stderr, "TOO LONG:%d: %s\n", line_number, buffer );

    if ( report_line_numbers ) {
      if ( report_line_length )
        fprintf( stderr, "%d: %d\n" , line_number, length );
      else
        fprintf( stderr, "%d\n" , line_number );
    }

    count++;

  }

  fclose( in );
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
