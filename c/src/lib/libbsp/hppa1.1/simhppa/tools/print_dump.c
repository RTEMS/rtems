/*
 *  print_dump
 *
 *  COPYRIGHT (c) 1994 by Division Incorporated
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define GETOPTARGS "v"

char *USAGE = "\
usage:  print_dump  [ -v ] \n\
            -v          -- verbose\n\
 Reads HP simulator 'memdump' output of 'print_buffer' structure
 on stdin.  Dumps it out in vanilla ASCII.
";

#include <stdio.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <stdarg.h>

#include "config.h"

#ifndef VMS
#ifndef HAVE_STRERROR
#if defined(__linux__) && (__GLIBC__ < 2)
extern char *sys_errlist[];
#endif

#define strerror( _err ) \
  ((_err) < sys_nerr) ? sys_errlist [(_err)] : "unknown error"

#else   /* HAVE_STRERROR */
char *strerror ();
#endif
#else   /* VMS */
char *strerror (int,...);
#endif

#define Failed(x)       (((int) (x)) == -1)
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
typedef unsigned int    u32;

typedef union uval {
    u8      uv_chars[4];
    u16     uv_words[2];
    u32     uv_long;
    void    *uv_ptr[sizeof(long) / sizeof(void *)];
} uval_t;


/*
 * vars controlled by command line options
 */

int verbose = FALSE;                    /* be verbose */

extern char *optarg;                    /* getopt(3) control vars */
extern int optind, opterr;
extern int errno;

char *progname;                         /* for error() */

void error(int errn, ...);

#define ERR_ERRNO  (1<<((sizeof(int) * 8) - 2)) /* hi bit; use 'errno' */
#define ERR_FATAL  (ERR_ERRNO / 2)              /* error is fatal; no return */
#define ERR_ABORT  (ERR_ERRNO / 4)              /* error is fatal; abort */
#define ERR_MASK   (ERR_ERRNO | ERR_FATAL | ERR_ABORT) /* all */

int  process(void);
void prchar(unsigned int ch);


int
main(int argc, char **argv, char **env)
{
    register int c;
    int showusage = FALSE;                      /* usage error? */

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

    return process();
}


/*
 * process(arg)
 *
 * Input looks like this
 *
 *   Starting address: 00000001.480035a0
 *   -----------------------------------

+0000 / 0d0a0d0a 2a2a2a20 53454d20 54455354 202d2d20 4e4f4445 2032202a 2a2a0d0a
+0020 / 73703a20 30783433 30303030 31300d0a 30783438 30613161 38383a20 676f7420
  ....
+0b40 / xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx

 *
 * The 'xxxxxxxxx' shows up if the page has not been allocated.
 */

int
process(void)
{
    uval_t b[8];
    u32   ignore;
    char *p;
    int  i;
    int  failed_once;

    char line[1024];

#define PRINT_BUFFER_SIZE (16 * 1024)
    struct {
        int  index;
        int  size;
        u8   buffer[PRINT_BUFFER_SIZE];
    } print_buffer;

    /* we stuff the data into print_buffer using memcpy() */
    p = (char *) &print_buffer;

    failed_once = 0;

    while (gets(line))
    {
        char *cp;

        /* hack; deal with the 'xxxxxxxx' problem noted above */
        for (cp=line; *cp; cp++)
            if (*cp == 'x')
                *cp = '0';

        if (*line != '+')
            continue;
        if (sscanf(line, "+%x / %x %x %x %x %x %x %x %x\n",
                   &ignore,
                   &b[0].uv_long,
                   &b[1].uv_long,
                   &b[2].uv_long,
                   &b[3].uv_long,
                   &b[4].uv_long,
                   &b[5].uv_long,
                   &b[6].uv_long,
                   &b[7].uv_long) != 9)
        {
            if (failed_once)
                error(ERR_FATAL, "2nd format problem; giving up");
            error(0, "format problem in line: `%s`", line);
            failed_once = 1;
        }

        memcpy((void *) p, (void *) b, sizeof(b));
        p += sizeof(b);
    }

    if (verbose)
        printf("buffer size: %d\n", print_buffer.size);

    if (print_buffer.size < 0)
        error(ERR_FATAL, "size is too small");

    if (print_buffer.size != sizeof(print_buffer.buffer))
    {
        error(ERR_FATAL, "buffer size mismatch, expected %d",
              sizeof(print_buffer.buffer));
        /* XXX we really should just dynamically allocate the buffer */
    }

    i = print_buffer.index + 1;
    while (i != print_buffer.index)
    {
        unsigned int c;
        c = print_buffer.buffer[i++];
        if (c && (c != '\r'))
            prchar(c);
        i %= print_buffer.size;
    }
    printf("\n");
    return 0;
}

/* de-controlify */
char *de_control[] = {
    "^@", "^A", "^B", "^C", "^D", "^E", "^F", "^G", "^H", "^I", "^J", "^K",
    "^L", "^M", "^N", "^O", "^P", "^Q", "^R", "^S", "^T", "^U", "^V", "^W",
    "^X", "^Y", "^Z", "^[", "^\\", "^]", "^~", "^_",
    " ", "!", "\"", "#", "$",  "%", "&", "'", "(", ")", "*", "+", ",", "-",
    ".", "/", "0",  "1", "2",  "3", "4", "5", "6", "7", "8", "9", ":", ";",
    "<", "=", ">",  "?", "@",  "A", "B", "C", "D", "E", "F", "G", "H", "I",
    "J", "K", "L",  "M", "N",  "O", "P", "Q", "R", "S", "T", "U", "V", "W",
    "X", "Y", "Z",  "[", "\\", "]", "^", "_", "`", "a", "b", "c", "d", "e",
    "f", "g", "h",  "i", "j",  "k", "l", "m", "n", "o", "p", "q", "r", "s",
    "t", "u", "v",  "w", "x",  "y", "z", "{", "|", "}", "~", "^?",
    "M-^@", "M-^A", "M-^B", "M-^C", "M-^D", "M-^E", "M-^F", "M-^G",
    "M-^H", "M-^I", "M-^J", "M-^K", "M-^L", "M-^M", "M-^N", "M-^O",
    "M-^P", "M-^Q", "M-^R", "M-^S", "M-^T", "M-^U", "M-^V", "M-^W",
    "M-^X", "M-^Y", "M-^Z", "M-^[", "M-^\\", "M-^]", "M-^~", "M-^_",
    "M- ", "M-!", "M-\"", "M-#", "M-$", "M-%", "M-&", "M-'",
    "M-(", "M-)", "M-*", "M-+", "M-,", "M--", "M-.", "M-/",
    "M-0", "M-1", "M-2", "M-3", "M-4", "M-5", "M-6", "M-7",
    "M-8", "M-9", "M-:", "M-;", "M-<", "M-=", "M->", "M-?",
    "M-@", "M-A", "M-B", "M-C", "M-D", "M-E", "M-F", "M-G",
    "M-H", "M-I", "M-J", "M-K", "M-L", "M-M", "M-N", "M-O",
    "M-P", "M-Q", "M-R", "M-S", "M-T", "M-U", "M-V", "M-W",
    "M-X", "M-Y", "M-Z", "M-[", "M-\\", "M-]", "M-^", "M-_",
    "M-`", "M-a", "M-b", "M-c", "M-d", "M-e", "M-f", "M-g",
    "M-h", "M-i", "M-j", "M-k", "M-l", "M-m", "M-n", "M-o",
    "M-p", "M-q", "M-r", "M-s", "M-t", "M-u", "M-v", "M-w",
    "M-x", "M-y", "M-z", "M-{", "M-|", "M-}", "M-~", "M-^?"
};

/*
 * prchar(ch); print ch in a readable format, ie ^X or X or ~^X or DEL, etc.
 */

void
prchar(unsigned int ch)
{
    if (isprint(ch) || isspace(ch))
        putchar(ch);
    else
        printf("%s", de_control[ch]);
}


/*
 * error(errn, arglist)
 *      report an error to stderr using printf(3) conventions.
 *      Any output is preceded by '<progname>: '
 *
 * Uses ERR_EXIT  bit to request exit(errn)
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
            error(0, local_errno ? "fatal error, exiting" : "exiting");
            exit(local_errno);
        }
        else
        {
            error(0, "fatal error, aborting");
            abort();
        }
    }
}

